#include "DashView.h"
#include <QtGui/QDragEnterEvent>
#include <QtGui/QPainter>
#include <QtCore/QMimeData>
#include <QtCore/QDebug>
#include <QtWidgets/QMessageBox>
#include "XeroItemFrame.h"
#include "NTValueDisplayWidget.h"
#include "PlotWidget.h"
#include "PlotMgr.h"
#include "JsonFieldNames.h"

DashView::DashView(std::shared_ptr<PlotMgr> plotmgr, std::shared_ptr<NetworkTableManager> ntmgr, QWidget *parent) : QWidget(parent)
{
	plotmgr_ = plotmgr;
	ntmgr_ = ntmgr;
	setAcceptDrops(true);
}

DashView::~DashView()
{
}

void DashView::restoreLayout(const QJsonArray &arr)
{
	for (int i = 0; i < arr.size(); i++)
	{
		if (arr.at(i).isObject())
		{
			QJsonObject obj = arr.at(i).toObject();
			if (obj.contains(JsonFieldNames::Type) && obj.value(JsonFieldNames::Type).isString())
			{
				QString type = obj.value(JsonFieldNames::Type).toString();
				if (type == JsonFieldNames::TypeValueNTV)
				{
					createNTWidget(obj);
				}
				else if (type == JsonFieldNames::TypeValuePlot)
				{
					createPlot(obj);
				}
			}
		}
	}
}

void DashView::createPlot(const QJsonObject& obj)
{
	PlotWidget* vwid;

	QStringList keys = obj.keys();

	if (!obj.contains(JsonFieldNames::PlotName) || !obj.value(JsonFieldNames::PlotName).isString())
		return;

	if (!obj.contains(JsonFieldNames::Constainers) || !obj.value(JsonFieldNames::Constainers).isArray())
		return;

	XeroItemFrame* frame = new XeroItemFrame(this);
	(void)connect(frame, &XeroItemFrame::headerClicked, this, &DashView::frameWindowHeaderClicked);

	QString value = obj[JsonFieldNames::PlotName].toString();

	try {
		vwid = new PlotWidget(plotmgr_, ntmgr_, value, frame);
		vwid->restoreFromJson(obj);
	}
	catch (...)
	{
		delete frame;
		QString msg = "The plot '" + value + "' does not exist";
		QMessageBox::critical(this, "No Such Plot", msg);
		return;
	}

	frame->setWidget(vwid);
	frame->setVisible(true);
	frame->setTitle(value);

	if (obj.contains(JsonFieldNames::X) && obj.contains(JsonFieldNames::Y) &&
		obj.contains(JsonFieldNames::Width) && obj.contains(JsonFieldNames::Height) &&
		obj.value(JsonFieldNames::X).isDouble() && obj.value(JsonFieldNames::Y).isDouble() &&
		obj.value(JsonFieldNames::Width).isDouble() && obj.value(JsonFieldNames::Height).isDouble())

	{
		int x = obj.value(JsonFieldNames::X).toInt();
		int y = obj.value(JsonFieldNames::Y).toInt();
		int w = obj.value(JsonFieldNames::Width).toInt();
		int h = obj.value(JsonFieldNames::Height).toInt();

		if (x < 0)
			x = 0;
		if (y < 0)
			y = 0;

		frame->setGeometry(x, y, w, h);
	}

	if (!vwid->restoreFromJson(obj))
	{
		delete vwid;
		delete frame;
	}
}

void DashView::createNTWidget(const QJsonObject& obj)
{
	if (!obj.contains(JsonFieldNames::Path) || !obj.value(JsonFieldNames::Path).isString())
		return;

	QString value = obj.value(JsonFieldNames::Path).toString();

	QString title;
	int pos = value.lastIndexOf("/");
	if (pos == -1)
		title = value;
	else
		title = value.mid(pos + 1);

	XeroItemFrame* frame = new XeroItemFrame(this);
	(void)connect(frame, &XeroItemFrame::headerClicked, this, &DashView::frameWindowHeaderClicked);
	
	NTValueDisplayWidget* vwid = new NTValueDisplayWidget(ntmgr_, value, frame);
	frame->setWidget(vwid);
	frame->setVisible(true);
	frame->setTitle(title);

	if (obj.contains(JsonFieldNames::Display) && obj.value(JsonFieldNames::Display).isString())
	{
		vwid->setDisplayType(obj.value(JsonFieldNames::Display).toString());
	}

	if (obj.contains(JsonFieldNames::X) && obj.contains(JsonFieldNames::Y) && 
		obj.contains(JsonFieldNames::Width) && obj.contains(JsonFieldNames::Height) && 
		obj.value(JsonFieldNames::X).isDouble() && obj.value(JsonFieldNames::Y).isDouble() &&
		obj.value(JsonFieldNames::Width).isDouble() && obj.value(JsonFieldNames::Height).isDouble())

	{
		int x = obj.value(JsonFieldNames::X).toInt();
		int y = obj.value(JsonFieldNames::Y).toInt();
		int w = obj.value(JsonFieldNames::Width).toInt();
		int h = obj.value(JsonFieldNames::Height).toInt();

		if (x < 0)
			x = 0;
		if (y < 0)
			y = 0;

		if (x + w > width())
		{
			if (w < width())
				x = width() - w;
			else
				w = width();
		}

		if (y + h > height())
		{
			if (h < height())
				y = height() - h;
			else
				h = height();
		}

		frame->setGeometry(x, y, w, h);
	}
}

QJsonArray DashView::getJSONDesc()
{
	QJsonArray arr;

	for (auto obj : children())
	{
		XeroItemFrame* frame = dynamic_cast<XeroItemFrame*>(obj);

		if (frame != nullptr)
		{
			qDebug() << "Processing: " << frame->title();

			if (frame->children().size() != 1)
				continue;

			NTValueDisplayWidget* ntv = dynamic_cast<NTValueDisplayWidget*>(frame->children().at(0));
			if (ntv != nullptr)
			{
				QJsonObject desc = ntv->getJSONDesc();
				QRect r = frame->frameGeometry();
				desc[JsonFieldNames::X] = r.left();
				desc[JsonFieldNames::Y] = r.top();
				desc[JsonFieldNames::Width] = r.width();
				desc[JsonFieldNames::Height] = r.height();
				arr.push_back(desc);
				continue;
			}

			PlotWidget* plt = dynamic_cast<PlotWidget*>(frame->children().at(0));
			if (plt != nullptr)
			{
				QJsonObject desc = plt->getJSONDesc();
				QRect r = frame->frameGeometry();
				desc[JsonFieldNames::X] = r.left();
				desc[JsonFieldNames::Y] = r.top();
				desc[JsonFieldNames::Width] = r.width();
				desc[JsonFieldNames::Height] = r.height();
				arr.push_back(desc);
				continue;
			}
		}
	}

	return arr;
}

void DashView::paintEvent(QPaintEvent* ev)
{
	QPainter p(this);

	QBrush br(QColor(192, 192, 192));
	p.setBrush(br);
	p.drawRect(0, 0, width(), height());
}

void DashView::dragEnterEvent(QDragEnterEvent* ev)
{
	if (ev->mimeData()->hasFormat("text/plain"))
	{
		ev->acceptProposedAction();
	}
}

void DashView::dropEvent(QDropEvent* ev)
{
	QString value = ev->mimeData()->text();
	if (value.startsWith("NT:"))
	{
		QString title;
		int pos = value.lastIndexOf("/");
		if (pos == -1)
			title = value.mid(3);
		else
			title = value.mid(pos + 1);

		XeroItemFrame* frame = new XeroItemFrame(this);
		(void)connect(frame, &XeroItemFrame::headerClicked, this, &DashView::frameWindowHeaderClicked);

		NTValueDisplayWidget* vwid = new NTValueDisplayWidget(ntmgr_, value.mid(3), frame);
		frame->setWidget(vwid);
		frame->setVisible(true);
		frame->setTitle(title);

		frame->setGeometry(ev->pos().x(), ev->pos().y(), frame->width(), frame->height());
	}
	else if (value.startsWith("PLOT:"))
	{
		PlotWidget* vwid;
		XeroItemFrame* frame = new XeroItemFrame(this);
		(void)connect(frame, &XeroItemFrame::headerClicked, this, &DashView::frameWindowHeaderClicked);

		try {
			vwid = new PlotWidget(plotmgr_, ntmgr_, value.mid(5), frame);
		}
		catch (...)
		{
			delete frame;
			QString msg = "The plot '" + value.mid(5) + "' does not exist";
			QMessageBox::critical(this, "No Such Plot", msg);
			return;
		}

		frame->setWidget(vwid);
		frame->setVisible(true);
		frame->setTitle(value.mid(5));

		frame->setGeometry(ev->pos().x(), ev->pos().y(), frame->width(), frame->height());
	}
}

void DashView::frameWindowHeaderClicked(XeroItemFrame* frame, bool shift)
{
	if (!shift)
	{
		for (auto sframe : selected_)
			sframe->setSelected(false);

		selected_.clear();
	}

	selected_.push_back(frame);
	frame->setSelected(true);
}

void DashView::mousePressEvent(QMouseEvent* ev)
{
	if ((ev->buttons() & Qt::LeftButton) == Qt::LeftButton)
	{
		for (auto sframe : selected_)
			sframe->setSelected(false);

		selected_.clear();
	}
	else if ((ev->buttons() & Qt::RightButton) == Qt::RightButton)
	{
		for (auto wid : children())
		{
			XeroItemFrame* sframe = dynamic_cast<XeroItemFrame*>(wid);
			if (sframe != nullptr)
			{
				sframe->setSelected(false);
				sframe->setGeometry(0, 0, sframe->width(), sframe->height());
			}
		}
	}
}


void DashView::alignLeft()
{
	XeroItemFrame* first = nullptr;

	for (int i = 0; i < children().count(); i++)
	{
		XeroItemFrame* f = dynamic_cast<XeroItemFrame*>(children().at(i));
		if (f != nullptr)
		{
			if (first == nullptr)
			{
				first = f;
			}
			else
			{
				f->setGeometry(first->pos().x(), f->pos().y(), f->width(), f->height());
			}
		}
	}
}

void DashView::alignRight()
{

}

void DashView::alignTop()
{

}

void DashView::alignHCenter()
{

}

void DashView::alignVCentor()
{

}

void DashView::alignBottom()
{

}

void DashView::sizeWidth()
{

}

void DashView::sizeHeight()
{

}

void DashView::sizeBoth()
{

}