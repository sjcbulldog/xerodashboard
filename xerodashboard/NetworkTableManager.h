#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <networktables/NetworkTableInstance.h>

class NetworkTableManager : public QObject
{
	Q_OBJECT

public:
	NetworkTableManager(const QString& ipaddr);
	virtual ~NetworkTableManager();

	void processNetworkTableEvents();

	nt::NetworkTableEntry getEntry(const QString& name);

signals:
	void newEntry(const QString &entry);
	void updatedEntry(const QString &entry);
	void deletedEntry(const QString& entry);
	void disconnected();
	void connected();

private:
	void listenNotify(const nt::EntryNotification& ev);

private:
	bool connected_;
	nt::NetworkTableInstance inst_;

	QStringList newEntries_;
	QStringList updatedEntries_;
	QStringList deletedEntries_;

	NT_EntryListener listen_;
};
