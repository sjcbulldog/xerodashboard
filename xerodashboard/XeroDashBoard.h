#pragma once

#include <QtCore/QTimer>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QDockWidget>
#include <memory>

class NetworkTableManager;
class DashView;
class PlotListWidget;
class NetworkTableTreeWidget;
class PlotMgr;

class XeroDashBoard : public QMainWindow
{
    Q_OBJECT

public:
    XeroDashBoard(QWidget *parent = Q_NULLPTR);
    virtual ~XeroDashBoard();

private:
    void createWindows();
    void createMenus();
    void createStatus();

    void showEditMenu();
    void editPreferences();

private:
    void timerCallback();

private:
    std::shared_ptr<NetworkTableManager> table_mgr_;
    QTimer timer_;

    std::shared_ptr<PlotMgr> plot_mgr_;

    QMenu* edit_menu_;

    DashView* dash_view_;

    QDockWidget* network_table_dock_;
    NetworkTableTreeWidget* network_table_view_;

    QDockWidget* plot_list_dock_;
    PlotListWidget* plot_list_view_;
};
