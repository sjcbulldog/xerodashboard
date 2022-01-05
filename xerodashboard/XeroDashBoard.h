#pragma once

#include <QtCore/QTimer>
#include <QtCore/QSettings>
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

protected:
    void closeEvent(QCloseEvent* ev);

private:
    static constexpr const char* GeometrySettings = "geometry";
    static constexpr const char* WindowStateSettings = "windowstate";
    static constexpr const char* LayoutSetting = "layout";

private:
    void createWindows();
    void createMenus();
    void createStatus();

    void showEditMenu();
    void editPreferences();

private:
    void timerCallback();

private:
    QSettings settings_;

    std::shared_ptr<NetworkTableManager> ntmgr_;
    QTimer timer_;

    std::shared_ptr<PlotMgr> plotmgr_;

    QMenu* edit_menu_;

    DashView* dash_view_;

    QDockWidget* network_table_dock_;
    NetworkTableTreeWidget* network_table_view_;

    QDockWidget* plot_list_dock_;
    PlotListWidget* plot_list_view_;

    QString key_;
    QString ipaddr_;
};
