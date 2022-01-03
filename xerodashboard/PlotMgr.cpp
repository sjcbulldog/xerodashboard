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
