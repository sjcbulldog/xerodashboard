#include "xerodashboard.h"
#include <QtCore/QJsonDocument>
#include <QtWidgets/QMenuBar>
#include "NetworkTableManager.h"
#include "NetworkTableTreeWidget.h"
#include "PlotListWidget.h"
#include "DashView.h"
#include "PlotMgr.h"

XeroDashBoard::XeroDashBoard(QWidget *parent) : QMainWindow(parent)
{
    key_ = "/XeroPlot/";
    ipaddr_ = "127.0.0.1";

    ntmgr_ = std::make_shared<NetworkTableManager>(ipaddr_);
    (void)connect(&timer_, &QTimer::timeout, this, &XeroDashBoard::timerCallback);
    timer_.start(std::chrono::milliseconds(100));

    plotmgr_ = std::make_shared<PlotMgr>(ntmgr_, key_, nullptr);

    setMinimumWidth(800);
    setMinimumHeight(600);

    createWindows();
    createMenus();
    createStatus();

    if (settings_.contains(GeometrySettings))
        restoreGeometry(settings_.value(GeometrySettings).toByteArray());

    if (settings_.contains(WindowStateSettings))
        restoreState(settings_.value(WindowStateSettings).toByteArray());

    if (settings_.contains(LayoutSetting))
    {
        QString layoutstr = settings_.value(LayoutSetting).toString();
        QByteArray bytes = layoutstr.toUtf8();
        QJsonDocument doc = QJsonDocument::fromJson(bytes);
        if (doc.isArray())
        {
            auto arr = doc.array();
            dash_view_->restoreLayout(arr);
        }
    }
}

XeroDashBoard::~XeroDashBoard()
{
}

void XeroDashBoard::closeEvent(QCloseEvent* ev)
{
    QJsonArray arr = dash_view_->getJSONDesc();
    QJsonDocument doc(arr);
    QString layout = doc.toJson(QJsonDocument::Compact);
    settings_.setValue(LayoutSetting, layout);

    settings_.setValue(GeometrySettings, saveGeometry());
    settings_.setValue(WindowStateSettings, saveState());
    QMainWindow::closeEvent(ev);
}

void XeroDashBoard::timerCallback()
{
    ntmgr_->processNetworkTableEvents();
}

void XeroDashBoard::createWindows()
{
    dash_view_ = new DashView(plotmgr_, ntmgr_);
    setCentralWidget(dash_view_);

    network_table_dock_ = new QDockWidget(tr("Network Table"), this);
    network_table_dock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    network_table_view_ = new NetworkTableTreeWidget(ntmgr_, network_table_dock_);
    network_table_dock_->setWidget(network_table_view_);
    addDockWidget(Qt::LeftDockWidgetArea, network_table_dock_);

    plot_list_dock_ = new QDockWidget(tr("Plot List"), this);
    plot_list_dock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    plot_list_view_ = new PlotListWidget(ntmgr_, plotmgr_, key_, plot_list_dock_);
    plot_list_dock_->setWidget(plot_list_view_);
    addDockWidget(Qt::LeftDockWidgetArea, plot_list_dock_);
}

void XeroDashBoard::createMenus()
{
    //QMenu* align_menu_;
    //QAction* align_left_action_;
    //QAction* align_right_action_;
    //QAction* align_top_action_;
    //QAction* align_bottom_action_;
    //QAction* align_hcenter_action_;
    //QAction* align_vcenter_action_;

    //QMenu* size_menu_;
    //QAction* size_same_width_action_;
    //QAction* size_same_height_action_;
    //QAction* size_same_both_action_;

    file_menu_ = new QMenu(tr("&File"));
    (void)connect(file_menu_, &QMenu::aboutToShow, this, &XeroDashBoard::showFileMenu);
    menuBar()->addMenu(file_menu_);

    file_save_action_ = file_menu_->addAction(tr("Save"));
    (void)connect(file_save_action_, &QAction::triggered, this, &XeroDashBoard::fileSave);

    file_menu_->addSeparator();

    file_save_as_action_ = file_menu_->addAction(tr("Save As"));
    (void)connect(file_save_as_action_, &QAction::triggered, this, &XeroDashBoard::fileSaveAs);

    file_menu_->addSeparator();

    file_load_action_ = file_menu_->addAction(tr("Load"));
    (void)connect(file_load_action_, &QAction::triggered, this, &XeroDashBoard::fileLoad);

    file_menu_->addSeparator();

    file_save_as_action_ = file_menu_->addAction(tr("Exit"));
    (void)connect(file_save_as_action_, &QAction::triggered, this, &XeroDashBoard::fileExit);

    edit_menu_ = new QMenu(tr("&Edit"));
    (void)connect(edit_menu_, &QMenu::aboutToShow, this, &XeroDashBoard::showEditMenu);
    menuBar()->addMenu(edit_menu_);

    edit_preferences_action_ = edit_menu_->addAction(tr("Preferences"));
    (void)connect(edit_preferences_action_, &QAction::triggered, this, &XeroDashBoard::editPreferences);

    align_menu_ = new QMenu(tr("&Align"));
    (void)connect(align_menu_, &QMenu::aboutToShow, this, &XeroDashBoard::showAlignMenu);
    menuBar()->addMenu(align_menu_);

    align_left_action_ = align_menu_->addAction(tr("Align Left Sides"));
    (void)connect(align_left_action_, &QAction::triggered, dash_view_, &DashView::alignLeft);

    align_right_action_ = align_menu_->addAction(tr("Align Right Sides"));
    (void)connect(align_right_action_, &QAction::triggered, dash_view_, &DashView::alignRight);

    align_top_action_ = align_menu_->addAction(tr("Align Top Sides"));
    (void)connect(align_top_action_, &QAction::triggered, dash_view_, &DashView::alignTop);

    align_bottom_action_ = align_menu_->addAction(tr("Align Bottom Sides"));
    (void)connect(align_bottom_action_, &QAction::triggered, dash_view_, &DashView::alignBottom);

    align_hcenter_action_ = align_menu_->addAction(tr("Align Horizontal Center"));
    (void)connect(align_hcenter_action_, &QAction::triggered, dash_view_, &DashView::alignHCenter);

    align_vcenter_action_ = align_menu_->addAction(tr("Align Vertical Center"));
    (void)connect(align_vcenter_action_, &QAction::triggered, dash_view_, &DashView::alignVCentor);

    size_menu_ = new QMenu(tr("&Size"));
    (void)connect(size_menu_, &QMenu::aboutToShow, this, &XeroDashBoard::showSizeMenu);
    menuBar()->addMenu(size_menu_);

    size_same_width_action_ = align_menu_->addAction(tr("Make Same Width"));
    (void)connect(size_same_width_action_, &QAction::triggered, dash_view_, &DashView::sizeWidth);

    size_same_height_action_ = align_menu_->addAction(tr("Make Same Height"));
    (void)connect(size_same_height_action_, &QAction::triggered, dash_view_, &DashView::sizeHeight);

    size_same_both_action_ = align_menu_->addAction(tr("Make Same Size"));
    (void)connect(size_same_both_action_, &QAction::triggered, dash_view_, &DashView::sizeBoth);
}

void XeroDashBoard::createStatus()
{
}

void XeroDashBoard::showEditMenu()
{
}

void XeroDashBoard::showFileMenu()
{
}

void XeroDashBoard::showAlignMenu()
{
}

void XeroDashBoard::showSizeMenu()
{
}

void XeroDashBoard::fileSave()
{
}

void XeroDashBoard::fileSaveAs()
{
}

void XeroDashBoard::fileLoad()
{
}

void XeroDashBoard::fileExit()
{
}

void XeroDashBoard::editPreferences()
{

}
