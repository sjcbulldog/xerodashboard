#pragma once

#include <QtWidgets/QTreeWidget>
#include <networktables/NetworkTableInstance.h>
#include "XeroTreeWidgetBase.h"

class NetworkTableManager;
class PlotMgr;
class Plot;

class PlotListWidget : public XeroTreeWidgetBase
{
	Q_OBJECT

public:
	PlotListWidget(std::shared_ptr<NetworkTableManager> ntmgr, std::shared_ptr<PlotMgr> plotmgr, const QString &plotkey, QWidget *parent = Q_NULLPTR);
	~PlotListWidget();

protected:
	QMimeData* mimeData(const QList<QTreeWidgetItem*> items) const;

private:
	void newEntryDetected(const QString& name);
	void updatedEntryDetected(const QString& name);
	void deletedEntryDetected(const QString& name);

	bool processPlotKeyString(const QString& str, QString& plotname, QStringList& key);

	void updatePlotStatus(std::shared_ptr<Plot> plot);

	void readExistingPlots();

private:
	QString plot_key_;
	std::shared_ptr<NetworkTableManager> ntmgr_;
	std::shared_ptr<PlotMgr> plotmgr_;
};
