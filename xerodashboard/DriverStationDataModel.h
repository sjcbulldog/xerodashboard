#pragma once

#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QMap>

class DSLogFile;
class DSEventFile;

class DriverStationDataModel
{
public:
	DriverStationDataModel();
	virtual ~DriverStationDataModel() = default;

	void init(const QString& dir);

private:
	static const constexpr char* DSLogExt = ".dslog";
	static const constexpr char* DSEventExt = ".dsevents";

private:
	QDir dir_;
	QMap<QString, QPair<std::shared_ptr<DSLogFile>, std::shared_ptr<DSEventFile>>> files_;
};

