#include "DashView.h"
#include <QtGui/QDragEnterEvent>
#include <QtGui/QPainter>
#include <QtCore/QMimeData>
#include <QtCore/QDebug>
#include "XeroItemFrame.h"
#include "NTValueDisplayWidget.h"
#include "PlotWidget.h"

DashView::DashView(std::shared_ptr<PlotMgr> plotmgr, std::shared_ptr<NetworkTableManager> ntmgr, QWidget *parent) : QWidget(parent)
{
	plotmgr_ = plotmgr;
	ntmgr_ = ntmgr;
	setAcceptDrops(true);
}

DashView::~DashView()
{
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
		NTValueDisplayWidget* vwid = new NTValueDisplayWidget(ntmgr_, value.mid(3), frame);
		frame->setWidget(vwid);
		frame->setVisible(true);
		frame->setTitle(title);

		frame->setGeometry(ev->pos().x(), ev->pos().y(), frame->width(), frame->height());
	}
	else if (value.startsWith("PLOT:"))
	{
		XeroItemFrame* frame = new XeroItemFrame(this);
		PlotWidget* vwid = new PlotWidget(plotmgr_, ntmgr_, value.mid(5), frame);
		frame->setWidget(vwid);
		frame->setVisible(true);
		frame->setTitle(value.mid(5));

		frame->setGeometry(ev->pos().x(), ev->pos().y(), frame->width(), frame->height());
	}
}