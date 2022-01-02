#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVector>

class Plot : public QObject
{
	Q_OBJECT

public:
	Plot(const QString& name);
	virtual ~Plot();

	const QString& name() const {
		return name_;
	}

	bool isComplete() const {
		return is_complete_;
	}

	int pointsLoaded() const {
		return std::count(hasdata_.begin(), hasdata_.end(), true);
	}

	void setComplete(bool comp) {
		saw_complete_ = comp;

		if (comp == false)
		{
			points_ = -1;
			std::fill(hasdata_.begin(), hasdata_.end(), false);
			is_complete_ = false;

			emit restarted();
		}
		else
		{
			checkComplete();
		}
	}

	void setColumns(const QStringList& cols) {
		columns_ = cols;
		points_ = -1;
		is_complete_ = false;
	}

	const QStringList& columns() const {
		return columns_;
	}

	void setPoints(int pts) {
		points_ = pts;
		checkComplete();
	}

	void addData(int rowno, const QVector<double>& row);

signals:
	void dataComplete();
	void restarted();

private:
	void checkComplete();

private:
	bool is_complete_;
	bool saw_complete_;
	QString name_;
	int points_;
	QStringList columns_;
	QVector<QVector<double>> data_;
	std::vector<bool> hasdata_;
};

