#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVector>
#include <memory>

class NetworkTableManager;

class Plot : public QObject
{
	Q_OBJECT

public:
	enum class State
	{
		New,
		Reading,
		ReadingDisconnected,
		Complete,
		CompleteDisconnected
	};

	Plot(std::shared_ptr<NetworkTableManager> ntmgr, const QString &key, const QString& name);
	virtual ~Plot();

	bool isDisconnected() const {
		return state_ == State::CompleteDisconnected || state_ == State::ReadingDisconnected;
	}

	bool isComplete() const {
		return state_ == State::Complete || state_ == State::CompleteDisconnected;
	}

	const QString& name() const {
		return name_;
	}

	State state() const {
		return state_;
	}

	int pointsLoaded() const {
		return std::count(hasdata_.begin(), hasdata_.end(), true);
	}

	void setComplete(bool comp) {
		if (saw_complete_ == true && comp == false)
		{
			points_ = -1;
			hasdata_.resize(0);
			data_.resize(0);
			columns_.clear();
		}

		saw_complete_ = comp;
		if (comp == false)
		{
			changeState(State::Reading);
		}
		else
		{
			checkComplete();
		}
	}

	void setColumns(const QStringList& cols) {
		columns_ = cols;
		points_ = -1;
		changeState(State::Reading);
	}

	const QStringList& columns() const {
		return columns_;
	}

	void setPoints(int pts) {
		points_ = pts;
		checkComplete();
	}

	void addData(int rowno, const QVector<double>& row);

	int getColumnIndex(const QString& name) const {
		for (int i = 0; i < columns_.size(); i++)
		{
			if (columns_[i] == name)
				return i;
		}

		return -1;
	}

	double getData(int row, int column) const {
		return (data_[row])[column];
	}

	int numColumns() const {
		return columns_.size();
	}

	int numRows() const {
		return data_.size();
	}

	const QVector<double> getRow(int row) const {
		return data_[row];
	}

signals:
	void stateChanged(State oldst, State newst);

private:
	void checkComplete();
	void disconnectDetected();
	void changeState(State newst);
	void readRowData(int row);

private:
	std::shared_ptr<NetworkTableManager> ntmgr_;
	QMetaObject::Connection disconnect_connection_;
	QString key_;

	State state_;
	bool saw_complete_;
	QString name_;
	int points_;
	QStringList columns_;
	QVector<QVector<double>> data_;
	std::vector<bool> hasdata_;
	bool connected_;
};

