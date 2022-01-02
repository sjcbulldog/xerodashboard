#include "XeroItemFrame.h"
#include <QtGui/QPainter>
#include <QtGui/QBrush>
#include <QtGui/QFontMetrics>
#include <QtGui/QMouseEvent>

XeroItemFrame::XeroItemFrame(QWidget *parent) : QWidget(parent)
{
	setMinimumSize(80, 80);
	child_ = nullptr;

	QFontMetrics fm(font());
	header_height_ = fm.height() + BorderThickness * 2 ;
}

XeroItemFrame::~XeroItemFrame()
{
}

QRect XeroItemFrame::closeBoxRect()
{
	QRect r(width() - BorderThickness - header_height_, BorderThickness, header_height_, header_height_);
	return r;
}

void XeroItemFrame::changeEvent(QEvent* ev)
{
	QWidget::changeEvent(ev);

	if (ev->type() == QEvent::FontChange)
	{
		QFontMetrics fm(font());
		header_height_ = fm.height() + BorderThickness * 2;
		child_->setGeometry(BorderThickness, header_height_, width() - 2 * BorderThickness, height() - header_height_ - BorderThickness);
	}
}

void XeroItemFrame::resizeEvent(QResizeEvent* ev)
{
	if (child_ != nullptr)
	{
		child_->setGeometry(BorderThickness, header_height_, width() - 2 * BorderThickness, height() - header_height_ - BorderThickness);
	}
}

void XeroItemFrame::mousePressEvent(QMouseEvent* ev)
{
	QRect r = closeBoxRect();
	if (r.contains(ev->pos()))
	{
		child_->close() ;
		close();
	}
}

void XeroItemFrame::paintEvent(QPaintEvent* ev)
{
	QPainter p(this);
	QFontMetricsF fm(p.font());

	p.setPen(QPen(QColor(0, 0, 0)));
	p.drawRect(0, 0, width(), height());

	p.setBrush(QBrush(QColor(135, 206, 250)));
	p.drawRect(0, 0, width(), header_height_ + BorderThickness * 2);
	
	QPoint pt(width() / 2 - fm.horizontalAdvance(title_) / 2, BorderThickness + fm.ascent());
	p.drawText(pt, title_);

	p.setBrush(Qt::BrushStyle::NoBrush);
	QRect r = closeBoxRect();
	p.drawRect(r);
	p.drawLine(r.topLeft(), r.bottomRight());
	p.drawLine(r.topRight(), r.bottomLeft());
}