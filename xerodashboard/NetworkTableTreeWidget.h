#pragma once

#include <networktables/NetworkTableEntry.h>
#include <memory>
#include "XeroTreeWidgetBase.h"

class NetworkTableManager;

class NetworkTableTreeWidget : public XeroTreeWidgetBase
{
	Q_OBJECT

public:
	NetworkTableTreeWidget(std::shared_ptr<NetworkTableManager> ntmgr, QWidget *parent = Q_NULLPTR);
	~NetworkTableTreeWidget();

protected:
	QMimeData* mimeData(const QList<QTreeWidgetItem*> items) const;

private:
	void newEntryDetected(const QString &name);
	void updatedEntryDetected(const QString &name);
	void deletedEntryDetected(const QString &name);
	void connectDetected();
	void disconnectDetected();

private:
	std::shared_ptr<NetworkTableManager> ntmgr_;
};
