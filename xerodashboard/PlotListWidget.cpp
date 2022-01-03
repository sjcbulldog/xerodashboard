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
}

PlotListWidget::~PlotListWidget()
{
}

QMimeData* PlotListWidget::mimeData(const QList<QTreeWidgetItem*> items) const
{
	QMimeData* data = nullptr;

	if (items.size() == 1)
	{
		data = new QMimeData();
		data->setText("PLOT:" + items.at(0)->text(0));
	}

	return data;
}

QTreeWidgetItem* PlotListWidget::findTopLevelItem(const QString& name)
{
	for (int i = 0; i < topLevelItemCount(); i++)
	{
		auto item = topLevelItem(i);
		if (item->text(0) == name)
			return item;
	}

	return nullptr;
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

QTreeWidgetItem *PlotListWidget::processPlot(const QString &path, QString &plotname, QStringList &key)
{
	QTreeWidgetItem* item = nullptr;

	if (processPlotKeyString(path, plotname, key))
	{
		item = findTopLevelItem(plotname);
		if (item == nullptr)
		{
			item = new QTreeWidgetItem();
			item->setText(0, plotname);
			item->setText(1, "Reading");
			addTopLevelItem(item);
			sortItems(0, Qt::SortOrder::AscendingOrder);
		}
	}

	return item;
}

void PlotListWidget::processNewData(QTreeWidgetItem* item, const QString &path, const QString &plotname, const QStringList &keyname)
{
	std::shared_ptr<Plot> plot = plotmgr_->getAddPlot(plotname);
	if (plot == nullptr)
		return;

	QString plotpath = plot_key_ + "/" + plotname + "/";

	auto value = ntmgr_->getEntry(plotpath + "complete").GetValue();
	if (value != nullptr && value->IsValid() && value->IsBoolean())
		plot->setComplete(value->GetBoolean());

	value = ntmgr_->getEntry(plotpath + "points").GetValue();
	if (value != nullptr && value->IsValid() && value->IsDouble())
		plot->setPoints(static_cast<int>(value->GetDouble()));

	value = ntmgr_->getEntry(plotpath + "columns").GetValue();
	if (value != nullptr && value->IsValid() && value->IsStringArray() && plot->columns().size() == 0)
	{
		QStringList cols;

		auto colnames = value->GetStringArray();
		for (int i = 0; i < colnames.size(); i++)
			cols.push_back(QString::fromStdString(colnames[i]));
		plot->setColumns(cols);
	}

	if (keyname.at(0) == "data")
	{
		assert(keyname.size() == 2);

		value = ntmgr_->getEntry(plotpath + "data/" + keyname.at(1)).GetValue();
		if (value->IsValid() && value->IsDoubleArray())
		{
			bool ok;
			int rowno = keyname.at(1).toInt(&ok);
			if (ok)
			{
				QVector<double> data;
				auto values = value->GetDoubleArray();
				for (int i = 0; i < values.size(); i++)
					data.push_back(values[i]);

				plot->addData(rowno, data);
			}
		}
	}

	switch (plot->state()) {
	case Plot::State::Complete:
		item->setText(1, "Complete (" + QString::number(plot->pointsLoaded()) + ")");
		break;

	case Plot::State::CompleteDisconnected:
		item->setText(1, "Disconnected (" + QString::number(plot->pointsLoaded()) + ")");
		break;

	case Plot::State::Reading:
		item->setText(1, "Reading (" + QString::number(plot->pointsLoaded()) + ")");
		break;
	}
}

void PlotListWidget::newEntryDetected(const QString& name)
{
	QString plotname;
	QStringList key;

	if (name.startsWith(plot_key_))
	{
		auto item = processPlot(name, plotname, key);
		if (item != nullptr)
		{
			processNewData(item, name, plotname, key);
		}
	}
}

void PlotListWidget::updatedEntryDetected(const QString& name)
{
	QString plotname;
	QStringList key;

	if (name.startsWith(plot_key_))
	{
		auto item = processPlot(name, plotname, key);
		if (item != nullptr)
		{
			processNewData(item, name, plotname, key);
		}
	}
}

void PlotListWidget::deletedEntryDetected(const QString& path)
{
	QString plotname;
	QStringList key;

	if (path.startsWith(plot_key_))
	{
		if (processPlotKeyString(path, plotname, key))
		{
			auto item = findTopLevelItem(plotname);
			item->setText(2, "Deleted");
		}
	}
}