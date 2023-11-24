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
	NTValueDisplayWidget(std::shared_ptr<NetworkTableManager> ntmgr, const QString &path, NT_Topic topic, QWidget * parent = Q_NULLPTR);
	~NTValueDisplayWidget();

	QJsonObject getJSONDesc() const;

	void setDisplayType(const QString& type) {
		display_type_ = type;
		update();
	}

	void setUnits(const std::string& units);

protected:
	void paintEvent(QPaintEvent* ev) override;
	bool event(QEvent* ev) override;

private:
	void newDetected(const nt::TopicInfo &info);
	void entryUpdateDetected(const nt::ValueEventData& value);
	void disconnectDetected();
	void connectDetected();

	void drawContentsBoolean(QPainter& p);
	void drawContentsText(QPainter& p);
	void drawContentsBar(QPainter& p);

	void customMenuRequested(const QPoint& pos);

	void displayAs(QString value);

private:
	QString display_type_;
	nt::NetworkTableEntry entry_;
	std::shared_ptr<NetworkTableManager> ntmgr_;
	QString path_;
	NT_Topic topic_;
	bool connected_;
	QMetaObject::Connection update_connection_;
	QMetaObject::Connection new_connection_;
	QMetaObject::Connection disconnect_connection_;
	QMetaObject::Connection connect_connection_;
};
