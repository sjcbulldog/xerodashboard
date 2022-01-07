#pragma once

#include <QtCore/QJsonObject>
#include <QtWidgets/QWidget>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QStackedWidget>
#include "Plot.h"
#include "EditableTabWidget.h"
#include "TabEditName.h"

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

	void addTab();
	void closeTab(int index = -1);

protected:
	void paintEvent(QPaintEvent* ev) override ;

private:
	void createWindows();
	void initPlot();

	void plotStateChanged(Plot::State oldst, Plot::State newst);
	void busy(bool b);

	bool attachPlot();

	void plotAddedDetected(const QString &plotname);

	void editTabText(int which);
	void editTabDone();
	void editTabAborted();

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
	EditableTabWidget* tabs_;
	QLabel* busy_;

	QList<int> splitter_sizes_;

	int which_tab_;
	TabEditName* editor_;
};
