#include "Plot.h"
#include <QtCore/QDebug>
#include "NetworkTableManager.h"

Plot::Plot(std::shared_ptr<NetworkTableManager> ntmgr, const QString &key, const QString& name)
{
	ntmgr_ = ntmgr;
	name_ = name;
	key_ = key;
	points_ = -1;
	saw_complete_ = false;
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

void dump(const std::vector<bool>& has)
{
	QString str = "hasdata:";
	for (int i = 0; i < has.size(); i++)
		str += has[i] ? "1" : "0";
	qDebug() << str;
}

void Plot::readRowData(int row)
{
	QString datakey = key_ + "/" + name_ + "/data/" + QString::number(row);

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

void Plot::addData(int row, const QVector<double>& data)
{
	if (columns_.size() > 0 && data.size() != columns_.size())
		return;

	if (row >= hasdata_.size())
		hasdata_.resize(row + 1, false);

	if (row >= data_.size())
		data_.resize(row + 1);

	for (int i = 0; i < row; i++)
	{
		if (!hasdata_[row])
			readRowData(i);
	}

	data_[row] = data;
	hasdata_[row] = true;

	checkComplete();
}

void Plot::checkComplete()
{
	if (!isDisconnected())
	{
		if (saw_complete_ && points_ != -1 && points_ == data_.size())
		{
			bool ret = true;

			for (int i = 0; i < points_; i++)
			{
				if (!hasdata_[i])
				{
					ret = false;
					break;
				}
			}

			changeState(ret ? State::Complete : State::Reading);
		}
	}
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