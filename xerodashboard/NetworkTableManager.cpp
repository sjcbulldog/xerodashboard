#include "NetworkTableManager.h"
#include <QtCore/QDebug>

NetworkTableManager::NetworkTableManager(const QString &ipaddr)
{
	inst_ = nt::NetworkTableInstance::GetDefault();

	auto fn = std::bind(&NetworkTableManager::listenNotify, this, std::placeholders::_1);
	int flags = 0xffffffff;

	listen_ = inst_.AddEntryListener("/", fn, flags);
	inst_.StartClient(ipaddr.toStdString().c_str());

	connected_ = inst_.IsConnected();
	if (connected_)
		emit connected();
}

NetworkTableManager::~NetworkTableManager()
{
	inst_.RemoveEntryListener(listen_);
	inst_.StopClient();
}

nt::NetworkTableEntry NetworkTableManager::getEntry(const QString& name)
{
	return inst_.GetEntry(name.toStdString());
}

QStringList NetworkTableManager::getSubKeys(const QString& name)
{
	QStringList ret;
	QString tname = name;

	if (tname.endsWith("/"))
		tname = tname.mid(0, tname.length() - 1);

	auto table = inst_.GetTable(tname.toStdString());
	auto keys = table->GetSubTables();
	for (int i = 0; i < keys.size(); i++)
	{
		QString qname = QString::fromStdString(keys[i]);
		if (!ret.contains(qname))
			ret.push_back(qname);
	}

	return ret;
}

void NetworkTableManager::listenNotify(const nt::EntryNotification& ev)
{
	QString name = QString::fromStdString(ev.name);
	QMutexLocker guard(&lock_);

	if ((ev.flags & NT_NOTIFY_NEW) == NT_NOTIFY_NEW)
	{
		if (deletedEntries_.contains(name))
			deletedEntries_.removeOne(name);

		if (!newEntries_.contains(name))
			newEntries_.push_back(name);
	}
	else if ((ev.flags & NT_NOTIFY_UPDATE) == NT_NOTIFY_UPDATE)
	{
		if (!deletedEntries_.contains(name) && !updatedEntries_.contains(name))
			updatedEntries_.push_back(name);
	}
	else if ((ev.flags & NT_NOTIFY_DELETE) == NT_NOTIFY_DELETE)
	{
		if (newEntries_.contains(name))
			newEntries_.removeOne(name);

		if (updatedEntries_.contains(name))
			updatedEntries_.removeOne(name);

		deletedEntries_.push_back(name);
	}
}

void NetworkTableManager::processNetworkTableEvents()
{
	if (!inst_.IsConnected())
	{
		if (connected_)
		{
			connected_ = false;
			emit disconnected();
		}
	}
	else
	{
		if (!connected_)
		{
			connected_ = true;
			emit connected();
		}

		QMutexLocker guard(&lock_);
		while (deletedEntries_.size() > 0)
		{
			QString entry = deletedEntries_.first();
			deletedEntries_.removeFirst();
			emit deletedEntry(entry);
		}

		while (newEntries_.size() > 0)
		{
			QString entry = newEntries_.first();
			newEntries_.removeFirst();
			emit newEntry(entry);
		}

		while (updatedEntries_.size() > 0)
		{
			QString entry = updatedEntries_.first();
			updatedEntries_.removeFirst();
			emit updatedEntry(entry);
		}
	}
}