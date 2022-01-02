#pragma once

#include <QtWidgets/QWidget>
#include <networktables/NetworkTableEntry.h>
#include <memory>

class NetworkTableManager;

class NTValueDisplayWidget : public QWidget
{
	Q_OBJECT

public:
	NTValueDisplayWidget(std::shared_ptr<NetworkTableManager> ntmgr, const QString &path, QWidget * parent = Q_NULLPTR);
	~NTValueDisplayWidget();

protected:
	void paintEvent(QPaintEvent* ev);

private:
	void entryUpdateDetected(const QString& path);
	void disconnectDetected();
	void connectDetected();

	void drawContentsBoolean(QPainter& p);
	void drawContentsText(QPainter& p);

private:
	nt::NetworkTableEntry entry_;
	std::shared_ptr<NetworkTableManager> ntmgr_;
	QString path_;
	bool connected_;
	QMetaObject::Connection update_connection_;
	QMetaObject::Connection disconnect_connection_;
	QMetaObject::Connection connect_connection_;
};
