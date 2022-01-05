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