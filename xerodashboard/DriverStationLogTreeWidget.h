#pragma once

#include <QtWidgets/QTreeWidget>

class DriverStationDataModel;

class DriverStationLogTreeWidget : public QTreeWidget
{
	Q_OBJECT

public:
	DriverStationLogTreeWidget(DriverStationDataModel& model, QWidget* parent);
	virtual ~DriverStationLogTreeWidget();

private:
	DriverStationDataModel& model_;
};

