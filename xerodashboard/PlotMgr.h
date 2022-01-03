#pragma once

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <memory>

class Plot;
class NetworkTableManager;

class PlotMgr : public QObject
{
	Q_OBJECT

public:
	PlotMgr(std::shared_ptr<NetworkTableManager> ntmgr, const QString &key, QObject *parent);
	~PlotMgr();

	std::shared_ptr<Plot> getAddPlot(const QString& name);
	std::shared_ptr<Plot> getPlot(const QString& name);

	void deletePlot(const QString& name) {
		plots_.remove(name);
	}

signals:

private:
	QString key_;
	QMap<QString, std::shared_ptr<Plot>> plots_;
	std::shared_ptr<NetworkTableManager> ntmgr_;
};
