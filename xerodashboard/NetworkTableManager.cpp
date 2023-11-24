#include "NetworkTableManager.h"
#include <QtCore/QDebug>
#include <variant>
#include <algorithm>

NetworkTableManager::NetworkTableManager(const QString &ipaddr)
{
	inst_ = nt::NetworkTableInstance::GetDefault();

	auto fn = std::bind(&NetworkTableManager::listenNotify, this, std::placeholders::_1);
	int flags = 0xffffffff;

	const std::string_view root[] = { "/" };

	listen_ = inst_.AddListener(std::span<const std::string_view>{ root }, flags, fn);
	inst_.SetServer(ipaddr.toStdString().c_str());
	inst_.StartClient3("xerodashboard");

	connected_ = inst_.IsConnected();
	if (connected_)
		emit connected();
}

NetworkTableManager::~NetworkTableManager()
{
	inst_.RemoveListener(listen_);
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

void NetworkTableManager::listenNotify(const nt::Event& ev)
{
	QMutexLocker guard(&lock_);

	if ((ev.flags & NT_EVENT_PUBLISH) == NT_EVENT_PUBLISH)
	{
		if (std::holds_alternative<nt::TopicInfo>(ev.data))
		{
			nt::TopicInfo info = std::get<nt::TopicInfo>(ev.data);
			QString name = QString::fromStdString(info.name);

			deletedEntries_.removeIf([&name](const nt::TopicInfo &i) -> bool {
				return QString::fromStdString(i.name) == name;
			});

			auto it = std::find_if(newEntries_.begin(), newEntries_.end(), [&name](const nt::TopicInfo& info) {
				return QString::fromStdString(info.name) == name;
			});

			if (it == newEntries_.end()) 
			{
				newEntries_.push_back(info);
			}
		}
	}
	else if ((ev.flags & NT_EVENT_UNPUBLISH) == NT_EVENT_UNPUBLISH)
	{
		if (std::holds_alternative<nt::TopicInfo>(ev.data))
		{
			nt::TopicInfo info = std::get<nt::TopicInfo>(ev.data);
			QString name = QString::fromStdString(info.name);

			newEntries_.removeIf([&name](const nt::TopicInfo &i) -> bool {
				return QString::fromStdString(i.name) == name;
			});

			auto it = std::find_if(deletedEntries_.begin(), deletedEntries_.end(), [&name](const nt::TopicInfo& info) {
				return QString::fromStdString(info.name) == name;
				});

			if (it == deletedEntries_.end())
			{
				deletedEntries_.push_back(info);
			}
		}
	}
	else if ((ev.flags & NT_EVENT_PROPERTIES) == NT_EVENT_PROPERTIES)
	{
		//
		// Nothing for now
		//
	}
	else if ((ev.flags & NT_EVENT_VALUE_REMOTE) == NT_EVENT_VALUE_REMOTE)
	{
		if (std::holds_alternative<nt::ValueEventData>(ev.data))
		{
			nt::ValueEventData data = std::get<nt::ValueEventData>(ev.data);
			updatedEntries_.push_back(data);
		}
	}
	else if ((ev.flags & NT_EVENT_VALUE_LOCAL) == NT_EVENT_VALUE_LOCAL)
	{
		if (std::holds_alternative<nt::ValueEventData>(ev.data))
		{
			nt::ValueEventData data = std::get<nt::ValueEventData>(ev.data);
			updatedEntries_.push_back(data);
		}
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
			auto entry = deletedEntries_.first();
			deletedEntries_.removeFirst();
			emit deletedEntry(entry);
		}

		while (newEntries_.size() > 0)
		{
			auto entry = newEntries_.first();
			newEntries_.removeFirst();
			emit newEntry(entry);
		}

		while (updatedEntries_.size() > 0)
		{
			auto entry = updatedEntries_.first();
			updatedEntries_.removeFirst();
			emit updatedEntry(entry);
		}
	}
}