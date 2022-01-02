#include "PlotWidget.h"
#include "PlotMgr.h"
#include "Plot.h"
#include "PlotGraphWidget.h"

PlotWidget::PlotWidget(std::shared_ptr<PlotMgr> plotmgr, std::shared_ptr<NetworkTableManager> ntmgr, const QString& plotname, QWidget *parent) : QWidget(parent)
{
	ntmgr_ = ntmgr;
	plotmgr_ = plotmgr;
	plotname_ = plotname;

	setMinimumSize(600, 400);

	createWindows();

	auto plot = plotmgr_->getPlot(plotname_);
	if (plot != nullptr)
		initPlot(plot);
}

PlotWidget::~PlotWidget()
{
}

void PlotWidget::createWindows()
{
	static QStringList hdrs = { "Nodes" };

	layout_ = new QHBoxLayout();
	setLayout(layout_);

	splitter_ = new QSplitter(Qt::Horizontal);
	layout_->addWidget(splitter_);

	nodes_ = new QTreeWidget();
	splitter_->addWidget(nodes_);
	nodes_->setColumnCount(1);
	nodes_->setHeaderLabels(hdrs);
	nodes_->setDragEnabled(true);

	tabs_ = new QTabWidget();
	splitter_->addWidget(tabs_);

	QList<int> sizes;
	sizes.push_back(splitter_->width() * 3 / 10);
	sizes.push_back(splitter_->width() * 7 / 10);
	splitter_->setSizes(sizes);

	PlotGraphWidget* gr = new PlotGraphWidget(tabs_);
	tabs_->addTab(gr, plotname_);
}

void PlotWidget::initPlot(std::shared_ptr<Plot> plot)
{
	nodes_->clear();
	for (const QString& col : plot->columns())
	{
		QTreeWidgetItem* item = new QTreeWidgetItem();
		item->setText(0, col);
		nodes_->addTopLevelItem(item);
	}
}