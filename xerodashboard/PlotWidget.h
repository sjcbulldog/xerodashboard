#pragma once

#include <QtCore/QJsonObject>
#include <QtWidgets/QWidget>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QStackedWidget>
#include "Plot.h"

class NetworkTableManager;
class PlotMgr;

class PlotWidget : public QWidget
{
	Q_OBJECT

public:
	PlotWidget(std::shared_ptr<PlotMgr> plotmgr, std::shared_ptr<NetworkTableManager> ntmgr, const QString &plotname, QWidget *parent = Q_NULLPTR);
	~PlotWidget();

	QJsonObject getJSONDesc() const ;
	bool restoreFromJson(const QJsonObject& obj);

private:
	void createWindows();
	void initPlot();

	void plotStateChanged(Plot::State oldst, Plot::State newst);
	void busy(bool b);

	bool attachPlot();

	void plotAddedDetected(const QString &plotname);

private:
	QMetaObject::Connection plot_added_connection_;
	std::shared_ptr<PlotMgr> plotmgr_;
	std::shared_ptr<NetworkTableManager> ntmgr_;
	std::shared_ptr<Plot> plot_;
	QString plotname_;

	QMetaObject::Connection disconnect_connection_;

	QStackedWidget* stack_;
	QSplitter * splitter_;
	QTreeWidget* nodes_;
	QTabWidget* tabs_;
	QLabel* busy_;

	QList<int> splitter_sizes_;
};
