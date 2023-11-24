#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMutex>
#include <networktables/NetworkTableInstance.h>
#include <QtCore/QMap>

class NetworkTableManager : public QObject
{
	Q_OBJECT

public:
	NetworkTableManager(const QString& ipaddr);
	virtual ~NetworkTableManager();

	void processNetworkTableEvents();

	nt::NetworkTableEntry getEntry(const QString& name);
	QStringList getSubKeys(const QString& name);

signals:
	void newEntry(const nt::TopicInfo &info);
	void updatedEntry(const nt::ValueEventData &data);
	void deletedEntry(const nt::TopicInfo& info);
	void disconnected();
	void connected();

private:
	void listenNotify(const nt::Event& ev);

private:
	bool connected_;
	nt::NetworkTableInstance inst_;

	QList<nt::TopicInfo> newEntries_;
	QList<nt::ValueEventData> updatedEntries_;
	QList<nt::TopicInfo> deletedEntries_;

	NT_Listener listen_;

	QMutex lock_;
};
