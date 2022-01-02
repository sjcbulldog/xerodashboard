#include "XeroItemFrame.h"
#include <QtGui/QPainter>
#include <QtGui/QBrush>
#include <QtGui/QFontMetrics>
#include <QtGui/QMouseEvent>

XeroItemFrame::XeroItemFrame(QWidget *parent) : QWidget(parent)
{
	setMinimumSize(10, 10);
	child_ = nullptr;

	QFontMetrics fm(font());
	header_height_ = fm.height() + BorderThickness * 2 ;
}

XeroItemFrame::~XeroItemFrame()
{
}

void XeroItemFrame::layout()
{
	if (child_ == nullptr)
		return;

	int w = width();
	int h = height();

	if (w < child_->minimumWidth() + BorderThickness * 2)
		w = child_->minimumWidth() + BorderThickness * 2;

	if (h < child_->minimumHeight() + BorderThickness * 4 + header_height_)
		h = child_->minimumHeight() + BorderThickness * 4 + header_height_;

	if (w != width() || h != height())
	{
		setGeometry(pos().x(), pos().y(), w, h);

		//
		// This will trigger a resize event which will recursively call back into this method but with a valid width and
		// height so that the else clause below will execute
		//
	}
	else
	{
		child_->setGeometry(BorderThickness, header_height_ + 3 * BorderThickness, width() - BorderThickness * 2, height() - header_height_ - 4 * BorderThickness);
	}
}

QRect XeroItemFrame::closeBoxRect()
{
	QRect r(width() - BorderThickness - header_height_, BorderThickness, header_height_, header_height_);
	return r;
}

QRect XeroItemFrame::headerRect()
{
	QRect r(0, 0, width(), header_height_ + 2 * BorderThickness);
	return r;
}

void XeroItemFrame::changeEvent(QEvent* ev)
{
	QWidget::changeEvent(ev);

	if (ev->type() == QEvent::FontChange)
	{
		QFontMetrics fm(font());
		header_height_ = fm.height() + BorderThickness * 2;
		layout();
	}
}

void XeroItemFrame::resizeEvent(QResizeEvent* ev)
{
	if (child_ != nullptr)
	{
		layout();
	}
}

void XeroItemFrame::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton)
	{
		QRect r = closeBoxRect();
		if (r.contains(ev->pos()))
		{
			child_->close();
			close();
		}
		else {
			QRect r = headerRect();
			if (r.contains(ev->pos()))
			{
				dragging_ = true;
				mouse_ = ev->globalPos();
				window_ = frameGeometry().topLeft();
			}
		}
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

void XeroItemFrame::mouseMoveEvent(QMouseEvent* ev)
{
	if (dragging_)
	{
		QPoint dist = ev->globalPos() - mouse_;

		QPoint dest = window_ + dist;
		if (dest.x() < 0)
			dest.setX(0);
		else if (dest.x() > parentWidget()->width() - width())
			dest.setX(parentWidget()->width() - width());

		if (dest.y() < 0)
			dest.setY(0);
		else if (dest.y() > parentWidget()->height() - height())
			dest.setY(parentWidget()->height() - height());

		move(dest);
	}
}

void XeroItemFrame::mouseReleaseEvent(QMouseEvent* ev)
{
	dragging_ = false;
}