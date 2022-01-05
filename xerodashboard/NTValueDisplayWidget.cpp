#include "NTValueDisplayWidget.h"
#include "NetworkTableManager.h"
#include "NTFormattingUtils.h"
#include "JsonFieldNames.h"
#include <QtCore/QDebug>
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

	setMinimumSize(80, 20);

	display_type_ = "text";
}

NTValueDisplayWidget::~NTValueDisplayWidget()
{
	disconnect(update_connection_);
	disconnect(disconnect_connection_);
	disconnect(connect_connection_);
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

	auto value = entry_.GetValue();
	if (value != nullptr && value->IsValid())
	{
		QBrush back(QColor(255, 255, 255));
		p.setBrush(back);
		p.drawRect(0, 0, width(), height());

		if (value->IsBoolean())
		{
			drawContentsBoolean(p);
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
