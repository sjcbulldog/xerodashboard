#pragma once

#include <QtCore/QObject>
#include <QtCore/QMap>

class Plot;

class PlotMgr : public QObject
{
	Q_OBJECT

public:
	PlotMgr(QObject *parent);
	~PlotMgr();

	std::shared_ptr<Plot> getAddPlot(const QString& name);
	std::shared_ptr<Plot> getPlot(const QString& name);

	void deletePlot(const QString& name) {
		plots_.remove(name);
	}

signals:

private:
	QMap<QString, std::shared_ptr<Plot>> plots_;
};
