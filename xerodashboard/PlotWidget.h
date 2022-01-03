#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>
#include "Plot.h"

class NetworkTableManager;
class PlotMgr;

class PlotWidget : public QWidget
{
	Q_OBJECT

public:
	PlotWidget(std::shared_ptr<PlotMgr> plotmgr, std::shared_ptr<NetworkTableManager> ntmgr, std::shared_ptr<Plot> plot, QWidget *parent = Q_NULLPTR);
	~PlotWidget();

private:
	void createWindows();
	void initPlot();

	void plotStateChanged(Plot::State oldst, Plot::State newst);
	void busy(bool b);

private:
	std::shared_ptr<PlotMgr> plotmgr_;
	std::shared_ptr<NetworkTableManager> ntmgr_;
	std::shared_ptr<Plot> plot_;

	QMetaObject::Connection disconnect_connection_;

	QHBoxLayout* layout_;
	QSplitter * splitter_;
	QTreeWidget* nodes_;
	QTabWidget* tabs_;
	QLabel* busy_;
};
