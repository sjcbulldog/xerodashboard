#pragma once

#include <QtCore/QJsonArray>
#include <QtWidgets/QWidget>

class NetworkTableManager;
class PlotMgr;

class DashView : public QWidget
{
	Q_OBJECT

public:
	DashView(std::shared_ptr<PlotMgr> plotmgr, std::shared_ptr<NetworkTableManager> ntmgr, QWidget *parent = Q_NULLPTR);
	~DashView();

	QJsonArray getJSONDesc();
	void restoreLayout(const QJsonArray &arr);

protected:
	void paintEvent(QPaintEvent* ev);
	void dragEnterEvent(QDragEnterEvent* ev);
	void dropEvent(QDropEvent* ev);

private:
	void createNTWidget(const QJsonObject& obj);
	void createPlot(const QJsonObject& obj);

private:
	std::shared_ptr<NetworkTableManager> ntmgr_;
	std::shared_ptr<PlotMgr> plotmgr_;
	QList<QWidget*> children_;
};
