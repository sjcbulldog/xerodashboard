#pragma once

#include "SingleChart.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QGridLayout>
#include <QtCore/QJsonArray>
#include <vector>
#include <memory>

class PlotMgr;

class PlotContainer : public QWidget
{
	Q_OBJECT

public:
	PlotContainer(std::shared_ptr<PlotMgr> plotmgr, const QString &plotname, QWidget *parent = Q_NULLPTR);
	~PlotContainer();

	void childFocused(SingleChart*);
	void setUnits(QString units);
	QString units() const {
		return units_;
	}

	void editTitle() {
		if (selected_ != nullptr)
			selected_->editTitle();
	}

	int count() const {
		return charts_.size();
	}

	SingleChart* chart(int index) {
		return charts_[index];
	}

	QJsonArray getJSONDesc() const;
	bool restoreFromJson(const QJsonArray& charts);

	void removeAllCharts();
	void createDefaultChart();

protected:
	virtual void keyPressEvent(QKeyEvent* ev) override;
	virtual void paintEvent(QPaintEvent* ev) override;
	virtual void mouseDoubleClickEvent(QMouseEvent* ev) override;

private:
	void arrangeCharts();
	void removeChart(SingleChart* ch);

private:
	std::shared_ptr<PlotMgr> plotmgr_;
	QString plotname_;
	QGridLayout* layout_;
	SingleChart* selected_;
	std::vector<SingleChart *> charts_;
	QString units_;
};
