#include "SingleChart.h"
#include <QtCore/QMimeData>
#include <QtWidgets/QListWidget>
#include <QtCore/QDebug>
#include <QtWidgets/QMessageBox>
#include <QtGui/QWheelEvent>
#include "PlotContainer.h"
#include "Plot.h"
#include "JsonFieldNames.h"

using namespace QtCharts;

SingleChart::SingleChart(QString units, std::shared_ptr<PlotMgr> plotmgr, const QString &plotname, QWidget *parent) : QChartView(parent)
{
	plotmgr_ = plotmgr;
	plotname_ = plotname;
	units_ = units;
	time_ = nullptr;
	legend_ = nullptr;
	hand_cursor_ = false;

	setRubberBand(QChartView::RectangleRubberBand);

	plot_added_connection_ = connect(plotmgr.get(), &PlotMgr::plotAdded, this, &SingleChart::plotAddedDetected);

	initChart();
	attachPlot();
}

SingleChart::~SingleChart()
{
	if (plot_ != nullptr)
	{
		disconnect(connection_);
	}

	disconnect(plot_added_connection_);
}

void SingleChart::wheelEvent(QWheelEvent* ev)
{
	qreal factor = ev->angleDelta().y() > 0 ? 0.5 : 2.0;
	chart()->zoom(factor);
	ev->accept();

	QChartView::wheelEvent(ev);

}

void SingleChart::mousePressEvent(QMouseEvent* ev)
{
#ifdef NOTYET
	if (ev->button() == Qt::MiddleButton)
	{
		qDebug() << "MousePress";
		setCursor(Qt::OpenHandCursor);
		hand_cursor_ = false;
		last_pos_ = ev->pos();
		ev->accept();
	}
#endif
	QChartView::mousePressEvent(ev);
}

void SingleChart::mouseMoveEvent(QMouseEvent* ev)
{
	mouse_ = chart()->mapToValue(QPointF(ev->x(), ev->y()));

	// pan the chart with a middle mouse drag
	if (hand_cursor_)
	{
		auto diff = this->chart()->mapToValue(ev->pos()) - this->chart()->mapToValue(last_pos_);
		qDebug() << "MouseMove: " << diff;

		this->chart()->scroll(diff.x(), diff.y());
		total_scroll_x_ += diff.x();
		total_scroll_y_ += diff.y();

		last_pos_ = ev->pos();
		ev->accept();
	}

	QChartView::mouseMoveEvent(ev);
}

void SingleChart::mouseReleaseEvent(QMouseEvent* ev)
{
	if (hand_cursor_)
	{
		setCursor(Qt::ArrowCursor);
		hand_cursor_ = false;
		ev->accept();
	}

	QChartView::mouseReleaseEvent(ev);
}

void SingleChart::plotAddedDetected(const QString& plotname)
{
	if (plotname_ == plotname)
		attachPlot();
}

bool SingleChart::attachPlot()
{
	if (plot_ != nullptr && plot_->name() == plotname_)
		return true;

	assert(plot_ == nullptr);
	plot_ = plotmgr_->getPlot(plotname_);
	if (plot_ != nullptr)
	{
		is_complete_ = plot_->isComplete();
		connection_ = connect(plot_.get(), &Plot::stateChanged, this, &SingleChart::plotStateChanged);
	}

	return plot_ != nullptr;
}

QJsonObject SingleChart::getJSONDesc() const
{
	QJsonObject obj;

	obj[JsonFieldNames::NodeList] = QJsonArray::fromStringList(nodes_);

	return obj;
}

bool SingleChart::restoreFromJson(const QJsonObject& obj)
{
	if (!obj.contains(JsonFieldNames::NodeList) || !obj.value(JsonFieldNames::NodeList).isArray())
		return false;

	nodes_.clear();

	QJsonArray nodes = obj.value(JsonFieldNames::NodeList).toArray();
	for (int i = 0; i < nodes.size(); i++)
	{
		if (nodes.at(i).isString())
			nodes_.push_back(nodes.at(i).toString());
	}

	resetNodes();

	return true;
}

void SingleChart::initChart()
{
	tminv_ = std::numeric_limits<double>::max();
	tmaxv_ = std::numeric_limits<double>::min();

	total_scroll_x_ = 0;
	total_scroll_y_ = 0;


	callout_ = nullptr;
	first_ = nullptr;

	left_axis_count_ = 0;
	right_axis_count_ = 0;

	editor_ = nullptr;

	first_point_ = true;

	first_line_ = nullptr;
	second_line_ = nullptr;

	rect_item_ = nullptr;

	chart()->setDropShadowEnabled(false);
	chart()->setAnimationOptions(QChart::AnimationOption::NoAnimation);
	setRubberBand(QChartView::RectangleRubberBand);
	setAcceptDrops(true);
}

void SingleChart::highlight(bool b)
{
	chart()->setDropShadowEnabled(b);
}

void SingleChart::setUnits(QString units)
{
	QString oldunits = units_;

	units_ = units;
	for (auto axis : chart()->axes())
	{
		QString newname = axis->titleText().replace(oldunits, units_);
		axis->setTitleText(newname);
	}
}

void SingleChart::focusInEvent(QFocusEvent* ev)
{
	PlotContainer* c = dynamic_cast<PlotContainer*>(parent());
	if (c != nullptr)
		c->childFocused(this);
}


void SingleChart::keyPressEvent(QKeyEvent* ev)
{
	switch (ev->key()) {
	case Qt::Key_Plus:
		chart()->zoomIn();
		break;
	case Qt::Key_Minus:
		chart()->zoomOut();
		break;
	case Qt::Key_Left:
		if (ev->modifiers() & Qt::ShiftModifier)
		{
			chart()->scroll(-50, 0);
			total_scroll_x_ -= 50;
		}
		else
		{
			chart()->scroll(-10, 0);
			total_scroll_x_ -= 10;
		}
		break;
	case Qt::Key_Right:
		if (ev->modifiers() & Qt::ShiftModifier)
		{
			chart()->scroll(50, 0);
			total_scroll_x_ += 50;
		}
		else
		{
			chart()->scroll(10, 0);
			total_scroll_x_ += 10;
		}
		break;
	case Qt::Key_Up:
		if (ev->modifiers() & Qt::ShiftModifier)
		{
			chart()->scroll(0, 50);
			total_scroll_y_ += 50;
		}
		else
		{
			chart()->scroll(0, 10);
			total_scroll_y_ += 10;
		}
		break;
	case Qt::Key_Down:
		if (ev->modifiers() & Qt::ShiftModifier)
		{
			chart()->scroll(0, -50);
			total_scroll_y_ -= 50;
		}
		else
		{
			chart()->scroll(0, -10);
			total_scroll_y_ -= 10;
		}
		break;
	case Qt::Key_Home:
		chart()->scroll(-total_scroll_x_, -total_scroll_y_);
		chart()->zoomReset();
		total_scroll_x_ = 0;
		total_scroll_y_ = 0;
		for (auto callout : callouts_)
			delete callout;
		callouts_.clear();
		first_valid_ = false;
		second_valid_ = false;
		first_point_ = true;
		clearAnnotations();
		break;
	case Qt::Key_Space:
		if (first_point_)
		{
			first_value_ = mouse_.rx();
			first_point_ = false;

			if (first_line_ != nullptr)
			{
				chart()->scene()->removeItem(first_line_);
				delete first_line_;
			}

			QPen p(QColor(0x00, 0xFF, 0x00, 0xFF));
			p.setWidth(4);
			first_line_ = createLine(first_value_, p);
		}
		else
		{
			second_value_ = mouse_.rx();
			first_point_ = true;
			if (second_line_ != nullptr)
			{
				chart()->scene()->removeItem(second_line_);
				delete second_line_;
			}

			QPen p(QColor(0x00, 0x00, 0xFF, 0xFF));
			p.setWidth(4);
			second_line_ = createLine(second_value_, p);
			displayAverages();
		}
		break;

	case Qt::Key_R:
		dataAdded();
		break;

	default:
		QGraphicsView::keyPressEvent(ev);
		break;
	}
}

void SingleChart::clearAnnotations()
{
	if (first_line_ != nullptr)
	{
		chart()->scene()->removeItem(first_line_);
		delete first_line_;
	}
	if (second_line_ != nullptr)
	{
		chart()->scene()->removeItem(second_line_);
		delete second_line_;
	}
	if (rect_item_ != nullptr)
	{
		chart()->scene()->removeItem(rect_item_);
		delete rect_item_;
	}

	for (auto item : text_items_)
	{
		chart()->scene()->removeItem(item);
		delete item;
	}
	text_items_.clear();
}

int SingleChart::findClosest(QVector<QPointF> &pts, double v)
{
	double dist = std::numeric_limits<double>::max();
	int ret = -1;


	for (int i = 0; i < pts.size() ; i++)
	{
		double diff = std::fabs(pts[i].rx() - v);
		if (diff < dist)
		{
			ret = i;
			dist = diff;
		}
	}

	return ret;
}

QStringList SingleChart::computeAverages()
{
	QStringList list;

	for (auto series : chart()->series())
	{
		QLineSeries* line = dynamic_cast<QLineSeries *>(series);
		if (line == nullptr)
			continue;

		QVector<QPointF> pts = line->pointsVector();
		int start = findClosest(pts, first_value_);
		int end = findClosest(pts, second_value_);

		if (start == -1 || end == -1)
			continue;

		double total = 0.0;
		double minv = std::numeric_limits<double>::max();
		double maxv = std::numeric_limits<double>::min();
		for (int i = start; i <= end; i++)
		{
			double v = pts[i].ry();
			total += v;

			minv = std::min(v, minv);
			maxv = std::max(v, maxv);
		}

		double avg = total / static_cast<double>((end - start + 1));
		QString one = series->name() + ":";
		one += "avg= " + QString::number(avg, 'f', 1);
		one += "min= " + QString::number(minv, 'f', 1);
		one += "max= " + QString::number(maxv, 'f', 1);
		list << one;
	}
	return list;
}

void SingleChart::displayAverages()
{
	int topbottom = 4;
	int leftright = 8;
	int fontsize = 22;
	QRectF rect = chart()->plotArea();
	QStringList avg = computeAverages();
	QPointF p1 = chart()->mapToPosition(QPointF(first_value_, 0.0));
	QPointF p2 = chart()->mapToPosition(QPointF(second_value_, 0.0));
	double middle = (p1.rx() + p2.rx()) / 2;

	for (auto item : text_items_)
	{
		chart()->scene()->removeItem(item);
		delete item;
	}
	text_items_.clear() ;

	if (rect_item_ != nullptr)
	{
		chart()->scene()->removeItem(rect_item_);
		delete rect_item_;
	}

	if (avg.size() > 0)
	{
		int maxwidth = 0;
		double height = 0;
		for (const QString& txt : avg)
		{
			QGraphicsSimpleTextItem* item = chart()->scene()->addSimpleText(txt);
			QFont f = item->font();
			f.setPointSize(fontsize);
			item->setFont(f);
			QFontMetrics metrics(f);

			int width = metrics.horizontalAdvance(txt);
			maxwidth = std::max(maxwidth, width);

			height += metrics.lineSpacing();
			text_items_.push_back(item);
		}

		maxwidth += leftright * 2;
		height += topbottom * 2;

		double ypos = rect.center().ry() - height / 2 + topbottom;
		for (auto item : text_items_)
		{
			QFont f = item->font();
			QFontMetrics metrics(f);
			int width = metrics.horizontalAdvance(item->text());
			double xpos = middle - width / 2;
			item->setPos(xpos, ypos);
			ypos += metrics.lineSpacing();
		}

		QRectF rf(middle - maxwidth / 2, rect.center().ry() - height / 2, maxwidth, height);
		QPen pen(QColor(0xFF, 0xFF, 0xFF, 0xFF));
		pen.setWidth(4);
		QBrush brush(QColor(0x00, 0x00, 0xFF, 0x40));

		rect_item_ = chart()->scene()->addRect(rf, pen, brush);
	}
}

QGraphicsLineItem* SingleChart::createLine(double stime, QPen p)
{
	QRectF rect = chart()->plotArea();
	QPointF pt = chart()->mapToPosition(QPointF(stime, 0.0));
	return chart()->scene()->addLine(pt.rx(), rect.top(), pt.rx(), rect.bottom(), p);
}

void SingleChart::setMinMax(const std::string& name, double minv, double maxv)
{
	auto it = min_max_.find(name);
	if (it == min_max_.end())
		min_max_[name] = std::make_pair(std::numeric_limits<double>::max(), std::numeric_limits<double>::min());

	auto pair = min_max_[name];
	if (minv < pair.first)
		pair.first = minv;

	if (maxv > pair.second)
		pair.second = maxv;

	min_max_[name] = pair;
}

void SingleChart::getMinMax(const std::string& name, double &minv, double &maxv)
{
	auto it = min_max_.find(name);
	if (it == min_max_.end())
	{
		minv = 0.0;
		maxv = 10.0;
	}
	else
	{
		minv = min_max_[name].first;
		maxv = min_max_[name].second;
	}

	if (std::fabs(minv - maxv) < 0.1)
	{
		if (std::fabs(minv) < 0.01)
		{
			minv = -1.0;
			maxv = 1.0;
		}
		else if (minv < 0)
		{
			maxv = 0.0;
			minv *= 2.0;
		}
		else
		{
			minv = 0.0;
			maxv *= 2.0;
		}
	}
}

void SingleChart::dragEnterEvent(QDragEnterEvent* ev)
{
	setBackgroundRole(QPalette::Highlight);
	ev->setDropAction(Qt::DropAction::CopyAction);
	ev->acceptProposedAction();
}

void SingleChart::dragMoveEvent(QDragMoveEvent* ev)
{
	if (plot_->state() != Plot::State::Reading)
		ev->acceptProposedAction();
}

void SingleChart::dragLeaveEvent(QDragLeaveEvent* ev)
{
	setBackgroundRole(QPalette::Background);
}

void SingleChart::dropEvent(QDropEvent* ev)
{
	QString text;
	static const char* listMimeType = "application/x-qabstractitemmodeldatalist";

	const QMimeData* data = ev->mimeData();
	if (data->hasFormat(listMimeType))
	{
		QByteArray encoded = data->data(listMimeType);
		QDataStream stream(&encoded, QIODevice::ReadOnly);

		while (!stream.atEnd())
		{
			int row, col;
			QMap<int, QVariant> roleDataMap;
			stream >> row >> col >> roleDataMap;

			QVariant v = roleDataMap[Qt::ItemDataRole::DisplayRole];
			insertNode(v.toString());
		}
	}
	setBackgroundRole(QPalette::Background);
}

QValueAxis* SingleChart::findAxis(QString node)
{
	for (auto axis : chart()->axes())
	{
		QString axisname = axis->titleText();
		if (axisname == node)
			return dynamic_cast<QValueAxis *>(axis);
	}

	return nullptr;
}

QLineSeries* SingleChart::findSeries(QString node)
{
	for (auto series : chart()->series())
	{
		if (series->name() == node)
			return dynamic_cast<QLineSeries*>(series);
	}
	return nullptr;
}

QString SingleChart::nodeToAxis(QString node)
{
	QString ret;

	int left = node.indexOf('(');
	if (left == -1)
	{
		ret = node;
	}
	else
	{
		int right = node.indexOf(')', left + 1);
		if (right == -1)
		{
			ret = node;
		}
		else
		{
			ret = node.mid(left + 1, right - left - 1);
		}
	}

	return ret;
}

void SingleChart::createTimeAxis()
{
	time_ = new QValueAxis();
	time_->setLabelsVisible(true);
	time_->setTickCount(10);
	time_->setTitleText("time (s)");
	time_->setTitleVisible(true);
	chart()->addAxis(time_, Qt::AlignBottom);
}

void SingleChart::createLegend()
{
	QLegend* legend = chart()->legend();
	legend->setVisible(true);
	legend->setAlignment(Qt::AlignBottom);
	legend->setMarkerShape(QLegend::MarkerShape::MarkerShapeCircle);
	QFont font = legend->font();
	font.setPointSize(8);
	font.setBold(true);
	legend->setFont(font);
}

void SingleChart::insertNode(const QString &node)
{
	if (!nodes_.contains(node))
		nodes_.push_back(node);

	if (plot_ == nullptr)
		return;

	size_t colidx = plot_->getColumnIndex(node);
	if (colidx == -1)
	{
		//
		// This should have never happened.  We are being safe here, but if we get to
		// this point, we should have a valid value from a valid plot
		//
		QString msg = "Could not add variable '";
		msg += node;
		msg += "' to the graph";
		QMessageBox box(QMessageBox::Icon::Critical,
			"Error", msg, QMessageBox::StandardButton::Ok);
		box.exec();
		return;
	}

	if (colidx == 0)
	{
		//
		// Do not add the independent variable (always assumed to be index zero) to
		// the plot as it just makes a straight line
		//
		QString msg = "Could not add variable '";
		msg += node;
		msg += "' to the graph - it is the independent variable";
		QMessageBox box(QMessageBox::Icon::Critical,
			"Error", msg, QMessageBox::StandardButton::Ok);
		box.exec();
		return;
	}

	if (time_ == nullptr)
		createTimeAxis();

	if (legend_ == nullptr)
		createLegend();

	//
	// Try to guess an axis name to keep all like value plotted against
	// a single axis.  For instance, all position values should be plotted
	// against a single position axis.  All velocity values should be
	// plotted against a single velocity axis.
	//
	QString axisname = nodeToAxis(node);
	QValueAxis* axis = findAxis(axisname);
	if (axis == nullptr)
	{
		axis = new QValueAxis();

		axis->setLabelsVisible(true);
		axis->setTickCount(10);
		axis->setTitleText(axisname);
		axis->setTitleVisible(true);

		if (left_axis_count_ <= right_axis_count_) {
			chart()->addAxis(axis, Qt::AlignLeft);
			left_axis_count_++;
		}
		else
		{
			chart()->addAxis(axis, Qt::AlignRight);
			right_axis_count_++;
		}
	}

	double maxv = std::numeric_limits<double>::min();
	double minv = std::numeric_limits<double>::max();

	QLineSeries* series = findSeries(node);
	if (series != nullptr)
	{
		QString msg = "Could not add variable '";
		msg += node;
		msg += "' to the graph - it is already in the graph";
		QMessageBox box(QMessageBox::Icon::Critical,
			"Error", msg, QMessageBox::StandardButton::Ok);
		box.exec();
		return;
	}

	series = new QLineSeries();
	if (first_ == nullptr)
		first_ = series;

	(void)connect(series, &QLineSeries::hovered, this, [series, this](const QPointF& pt, bool state) { this->seriesHover(series, pt, state); });
	(void)connect(series, &QLineSeries::clicked, this, [series, this](const QPointF& pt) { this->seriesClick(series, pt); });


	series->setName(node);

	setMinMax(axisname.toStdString(), minv, maxv);

	chart()->addSeries(series);

	time_->setRange(tminv_, tmaxv_);

	getMinMax(axisname.toStdString(), minv, maxv);
	axis->setRange(minv, maxv);

	series->attachAxis(time_);
	series->attachAxis(axis);

	axis->applyNiceNumbers();
	time_->applyNiceNumbers();

	QFont font = chart()->titleFont();
	font.setPointSize(20);
	font.setBold(true);
	chart()->setTitleFont(font);
	chart()->setTitle(title_);

	// 
	// Add any data that variable contains
	//
	dataAdded();
}

void SingleChart::dataReset()
{
	tminv_ = std::numeric_limits<double>::max();
	tmaxv_ = std::numeric_limits<double>::min();
	min_max_.clear();

	for (QAbstractSeries* series : chart()->series())
	{
		QLineSeries* line = dynamic_cast<QLineSeries*>(series);
		if (line != nullptr)
		{
			line->clear();
		}
	}
}

void SingleChart::seriesHover(QLineSeries* ser, const QPointF& pt, bool state)
{
	QPointF npt = chart()->mapToPosition(pt, ser);
	npt = chart()->mapToValue(npt, first_);

	(void)ser;
	if (callout_ == nullptr)
		callout_ = new Callout(chart());

	if (state)
	{
		QString text;

		text += "Time: " + QString::number(pt.x(), 'f', 2) + "\n";
		text += ser->name() + ": " + QString::number(pt.y(), 'f', 1);

		callout_->setText(text);
		callout_->setAnchor(npt);
		callout_->setZValue(11);
		callout_->updateGeometry();
		callout_->show();
	}
	else
		callout_->hide();
}

void SingleChart::seriesClick(QLineSeries* ser, const QPointF& pt)
{
	(void)ser;
	(void)pt;

	callouts_.push_back(callout_);
	callout_ = nullptr;
}

void SingleChart::editTitle()
{
	if (editor_ == nullptr)
	{
		editor_ = new TabEditName(this);
		(void)connect(editor_, &TabEditName::returnPressed, this, &SingleChart::editorDone);
		(void)connect(editor_, &TabEditName::escapePressed, this, &SingleChart::editorAborted);

		QFont font = editor_->font();
		font.setPointSize(20);
		font.setBold(true);
		editor_->setFont(font);

		editor_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	}

	QRect r(20, 28, width() - 40, 40);
	editor_->setGeometry(r);
	editor_->setText(title_);
	editor_->setVisible(true);
	editor_->setFocus();
	editor_->selectAll();
}

void SingleChart::editorDone()
{
	setTitle(editor_->text());
	editor_->setVisible(false);
}

void SingleChart::editorAborted()
{
	editor_->setVisible(false);
}

void SingleChart::setTitle(QString t)
{
	title_ = t;

	QFont font = chart()->titleFont();
	font.setPointSize(20);
	font.setBold(true);
	chart()->setTitleFont(font);
	chart()->setTitle(title_);
}

void SingleChart::dataAdded()
{
	for (QAbstractSeries* series : chart()->series())
	{
		double maxv = std::numeric_limits<double>::min();
		double minv = std::numeric_limits<double>::max();

		QLineSeries* line = dynamic_cast<QLineSeries*>(series);
		if (line != nullptr)
		{
			line->clear();
			size_t colidx = plot_->getColumnIndex(line->name());
			if (colidx < plot_->numColumns())
			{
				for (size_t i = 0; i < plot_->numRows(); i++)
				{
					const QVector<double>& row = plot_->getRow(i);
					double t = row[0];
					double d = row[colidx];

					line->append(t, d);

					if (d > maxv)
						maxv = d;

					if (d < minv)
						minv = d;

					if (t > tmaxv_)
						tmaxv_ = t;

					if (t < tminv_)
						tminv_ = t;
				}
			}

			QString axisname = nodeToAxis(line->name());

			setMinMax(axisname.toStdString(), minv, maxv);
			getMinMax(axisname.toStdString(), minv, maxv);

			time_->setRange(tminv_, tmaxv_);

			QValueAxis* axis = findAxis(axisname);
			axis->setRange(minv, maxv);
		}
	}
}

void SingleChart::resetNodes()
{
	qDebug() << "resetNodes";
	if (time_ != nullptr)
	{
		chart()->removeAxis(time_);
		time_ = nullptr;
	}

	auto alist = chart()->axes();
	for (auto a : alist)
		chart()->removeAxis(a);

	auto slist = chart()->series();
	for (auto s : slist)
		chart()->removeSeries(s);

	for (const QString node : nodes_)
		insertNode(node);
}

void SingleChart::plotStateChanged(Plot::State oldst, Plot::State newst)
{
	if (is_complete_ == false && plot_->isComplete())
	{
		resetNodes();
	}

	is_complete_ = plot_->isComplete();
}