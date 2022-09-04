#include "DriverStationDataModel.h"
#include "DSLogFile.h"
#include "DSEventFile.h"
#include <QtCore/QDir>
#include <QtCore/QDirIterator>

DriverStationDataModel::DriverStationDataModel()
{
}

void DriverStationDataModel::init(const QString& dir)
{
	dir_ = QDir(dir);
	if (!dir_.exists())
		return;

	QDirIterator iter(dir_.absolutePath(), QDirIterator::IteratorFlag::NoIteratorFlags);
	while (iter.hasNext())
	{
		QString file = iter.next();
		QFileInfo info(file);
		if (info.isFile())
		{
			QString tag = info.completeBaseName();
			if (files_.contains(tag))
				continue;

			QString logfile = dir_.absolutePath() + "/" + tag + DSLogExt;
			QString evfile = dir_.absolutePath() + "/" + tag + DSEventExt;
			QFileInfo loginfo(logfile);
			QFileInfo evinfo(evfile);

			if (loginfo.exists() && evinfo.exists())
			{
				std::shared_ptr<DSLogFile> log = std::make_shared<DSLogFile>();
				if (!log->load(logfile))
					continue;

				std::shared_ptr<DSEventFile> ev = std::make_shared<DSEventFile>();
				if (!ev->load(evfile))
					continue;

				files_.insert(tag, qMakePair(log, ev));
			}
		}
	}
}