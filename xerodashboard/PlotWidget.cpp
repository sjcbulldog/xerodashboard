#include "PlotWidget.h"
#include "PlotMgr.h"
#include "Plot.h"
#include "PlotContainer.h"
#include "NetworkTableManager.h"
#include <stdexcept>

PlotWidget::PlotWidget(std::shared_ptr<PlotMgr> plotmgr, std::shared_ptr<NetworkTableManager> ntmgr, std::shared_ptr<Plot> plot, QWidget *parent) : QWidget(parent)
{
	ntmgr_ = ntmgr;
	plotmgr_ = plotmgr;
	plot_ = plot;

	setMinimumSize(600, 400);

	createWindows();
	initPlot();

	disconnect_connection_ = connect(plot.get(), &Plot::stateChanged, this, &PlotWidget::plotStateChanged);

	if (plot_->isComplete())
	{
		busy(false);
	}
	else
	{
		busy(true);
	}
}

PlotWidget::~PlotWidget()
{
	disconnect(disconnect_connection_);
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

	PlotContainer* gr = new PlotContainer(plot_);
	tabs_->addTab(gr, plot_->name());

	busy_ = new QLabel("Reading data");
	QFont font = busy_->font();
	font.setPointSizeF(24);
	busy_->setFont(font);
	busy_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

void PlotWidget::initPlot()
{
	nodes_->clear();
	for (const QString& col : plot_->columns())
	{
		QTreeWidgetItem* item = new QTreeWidgetItem();
		item->setText(0, col);
		nodes_->addTopLevelItem(item);
	}
}

void PlotWidget::plotStateChanged(Plot::State oldst, Plot::State newst)
{
	if (plot_->isComplete())
	{
		busy(false);
	}
	else
	{
		busy(true);
	}
}

void PlotWidget::busy(bool b)
{
	if (b)
	{
		tabs_->setParent(nullptr);
		splitter_->addWidget(busy_);
	}
	else
	{
		busy_->setParent(nullptr);
		splitter_->addWidget(tabs_);
	}
}
