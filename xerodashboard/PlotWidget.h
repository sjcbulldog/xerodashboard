#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QBoxLayout>

class NetworkTableManager;
class PlotMgr;
class Plot;

class PlotWidget : public QWidget
{
	Q_OBJECT

public:
	PlotWidget(std::shared_ptr<PlotMgr> plotmgr, std::shared_ptr<NetworkTableManager> ntmgr, const QString &plotname, QWidget *parent = Q_NULLPTR);
	~PlotWidget();

private:
	void createWindows();
	void initPlot(std::shared_ptr<Plot> plot);

private:
	std::shared_ptr<PlotMgr> plotmgr_;
	std::shared_ptr<NetworkTableManager> ntmgr_;
	QString plotname_;

	QHBoxLayout* layout_;
	QSplitter * splitter_;
	QTreeWidget* nodes_;
	QTabWidget* tabs_;
};
