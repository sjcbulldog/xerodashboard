#include "Plot.h"

Plot::Plot(const QString& name)
{
	name_ = name;
	points_ = -1;
	is_complete_ = false;
	saw_complete_ = false;
}

Plot::~Plot()
{
}

void Plot::addData(int row, const QVector<double>& data)
{
	if (columns_.size() > 0 && data.size() != columns_.size())
		return;

	if (row >= hasdata_.size())
		hasdata_.resize(row + 1, false);

	if (row >= data_.size())
		data_.resize(row + 1);

	data_[row] = data;
	hasdata_[row] = true;

	checkComplete();
}

void Plot::checkComplete()
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

		is_complete_ = ret;
	}

	if (is_complete_)
		emit dataComplete();
}