#include "PlotMgr.h"
#include "Plot.h"

PlotMgr::PlotMgr(std::shared_ptr<NetworkTableManager> ntmgr, const QString &key, QObject *parent) : QObject(parent)
{
	key_ = key;
	ntmgr_ = ntmgr;
}

PlotMgr::~PlotMgr()
{
}

std::shared_ptr<Plot> PlotMgr::getAddPlot(const QString& name)
{
	std::shared_ptr<Plot> ret;

	if (plots_.contains(name))
	{
		ret = plots_.value(name);
	}
	else
	{
		ret = std::make_shared<Plot>(ntmgr_, key_, name);
		plots_.insert(name, ret);
	}

	return ret;
}

std::shared_ptr<Plot> PlotMgr::getPlot(const QString& name)
{
	std::shared_ptr<Plot> ret;

	if (plots_.contains(name))
	{
		ret = plots_.value(name);
	}

	return ret;
}

QString PlotMgr::extractPlotName(const QString& keyname)
{
	QString ret;

	assert(keyname.startsWith(key_));
	QString str = keyname.mid(key_.length());
	int index = str.indexOf('/');
	if (index == -1)
		ret = str;
	else
		ret = str.mid(0, index);

	return ret;
}


std::shared_ptr<Plot> PlotMgr::processNewKey(const QString& keyname)
{
	QString name = extractPlotName(keyname);
	auto plot = getAddPlot(name);
	plot->readData();

	return plot;
}

std::shared_ptr<Plot> PlotMgr::processUpdatedKey(const QString& keyname)
{
	QString name = extractPlotName(keyname);
	auto plot = getAddPlot(name);
	plot->readData();

	return plot;
}

QString PlotMgr::processDeletedKey(const QString& keyname)
{
	return extractPlotName(keyname);
}
