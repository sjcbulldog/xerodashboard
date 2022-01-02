#include "NTValueDisplayWidget.h"
#include "NetworkTableManager.h"
#include "NTFormattingUtils.h"
#include <QtGui/QPainter>

NTValueDisplayWidget::NTValueDisplayWidget(std::shared_ptr<NetworkTableManager> ntmgr, const QString& path, QWidget *parent): QWidget(parent)
{
	ntmgr_ = ntmgr;
	path_ = path;
	entry_ = ntmgr->getEntry(path_);
	connected_ = true;

	update_connection_ = connect(ntmgr.get(), &NetworkTableManager::updatedEntry, this, &NTValueDisplayWidget::entryUpdateDetected);
	disconnect_connection_ = connect(ntmgr.get(), &NetworkTableManager::disconnected, this, &NTValueDisplayWidget::disconnectDetected);
	connect_connection_ = connect(ntmgr.get(), &NetworkTableManager::connected, this, &NTValueDisplayWidget::connectDetected);
}

NTValueDisplayWidget::~NTValueDisplayWidget()
{
	disconnect(update_connection_);
	disconnect(disconnect_connection_);
	disconnect(connect_connection_);
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

void NTValueDisplayWidget::paintEvent(QPaintEvent* ev)
{
	auto value = entry_.GetValue();
	if (value != nullptr && value->IsValid())
	{
		QPainter p(this);

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
		//
		// What to draw if the value is not valid, or does not exist
		//
	}
}

void NTValueDisplayWidget::drawContentsBoolean(QPainter& p)
{
	auto value = entry_.GetValue();
	assert(value->IsValid());
	assert(value->IsBoolean());

	QColor color = value->GetBoolean() ? QColor(0, 255, 0) : QColor(255, 0, 0);
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
