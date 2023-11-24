#include "PlotListWidget.h"
#include "NetworkTableManager.h"
#include <QtCore/QDebug>
#include <QtCore/QMimeData>
#include "PlotMgr.h"
#include "Plot.h"

PlotListWidget::PlotListWidget(std::shared_ptr<NetworkTableManager> ntmgr, std::shared_ptr<PlotMgr> plotmgr, 
							   const QString& plotkey, QWidget* parent) : XeroTreeWidgetBase(parent)
{
	plot_key_ = plotkey;
	ntmgr_ = ntmgr;
	plotmgr_ = plotmgr;

	static QStringList labels = { "Name", "Status" };
	setColumnCount(2);
	setHeaderLabels(labels);
	setSortingEnabled(true);

	setDragDropMode(DragDropMode::DragOnly);
	setDragEnabled(true);

	setSelectionMode(QAbstractItemView::SingleSelection);

	(void)connect(ntmgr_.get(), &NetworkTableManager::newEntry, this, &PlotListWidget::newEntryDetected);
	(void)connect(ntmgr_.get(), &NetworkTableManager::updatedEntry, this, &PlotListWidget::updatedEntryDetected);
	(void)connect(ntmgr_.get(), &NetworkTableManager::deletedEntry, this, &PlotListWidget::deletedEntryDetected);

	readExistingPlots();
}

PlotListWidget::~PlotListWidget()
{
}

void PlotListWidget::readExistingPlots()
{
	QStringList keys = ntmgr_->getSubKeys(plot_key_);
	for (const QString& key : keys)
	{
		QString longname = plot_key_ + key;
		auto plot = plotmgr_->processNewKey(longname);
		if (plot != nullptr) {
			updatePlotStatus(plot);
		}
	}
}

QMimeData* PlotListWidget::mimeData(const QList<QTreeWidgetItem*> &items) const
{
	QMimeData* data = nullptr;

	if (items.size() == 1)
	{
		data = new QMimeData();
		data->setText("PLOT:" + items.at(0)->text(0));
	}

	return data;
}

bool PlotListWidget::processPlotKeyString(const QString& name, QString& plotname, QStringList& key)
{
	bool ret = false;
	assert(name.startsWith(plot_key_));

	QString subname = name.mid(plot_key_.length());
	key = subname.split('/');
	if (key.size() >= 2)
	{
		if (key.at(0).length() == 0)
			key.removeAt(0);

		plotname = key.at(0);
		key.removeAt(0);

		ret = true;
	}

	return ret;
}

void PlotListWidget::updatePlotStatus(std::shared_ptr<Plot> plot)
{
	auto item = findTopLevelItem(plot->name());
	if (item == nullptr)
	{
		item = new QTreeWidgetItem();
		item->setText(0, plot->name());
		item->setToolTip(0, plot->name());
		addTopLevelItem(item);
	}

	item->setText(1, plot->statusString());
}

void PlotListWidget::newEntryDetected(const nt::TopicInfo& info)
{
	QString plotname;
	QStringList key;

	QString name = QString::fromStdString(info.name);
	if (name.startsWith(plot_key_))
	{
		auto plot = plotmgr_->processNewKey(name);
		if (plot != nullptr) {
			updatePlotStatus(plot);
			topics_[info.topic] = name;
		}
	}
}

void PlotListWidget::updatedEntryDetected(const nt::ValueEventData& data)
{
	QString plotname;
	QStringList key;

	if (topics_.contains(data.topic))
	{
		QString name = topics_[data.topic];
		auto plot = plotmgr_->processUpdatedKey(name);
		if (plot != nullptr)
			updatePlotStatus(plot);
	}
}

void PlotListWidget::deletedEntryDetected(const nt::TopicInfo& info)
{
	QString plotname;
	QStringList key;

	QString name = QString::fromStdString(info.name);
	if (name.startsWith(plot_key_))
	{
		plotmgr_->processDeletedKey(name);
	}
}