#include "xerodashboard.h"
#include <QtWidgets/QMenuBar>
#include "NetworkTableManager.h"
#include "NetworkTableTreeWidget.h"
#include "PlotListWidget.h"
#include "DashView.h"
#include "PlotMgr.h"

XeroDashBoard::XeroDashBoard(QWidget *parent) : QMainWindow(parent)
{
    ntmgr_ = std::make_shared<NetworkTableManager>("127.0.0.1");
    (void)connect(&timer_, &QTimer::timeout, this, &XeroDashBoard::timerCallback);
    timer_.start(std::chrono::milliseconds(100));

    plotmgr_ = std::make_shared<PlotMgr>(nullptr);

    setMinimumWidth(800);
    setMinimumHeight(600);

    createWindows();
    createMenus();
    createStatus();
}

XeroDashBoard::~XeroDashBoard()
{
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
    plot_list_view_ = new PlotListWidget(ntmgr_, plotmgr_, "/XeroPlot", plot_list_dock_);
    plot_list_dock_->setWidget(plot_list_view_);
    addDockWidget(Qt::LeftDockWidgetArea, plot_list_dock_);
}

void XeroDashBoard::createMenus()
{
    QAction* action;

    edit_menu_ = new QMenu(tr("&Edit"));
    (void)connect(edit_menu_, &QMenu::aboutToShow, this, &XeroDashBoard::showEditMenu);
    menuBar()->addMenu(edit_menu_);

    action = edit_menu_->addAction(tr("Preferences"));
    (void)connect(action, &QAction::triggered, this, &XeroDashBoard::editPreferences);
}

void XeroDashBoard::createStatus()
{
}

void XeroDashBoard::showEditMenu()
{
}

void XeroDashBoard::editPreferences()
{
}