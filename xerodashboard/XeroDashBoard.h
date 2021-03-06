#pragma once

#include "GameFieldManager.h"
#include <QtCore/QTimer>
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QLabel>
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
    
    static constexpr const char* IPAddrSettings = "ipaddr";
    static constexpr const char* PlotKeySettings = "plotkey";
    static constexpr const char* TileMarginSettings = "tilemargin";

private:
    void createWindows();
    void createMenus();
    void createStatus();

    void showEditMenu();
    void showFileMenu();
    void showAlignMenu();
    void showSizeMenu();
    void showWindowMenu();

    void fileSave();
    void fileSaveAs();
    void fileLoad();
    void fileExit();

    void editPreferences();
 
private:
    void timerCallback();
    void saveJson();
    void selectedSetCountChanged();


private:
    QSettings settings_;

    std::shared_ptr<NetworkTableManager> ntmgr_;
    QTimer timer_;

    std::shared_ptr<PlotMgr> plotmgr_;

    QMenu* file_menu_;
    QAction* file_save_action_;
    QAction* file_save_as_action_;
    QAction* file_load_action_;

    QMenu* edit_menu_;
    QAction* edit_preferences_action_;

    QMenu* align_menu_;
    QAction* align_left_action_;
    QAction* align_right_action_;
    QAction* align_top_action_;
    QAction* align_bottom_action_;
    QAction* align_hcenter_action_;
    QAction* align_vcenter_action_;
    QAction* align_tile_vertical_action_;
    QAction* align_tile_horizontal_action_;

    QMenu* size_menu_;
    QAction* size_same_width_action_;
    QAction* size_same_height_action_;
    QAction* size_same_both_action_;

    QMenu* window_menu_;
    QAction* window_new_tab_;
    QAction* window_close_tab_;
    QAction* window_show_path_;

    QLabel* status_selected_;

    DashView* dash_view_;

    QDockWidget* network_table_dock_;
    NetworkTableTreeWidget* network_table_view_;

    QDockWidget* plot_list_dock_;
    PlotListWidget* plot_list_view_;

    QString key_;
    QString ipaddr_;

    QString filename_;
    QDir homedir_;

    GameFieldManager field_mgr_;
    bool game_mgr_;
};
