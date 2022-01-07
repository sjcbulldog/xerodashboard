#include "PlotContainer.h"
#include <QtCore/QJsonObject>
#include "JsonFieldNames.h"

PlotContainer::PlotContainer(std::shared_ptr<PlotMgr> plotmgr, const QString &plotname, QWidget *parent) : QWidget(parent)
{
	plotmgr_ = plotmgr;
	plotname_ = plotname;
	selected_ = nullptr;
	layout_ = new QGridLayout(this);

	setLayout(layout_);
	setMouseTracking(true);
	setFocusPolicy(Qt::ClickFocus);

	units_ = "in";

}

PlotContainer::~PlotContainer()
{
	for (int i = 0; i < charts_.size(); i++)
	{
		delete charts_.at(i);
	}
	charts_.clear();
}

void PlotContainer::createDefaultChart()
{
	SingleChart* ch = new SingleChart(units_, plotmgr_, plotname_, this);
	charts_.push_back(ch);
	layout_->addWidget(ch, 0, 0);
	arrangeCharts();
}

QJsonObject PlotContainer::getJSONDesc() const 
{
	QJsonObject obj;
	QJsonArray arr;

	for (int i = 0; i < charts_.size(); i++)
	{
		auto ch = charts_.at(i);
		arr.push_back(ch->getJSONDesc());
	}

	obj[JsonFieldNames::Charts] = arr;
	return obj;
}

bool PlotContainer::restoreFromJson(const QJsonArray& charts)
{
	for (int i = 0; i < charts.size(); i++)
	{
		if (charts.at(i).isObject())
		{
			QJsonObject obj = charts.at(i).toObject();

			SingleChart* ch = new SingleChart(units_, plotmgr_, plotname_, this);
			charts_.push_back(ch);
			layout_->addWidget(ch);
			arrangeCharts();

			if (obj.contains(JsonFieldNames::NodeList) && obj.value(JsonFieldNames::NodeList).isArray())
			{
				QJsonArray nodes = obj.value(JsonFieldNames::NodeList).toArray();
				for (int j = 0; j < nodes.count(); j++)
				{
					qDebug() << "NODE " << j << ": " << nodes.at(j).toString();
					if (nodes.at(j).isString())
					{
						ch->insertNode(nodes.at(j).toString());
					}
				}
			}
		}
	}

	return true;
}

void PlotContainer::setUnits(QString units)
{
	units_ = units;

	for (auto chart : charts_)
		chart->setUnits(units_);
}

void PlotContainer::childFocused(SingleChart* ch)
{
	if (selected_ != nullptr)
		selected_->highlight(false);

	auto it = std::find(charts_.begin(), charts_.end(), ch);
	if (it != charts_.end())
	{
		selected_ = ch;
		selected_->highlight(true);
	}
}

void PlotContainer::removeAllCharts()
{
	while (charts_.size() > 0)
	{
		SingleChart* ch = charts_.at(0);
		removeChart(ch);
	}
}

void PlotContainer::removeChart(SingleChart* ch)
{
	auto it = std::find(charts_.begin(), charts_.end(), ch);
	if (it != charts_.end())
	{
		layout_->removeWidget(selected_);
		charts_.erase(it);
		delete ch;

		arrangeCharts();
	}
}

void PlotContainer::keyPressEvent(QKeyEvent *ev)
{
	if (ev->key() == Qt::Key::Key_Insert)
	{
		if (charts_.size() == 9)
			return;

		SingleChart* ch = new SingleChart(units_, plotmgr_, plotname_, this);
		charts_.push_back(ch);
		layout_->addWidget(ch);
		arrangeCharts();
	}
	else if (ev->key() == Qt::Key::Key_Delete)
	{
		removeChart(selected_);
		if (charts_.size() > 0)
		{
			selected_ = charts_.at(0);
		}
	}
}

void PlotContainer::paintEvent(QPaintEvent* ev)
{
	QPainter paint(this);
	QRectF r(0, 0, width(), height());
	QBrush b(QColor(0xc0, 0xc0, 0xc0, 0xff));
	paint.setBrush(b);
	paint.drawRect(r);
}

void PlotContainer::mouseDoubleClickEvent(QMouseEvent *ev)
{
	if (selected_ != nullptr)
		selected_->editTitle();
}

void PlotContainer::arrangeCharts()
{
	for (auto w : charts_)
		layout_->removeWidget(w);

	if (charts_.size() == 1)
	{
		layout_->addWidget(charts_[0], 0, 0);
		layout_->setRowStretch(0, 100);
		layout_->setRowStretch(1, 0);
		layout_->setRowStretch(2, 0);
		layout_->setColumnStretch(0, 100);
		layout_->setColumnStretch(1, 0);
		layout_->setColumnStretch(2, 0);
	}
	else if (charts_.size() == 2)
	{
		layout_->addWidget(charts_[0], 0, 0);
		layout_->addWidget(charts_[1], 1, 0);
		layout_->setRowStretch(0, 100);
		layout_->setRowStretch(1, 100);
		layout_->setRowStretch(2, 0);
		layout_->setColumnStretch(0, 100);
		layout_->setColumnStretch(1, 0);
		layout_->setColumnStretch(2, 0);
	}
	if (charts_.size() == 3)
	{
		layout_->addWidget(charts_[0], 0, 0);
		layout_->addWidget(charts_[1], 0, 1);
		layout_->addWidget(charts_[2], 1, 0, 1, 2);
		layout_->setColumnStretch(0, 100);
		layout_->setColumnStretch(1, 100);
		layout_->setColumnStretch(2, 0);
		layout_->setRowStretch(0, 100);
		layout_->setRowStretch(1, 100);
		layout_->setRowStretch(2, 0);
	}
	else if (charts_.size() == 4)
	{
		layout_->addWidget(charts_[0], 0, 0);
		layout_->addWidget(charts_[1], 0, 1);
		layout_->addWidget(charts_[2], 1, 0);
		layout_->addWidget(charts_[3], 1, 1);
		layout_->setColumnStretch(0, 100);
		layout_->setColumnStretch(1, 100);
		layout_->setColumnStretch(2, 0);
		layout_->setRowStretch(0, 100);
		layout_->setRowStretch(1, 100);
		layout_->setRowStretch(2, 0);
	}
	else if (charts_.size() == 5)
	{
		layout_->addWidget(charts_[0], 0, 0);
		layout_->addWidget(charts_[1], 0, 1);
		layout_->addWidget(charts_[2], 1, 0);
		layout_->addWidget(charts_[3], 1, 1);
		layout_->addWidget(charts_[4], 2, 0, 1, 2);
		layout_->setColumnStretch(0, 100);
		layout_->setColumnStretch(1, 100);
		layout_->setColumnStretch(2, 0);
		layout_->setRowStretch(0, 100);
		layout_->setRowStretch(1, 100);
		layout_->setRowStretch(2, 100);
	}
	else if (charts_.size() == 6)
	{
		layout_->addWidget(charts_[0], 0, 0);
		layout_->addWidget(charts_[1], 0, 1);
		layout_->addWidget(charts_[2], 1, 0);
		layout_->addWidget(charts_[3], 1, 1);
		layout_->addWidget(charts_[4], 2, 0);
		layout_->addWidget(charts_[5], 2, 1);
		layout_->setColumnStretch(0, 100);
		layout_->setColumnStretch(1, 100);
		layout_->setColumnStretch(2, 0);
		layout_->setRowStretch(0, 100);
		layout_->setRowStretch(1, 100);
		layout_->setRowStretch(2, 100);
	}
	else if (charts_.size() == 7)
	{
		layout_->addWidget(charts_[0], 0, 0);
		layout_->addWidget(charts_[1], 0, 1);
		layout_->addWidget(charts_[2], 0, 2);
		layout_->addWidget(charts_[3], 1, 0);
		layout_->addWidget(charts_[4], 1, 1);
		layout_->addWidget(charts_[5], 1, 2);
		layout_->addWidget(charts_[6], 2, 0, 1, 3);
		layout_->setColumnStretch(0, 100);
		layout_->setColumnStretch(1, 100);
		layout_->setColumnStretch(2, 100);
		layout_->setRowStretch(0, 100);
		layout_->setRowStretch(1, 100);
		layout_->setRowStretch(2, 100);
	}
	else if (charts_.size() == 8)
	{
		layout_->addWidget(charts_[0], 0, 0);
		layout_->addWidget(charts_[1], 0, 1);
		layout_->addWidget(charts_[2], 0, 2);
		layout_->addWidget(charts_[3], 1, 0);
		layout_->addWidget(charts_[4], 1, 1);
		layout_->addWidget(charts_[5], 1, 2);
		layout_->addWidget(charts_[6], 2, 0);
		layout_->addWidget(charts_[7], 2, 1, 1, 2);
		layout_->setColumnStretch(0, 100);
		layout_->setColumnStretch(1, 100);
		layout_->setColumnStretch(2, 100);
		layout_->setRowStretch(0, 100);
		layout_->setRowStretch(1, 100);
		layout_->setRowStretch(2, 100);
	}
	else if (charts_.size() == 9)
	{
		layout_->addWidget(charts_[0], 0, 0);
		layout_->addWidget(charts_[1], 0, 1);
		layout_->addWidget(charts_[2], 0, 2);
		layout_->addWidget(charts_[3], 1, 0);
		layout_->addWidget(charts_[4], 1, 1);
		layout_->addWidget(charts_[5], 1, 2);
		layout_->addWidget(charts_[6], 2, 0);
		layout_->addWidget(charts_[7], 2, 1);
		layout_->addWidget(charts_[8], 2, 2);
		layout_->setColumnStretch(0, 100);
		layout_->setColumnStretch(1, 100);
		layout_->setColumnStretch(2, 100);
		layout_->setRowStretch(0, 100);
		layout_->setRowStretch(1, 100);
		layout_->setRowStretch(2, 100);
	}
}