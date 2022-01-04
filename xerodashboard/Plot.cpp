#include "Plot.h"
#include <QtCore/QDebug>
#include "NetworkTableManager.h"

Plot::Plot(std::shared_ptr<NetworkTableManager> ntmgr, const QString &key, const QString& name)
{
	ntmgr_ = ntmgr;
	name_ = name;
	key_ = key;
	points_ = -1;
	state_ = State::New;

	disconnect_connection_ = connect(ntmgr.get(), &NetworkTableManager::disconnected, this, &Plot::disconnectDetected);
}

Plot::~Plot()
{
	disconnect(disconnect_connection_);
}

void Plot::disconnectDetected()
{
	if (state_ == State::Complete)
		changeState(State::CompleteDisconnected);
	else
		changeState(State::ReadingDisconnected);
}

void Plot::readRowData(int row)
{
	QString datakey = key_  + name_ + "/data/" + QString::number(row);

	auto entry = ntmgr_->getEntry(datakey);
	auto value = entry.GetValue();
	if (value != nullptr && value->IsValid() && value->IsDoubleArray())
	{
		auto data = value->GetDoubleArray();
		if (data.size() == columns_.size())
		{
			QVector<double> rowdata;
			for (int i = 0; i < data.size(); i++)
			{
				rowdata.push_back(data[i]);
			}
			data_[row] = rowdata;
			hasdata_[row] = true;
		}
	}
}

std::shared_ptr<nt::Value> Plot::getPlotValue(const QString& name)
{
	QString plotbase = key_ + name_ + "/";
	return 	ntmgr_->getEntry(plotbase + name).GetValue();
}

void Plot::readData()
{
	bool newiscomp = true;

	auto value = getPlotValue("complete");
	if (value == nullptr || !value->IsValid() || !value->IsBoolean())
		newiscomp = false;
	else
	{
		newiscomp = value->GetBoolean();
	}

	if (newiscomp == false && isComplete())
	{
		//
		// The plot is transitioning from a complete status to a reading state, reset
		// the various stuff that needs to be reset
		//
		data_.resize(0);
		hasdata_.resize(0);
		columns_.clear();
	}

	value = getPlotValue("points");
	if (value == nullptr || !value->IsValid() || !value->IsDouble())
		return;

	points_ = static_cast<int>(value->GetDouble());

	value = getPlotValue("columns");
	if (value == nullptr || !value->IsValid() || !value->IsStringArray())
		return;

	if (columns_.size() == 0)
	{
		auto colnames = value->GetStringArray();
		for (int i = 0; i < colnames.size(); i++)
			columns_.push_back(QString::fromStdString(colnames[i]));
	}

	if (data_.size() < points_)
	{
		data_.resize(points_);
	}

	if (hasdata_.size() < points_)
	{
		hasdata_.resize(points_, false);
	}

	for (int row = 0; row < points_; row++)
	{
		if (!hasdata_[row])
		{
			readRowData(row);
		}
	}

	value = getPlotValue("complete");

	if (value != nullptr && value->IsValid() && value->IsBoolean())
	{
		if (value->GetBoolean())
		{
			if (connected_)
				changeState(State::Complete);
			else
				changeState(State::CompleteDisconnected);
		}
		else
		{
			if (connected_)
				changeState(State::Reading);
			else
				changeState(State::ReadingDisconnected);
		}
	}
	else
	{
		if (connected_)
			changeState(State::Reading);
		else
			changeState(State::ReadingDisconnected);
	}
}

QString Plot::statusString()
{
	QString ret;

	switch (state_)
	{
	case State::New:
		ret = "new";
		break;

	case State::Complete:
		ret = "complete";
		break;

	case State::CompleteDisconnected:
		ret = "complete/disconnected";
		break;

	case State::Reading:
		ret = "reading";
		break;

	case State::ReadingDisconnected:
		ret = "disconnected";
		break;
	}

	ret += " (" + QString::number(data_.size()) + ")";

	return ret;
}

void Plot::changeState(State newst)
{
	if (state_ != newst)
	{
		State oldst = state_;
		state_ = newst;
		emit stateChanged(oldst, state_);
	}
}