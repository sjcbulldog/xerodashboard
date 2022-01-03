#pragma once

#include "SingleChart.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QGridLayout>
#include <QtCore/QJsonArray>
#include <vector>
#include <memory>

class Plot;

class PlotContainer : public QWidget
{
	Q_OBJECT

public:
	PlotContainer(std::shared_ptr<Plot> plot, QWidget *parent = Q_NULLPTR);
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

protected:
	virtual void keyPressEvent(QKeyEvent* ev) override;
	virtual void paintEvent(QPaintEvent* ev) override;
	virtual void mouseDoubleClickEvent(QMouseEvent* ev) override;

private:
	void arrangeCharts();

private:
	std::shared_ptr<Plot> plot_;
	QGridLayout* layout_;
	SingleChart* selected_;
	std::vector<SingleChart *> charts_;
	QString units_;
};
