#include "DriverStationLogTreeWidget.h"

DriverStationLogTreeWidget::DriverStationLogTreeWidget(DriverStationDataModel& model, QWidget* parent) : QTreeWidget(parent), model_(model)
{
	setHeaderHidden(true);
	setColumnCount(3);
	QTreeWidgetItem* item = new QTreeWidgetItem();
	item->setText(0, "Name");
	addTopLevelItem(item);
}

DriverStationLogTreeWidget::~DriverStationLogTreeWidget()
{
}
