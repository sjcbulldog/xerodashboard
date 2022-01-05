#pragma once

#include <QtCore/QJsonOBject>
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

	QJsonObject getJSONDesc() const;

	void setDisplayType(const QString& type) {
		display_type_ = type;
		update();
	}

protected:
	void paintEvent(QPaintEvent* ev);

private:
	void newDetected(const QString& path);
	void entryUpdateDetected(const QString& path);
	void disconnectDetected();
	void connectDetected();

	void drawContentsBoolean(QPainter& p);
	void drawContentsText(QPainter& p);

private:
	QString display_type_;
	nt::NetworkTableEntry entry_;
	std::shared_ptr<NetworkTableManager> ntmgr_;
	QString path_;
	bool connected_;
	QMetaObject::Connection update_connection_;
	QMetaObject::Connection disconnect_connection_;
	QMetaObject::Connection connect_connection_;
};
