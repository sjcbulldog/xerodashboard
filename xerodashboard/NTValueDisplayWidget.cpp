#include "NTValueDisplayWidget.h"
#include "NetworkTableManager.h"
#include "NTFormattingUtils.h"
#include "JsonFieldNames.h"
#include <QtCore/QDebug>
#include <QtCore/QEvent>
#include <QtWidgets/QToolTip>
#include <QtWidgets/QMenu>
#include <QtGui/QHelpEvent>
#include <QtGui/QPainter>

NTValueDisplayWidget::NTValueDisplayWidget(std::shared_ptr<NetworkTableManager> ntmgr, const QString& path, QWidget *parent): QWidget(parent)
{
	ntmgr_ = ntmgr;
	path_ = path;
	entry_ = ntmgr->getEntry(path_);
	connected_ = true;

	update_connection_ = connect(ntmgr.get(), &NetworkTableManager::updatedEntry, this, &NTValueDisplayWidget::entryUpdateDetected);
	update_connection_ = connect(ntmgr.get(), &NetworkTableManager::newEntry, this, &NTValueDisplayWidget::newDetected);
	disconnect_connection_ = connect(ntmgr.get(), &NetworkTableManager::disconnected, this, &NTValueDisplayWidget::disconnectDetected);
	connect_connection_ = connect(ntmgr.get(), &NetworkTableManager::connected, this, &NTValueDisplayWidget::connectDetected);

	(void)connect(this, &QWidget::customContextMenuRequested, this, &NTValueDisplayWidget::customMenuRequested);

	setMinimumSize(40, 10);

	setContextMenuPolicy(Qt::CustomContextMenu);

	display_type_ = "text";
}

NTValueDisplayWidget::~NTValueDisplayWidget()
{
	disconnect(update_connection_);
	disconnect(disconnect_connection_);
	disconnect(connect_connection_);
}

void NTValueDisplayWidget::displayAs(QString v)
{
	display_type_ = v;
}

void NTValueDisplayWidget::customMenuRequested(const QPoint& pos)
{
	QMenu menu;

	QAction textDisplay("Display As Text", this);
	auto fn = std::bind(&NTValueDisplayWidget::displayAs, this, QString("text"));
	connect(&textDisplay, &QAction::triggered, fn);
	menu.addAction(&textDisplay);

	QAction colorDisplay("Display As Color", this);
	fn = std::bind(&NTValueDisplayWidget::displayAs, this, QString("color"));
	connect(&colorDisplay, &QAction::triggered, fn);
	menu.addAction(&colorDisplay);

	QAction barDisplay("Display As Bar", this);
	fn = std::bind(&NTValueDisplayWidget::displayAs, this, QString("bar"));
	connect(&barDisplay, &QAction::triggered, fn);
	menu.addAction(&barDisplay);

	menu.exec(mapToGlobal(pos));
	update();
}

bool NTValueDisplayWidget::event(QEvent* ev)
{
	if (ev->type() == QEvent::ToolTip)
	{
		QHelpEvent* hev = static_cast<QHelpEvent*>(ev);
		QToolTip::showText(hev->globalPos(), path_);
	}
	return QWidget::event(ev);
}

QJsonObject NTValueDisplayWidget::getJSONDesc() const
{
	QJsonObject obj;

	obj[JsonFieldNames::Type] = JsonFieldNames::TypeValueNTV;
	obj[JsonFieldNames::Display] = display_type_;
	obj[JsonFieldNames::Path] = path_;
	obj[JsonFieldNames::X] = pos().x();
	obj[JsonFieldNames::Y] = pos().y();
	obj[JsonFieldNames::Width] = width();
	obj[JsonFieldNames::Height] = height();

	return obj;
}

void NTValueDisplayWidget::disconnectDetected()
{
	if (connected_ != false)
	{
		connected_ = false;
		update();
	}
}

void NTValueDisplayWidget::connectDetected()
{
	if (connected_ != true)
	{
		connected_ = true;
		entry_ = ntmgr_->getEntry(path_);
		update();
	}
}

void NTValueDisplayWidget::entryUpdateDetected(const QString& name)
{
	if (name == path_)
	{
		entry_ = ntmgr_->getEntry(path_);
		update();
	}
}

void NTValueDisplayWidget::newDetected(const QString& name)
{
	if (name == path_)
	{
		entry_ = ntmgr_->getEntry(path_);
		update();
	}
}

void NTValueDisplayWidget::paintEvent(QPaintEvent* ev)
{
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);

	auto value = entry_.GetValue();
	if (value != nullptr && value->IsValid())
	{
		QBrush back(QColor(255, 255, 255));
		p.setBrush(back);
		p.drawRect(0, 0, width(), height());

		if (value->IsBoolean())
		{
			if (display_type_ == "color")
				drawContentsBoolean(p);
			else
				drawContentsText(p);

		}
		else if (value->IsDouble())
		{
			if (display_type_ == "bar")
				drawContentsBar(p);
			else
				drawContentsText(p);
		}
		else
		{
			drawContentsText(p);
		}
	}
	else
	{
		QPen pen(QColor(255, 0, 0));
		p.setPen(pen);
		p.drawLine(0, 0, width() - 1, height() - 1);
		p.drawLine(0, height() - 1, width() - 1, 0);
	}
}

void NTValueDisplayWidget::drawContentsBoolean(QPainter& p)
{
	auto value = entry_.GetValue();
	assert(value->IsValid());
	assert(value->IsBoolean());

	QColor color;
	if (!connected_)
	{
		color = QColor(128, 128, 128);
	}
	else if (value->GetBoolean())
	{
		color = QColor(0, 255, 0);
	}
	else
	{
		color = QColor(255, 0, 0);
	}

	QBrush brush(color);
	p.setBrush(brush);
	p.drawRect(0, 0, width(), height());
}

void NTValueDisplayWidget::drawContentsBar(QPainter& p)
{
	auto value = entry_.GetValue();
	assert(value->IsValid());
	assert(value->IsDouble());

	int w = width() - 8;
	int h = height() - 4;
	int xrad = 4;
	int yrad = 4;

	QPen pen(QColor(0, 0, 0));
	pen.setWidth(2);
	p.setPen(pen);

	if (!connected_)
	{
		p.setBrush(QBrush(QColor(128, 128, 128)));
		p.drawRoundedRect(4, 2, w, h, xrad, yrad, Qt::SizeMode::AbsoluteSize);
	}
	else
	{
		double d = value->GetDouble();
		QColor c;
		int left, right, bar;

		if (d > 1.0)
			d = 1.0;
		else if (d < -1.0)
			d = -1.0;

		if (d < 0)
		{
			c = QColor(0xF5, 0xE0, 0x50);
			left = width() / 2 * (1 + d);
			right = width() / 2;
			bar = left;
		}
		else
		{
			c = QColor(0, 255, 0);
			left = width() / 2;
			right = (width() / 2) * d + width() / 2;
			bar = right;
		}

		p.setBrush(QBrush(QColor(0, 0, 0)));
		p.drawRoundedRect(4, 2, w, h, xrad, yrad, Qt::SizeMode::AbsoluteSize);

		p.setClipRect(left, 0, right - left, height());
		p.setBrush(QBrush(c));
		p.drawRoundedRect(4, 2, w, h, xrad, yrad, Qt::SizeMode::AbsoluteSize);
	}
}

void NTValueDisplayWidget::drawContentsText(QPainter &p)
{
	QFontMetricsF fm(p.font());

	QColor txtcolor;

	if (connected_)
		txtcolor = QColor(0, 0, 0);
	else
		txtcolor = QColor(128, 128, 128);

	auto value = entry_.GetValue();
	if (value->IsValid())
	{
		QString txt = NTFormattingUtils::toString(entry_);
		QPoint pt(width() / 2 - fm.horizontalAdvance(txt) / 2, height() / 2 - fm.height() / 2 + fm.ascent());
		p.setPen(QPen(txtcolor));
		p.drawText(pt, txt);
	}
}
