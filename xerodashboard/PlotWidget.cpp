#include "PlotWidget.h"
#include <QtWidgets/QBoxLayout>
#include "PlotMgr.h"
#include "Plot.h"
#include "PlotContainer.h"
#include "NetworkTableManager.h"
#include "JsonFieldNames.h"
#include <stdexcept>

PlotWidget::PlotWidget(std::shared_ptr<PlotMgr> plotmgr, std::shared_ptr<NetworkTableManager> ntmgr, const QString &plotname, QWidget *parent) : QWidget(parent)
{
	ntmgr_ = ntmgr;
	plotmgr_ = plotmgr;
	plotname_ = plotname;

	// Watch for new plots when they are added
	plot_added_connection_ = connect(plotmgr.get(), &PlotMgr::plotAdded, this, &PlotWidget::plotAddedDetected);

	// Set the min size for this window, below this is not really useful
	setMinimumSize(600, 400);

	// Create the windows assocaited with the plot widget
	createWindows();

	// Attach the plot if it is availble, if it is not that is OK, we will attach when it is added
	attachPlot();

	// Configure the window to the right of the nodes window
	if (plot_ == nullptr || !plot_->isComplete())
	{
		busy(true);
	}
	else
	{
		busy(false);
	}
}

PlotWidget::~PlotWidget()
{
	if (plot_ != nullptr)
		disconnect(disconnect_connection_);

	disconnect(plot_added_connection_);
}

void PlotWidget::plotAddedDetected(const QString &plotname)
{
	if (plotname == plotname_)
		attachPlot();
}

bool PlotWidget::attachPlot()
{
	if (plot_ != nullptr && plot_->name() == plotname_)
		return true;

	assert(plot_ == nullptr);

	plot_ = plotmgr_->getPlot(plotname_);
	if (plot_ != nullptr)
	{
		disconnect_connection_ = connect(plot_.get(), &Plot::stateChanged, this, &PlotWidget::plotStateChanged);
		initPlot();
	}

	return plot_ != nullptr;
}

QJsonObject PlotWidget::getJSONDesc() const
{
	QJsonObject obj;
	obj[JsonFieldNames::Type] = JsonFieldNames::TypeValuePlot;
	obj[JsonFieldNames::PlotName] = plotname_;

	QJsonArray sizes;
	sizes.push_back(splitter_->sizes().at(0));
	sizes.push_back(splitter_->sizes().at(1));
	obj[JsonFieldNames::SplitterWidth] = sizes;

	QJsonArray containers;
	for (int i = 0; i < tabs_->count(); i++)
	{
		PlotContainer* plots = dynamic_cast<PlotContainer*>(tabs_->widget(i));
		if (plots != nullptr)
		{
			QJsonArray chobj = plots->getJSONDesc();
			containers.push_back(chobj);
		}
	}
	obj[JsonFieldNames::Constainers] = containers;

	return obj;
}

bool PlotWidget::restoreFromJson(const QJsonObject& obj)
{
	if (!obj.contains(JsonFieldNames::Constainers) || !obj.value(JsonFieldNames::Constainers).isArray())
		return false;

	if (obj.contains(JsonFieldNames::SplitterWidth) && obj.value(JsonFieldNames::SplitterWidth).isArray())
	{
		splitter_sizes_.clear();
		QJsonArray sizes = obj.value(JsonFieldNames::SplitterWidth).toArray();
		for (int i = 0; i < sizes.count(); i++)
		{
			if (sizes.at(i).isDouble())
				splitter_sizes_.push_back(sizes.at(i).toInt());
		}
		if (splitter_sizes_.size() == 2)
			splitter_->setSizes(splitter_sizes_);
	}

	while(tabs_->count() > 0)
	{
		auto* gr = tabs_->widget(0);
		tabs_->removeTab(0);
		delete gr;
	}

	tabs_->clear();
	QJsonArray arr = obj.value(JsonFieldNames::Constainers).toArray();
	for (int i = 0; i < arr.size(); i++)
	{
		if (arr.at(i).isArray())
		{
			PlotContainer* cont = new PlotContainer(plotmgr_, plotname_);
			tabs_->addTab(cont, plotname_);

			cont->removeAllCharts();
			cont->restoreFromJson(arr.at(i).toArray());
		}
	}

	return true;
}

void PlotWidget::createWindows()
{
	static QStringList hdrs = { "Nodes" };

	QBoxLayout* layout = new QHBoxLayout();
	setLayout(layout);

	stack_ = new QStackedWidget(this);
	layout->addWidget(stack_);

	splitter_ = new QSplitter(Qt::Horizontal);
	stack_->addWidget(splitter_);

	nodes_ = new QTreeWidget();
	splitter_->addWidget(nodes_);
	nodes_->setColumnCount(1);
	nodes_->setHeaderLabels(hdrs);
	nodes_->setDragEnabled(true);

	tabs_ = new QTabWidget();
	splitter_->addWidget(tabs_);

	splitter_sizes_.push_back(splitter_->width() * 3 / 10);
	splitter_sizes_.push_back(splitter_->width() * 7 / 10);
	splitter_->setSizes(splitter_sizes_);

	PlotContainer* gr = new PlotContainer(plotmgr_, plotname_);
	gr->createDefaultChart();
	tabs_->addTab(gr, plotname_);

	busy_ = new QLabel("No data");
	QFont font = busy_->font();
	font.setPointSizeF(36);
	busy_->setFont(font);
	busy_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	stack_->addWidget(busy_);
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
		if (nodes_->topLevelItemCount() == 0)
			initPlot();
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
		stack_->setCurrentIndex(1);
		if (plot_ == nullptr)
			busy_->setText("No Data");
		else
			busy_->setText("Reading Data");
	}
	else
	{
		stack_->setCurrentIndex(0);
		splitter_->setSizes(splitter_sizes_);
	}
}
