#include "DSLogFile.h"
#include <QtCore/QFile>

DSLogFile::DSLogFile()
{
}

bool DSLogFile::load(const QString& filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
		return false;

	version_ = get4(file);
	get4(file);
	timestamp_ = get4(file);
	fractime_ = get1(file);
	file.read(7);

	while (!file.atEnd())
	{
		auto entry = std::make_shared<DSLogFileEntry>();
		entry->setTripTime(get1(file) / 15);							// trip time ms
		entry->setPacketLoss(get1(file) * 4);							// packet loss %
		entry->setBatteryLevel(get1(file) + get1(file) / 256);			// Battery level
		entry->setCPUPercent(get1(file) / 2);							// CPU %
		entry->setTrace(get1(file));									// Trace
		entry->setCANUtilization(get1(file) / 2);						// Can Utilization %
		entry->setWifiDb(get1(file) / 2);								// WIFI DB
		entry->setWiFiMb(get1(file));									// WIFI MB
		get2(file);
		file.read(21);
	}
}

