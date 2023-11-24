#include "NetworkTableTreeWidget.h"
#include <QtCore/QDebug>
#include <QtCore/QMimeData>
#include <QtWidgets/QHeaderView>
#include "NetworkTableManager.h"
#include "NTFormattingUtils.h"

NetworkTableTreeWidget::NetworkTableTreeWidget(std::shared_ptr<NetworkTableManager> ntmgr, QWidget* parent) : XeroTreeWidgetBase(parent)
{
	ntmgr_ = ntmgr;

	static QStringList labels = { "Name", "Value" };
	setColumnCount(2);
	setHeaderLabels(labels);
	setSortingEnabled(true);

	setDragDropMode(DragDropMode::DragOnly);
	setDragEnabled(true);

	setSelectionMode(QAbstractItemView::SingleSelection);

	(void)connect(ntmgr_.get(), &NetworkTableManager::newEntry, this, &NetworkTableTreeWidget::newEntryDetected);
	(void)connect(ntmgr_.get(), &NetworkTableManager::updatedEntry, this, &NetworkTableTreeWidget::updatedEntryDetected);
	(void)connect(ntmgr_.get(), &NetworkTableManager::deletedEntry, this, &NetworkTableTreeWidget::deletedEntryDetected);
	(void)connect(ntmgr_.get(), &NetworkTableManager::connected, this, &NetworkTableTreeWidget::connectDetected);
	(void)connect(ntmgr_.get(), &NetworkTableManager::disconnected, this, &NetworkTableTreeWidget::disconnectDetected);
}

NetworkTableTreeWidget::~NetworkTableTreeWidget()
{
}

QMimeData* NetworkTableTreeWidget::mimeData(const QList<QTreeWidgetItem*> items) const
{
	QMimeData* data = nullptr;

	if (items.size() == 1)
	{
		QString path;
		QTreeWidgetItem* item = items.at(0);

		while (item)
		{
			if (path.length() > 0)
				path = item->text(0) + "/" + path;
			else
				path = item->text(0);

			item = item->parent();
		}

		data = new QMimeData();
		int topic = item->data(0, Qt::UserRole).toInt();
		data->setText("NT:/" + path + "$$$" + QString::number(topic));
	}

	return data;
}

void NetworkTableTreeWidget::connectDetected()
{

}

void NetworkTableTreeWidget::disconnectDetected()
{
	while (topLevelItemCount() > 0)
	{
		auto item = takeTopLevelItem(0);
		delete item;
	}
}

void NetworkTableTreeWidget::newEntryDetected(const nt::TopicInfo &info)
{
	QString name = QString::fromStdString(info.name);
	QTreeWidgetItem* item = createItem(name);
	item->setData(0, Qt::UserRole, info.topic);
	sortItems(0, Qt::SortOrder::AscendingOrder);

	nt::NetworkTableEntry entry = ntmgr_->getEntry(name);
	QString text = NTFormattingUtils::toString(entry);
	item->setText(1, text);
}

void NetworkTableTreeWidget::updatedEntryDetected(const nt::ValueEventData &value)
{
	if (name_from_handle_.contains(value.topic)) {
		QString name = name_from_handle_[value.topic];
		QTreeWidgetItem* item = createItem(name);
		sortItems(0, Qt::SortOrder::AscendingOrder);

		nt::NetworkTableEntry entry = ntmgr_->getEntry(name);
		QString text = NTFormattingUtils::toString(entry);
		item->setText(1, text);
	}
}

void NetworkTableTreeWidget::deletedEntryDetected(const nt::TopicInfo& info)
{
	name_from_handle_.remove(info.topic);
}
