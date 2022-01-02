#pragma once

#include <QtWidgets/QWidget>

class NetworkTableManager;

class DashView : public QWidget
{
	Q_OBJECT

public:
	DashView(std::shared_ptr<NetworkTableManager> ntmgr, QWidget *parent = Q_NULLPTR);
	~DashView();

protected:
	void dragEnterEvent(QDragEnterEvent* ev);
	void dropEvent(QDropEvent* ev);

private:
	std::shared_ptr<NetworkTableManager> ntmgr_;
};
