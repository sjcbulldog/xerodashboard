#pragma once

#include <QtCore/QJsonArray>
#include <QtWidgets/QWidget>

class NetworkTableManager;
class PlotMgr;
class XeroItemFrame;

class DashView : public QWidget
{
	Q_OBJECT

public:
	DashView(std::shared_ptr<PlotMgr> plotmgr, std::shared_ptr<NetworkTableManager> ntmgr, QWidget *parent = Q_NULLPTR);
	~DashView();

	QJsonArray getJSONDesc();
	void restoreLayout(const QJsonArray &arr);

protected:
	void paintEvent(QPaintEvent* ev) override;
	void dragEnterEvent(QDragEnterEvent* ev) override;
	void dropEvent(QDropEvent* ev) override;
	void mousePressEvent(QMouseEvent* ev) override;

private:
	void createNTWidget(const QJsonObject& obj);
	void createPlot(const QJsonObject& obj);
	void frameWindowHeaderClicked(XeroItemFrame*, bool shift);

private:
	std::shared_ptr<NetworkTableManager> ntmgr_;
	std::shared_ptr<PlotMgr> plotmgr_;
	QList<XeroItemFrame*> selected_;
};
