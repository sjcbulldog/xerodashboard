#pragma once

#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <QtCore/QString>
#include <QtCore/QJsonObject>
#include <string>
#include <map>
#include <memory>
#include "Callout.h"
#include "TabEditName.h"
#include "PlotMgr.h"
#include "Plot.h"

class SingleChart : public QtCharts::QChartView
{
public:
	SingleChart(QString units, std::shared_ptr<PlotMgr> plot, const QString &plotname, QWidget *parent = Q_NULLPTR);
	~SingleChart();

	void setUnits(QString units);
	QString units() const {
		return units_;
	}

	void setTitle(QString title);

	QString title() const {
		return title_;
	}

	void editTitle();

	void highlight(bool high);

	QJsonObject getJSONDesc() const;
	bool restoreFromJson(const QJsonObject& obj);

	void insertNode(const QString& node);

protected:
	virtual void dragEnterEvent(QDragEnterEvent* event) override;
	virtual void dragMoveEvent(QDragMoveEvent* event) override;
	virtual void dragLeaveEvent(QDragLeaveEvent* event) override;
	virtual void dropEvent(QDropEvent* ev) override;
	virtual void keyPressEvent(QKeyEvent* ev) override;
	virtual void mouseMoveEvent(QMouseEvent* ev) override;
	virtual void focusInEvent(QFocusEvent* ev) override;

private:
	QtCharts::QValueAxis* findAxis(QString axisname);
	QtCharts::QLineSeries* findSeries(QString node);
	QString nodeToAxis(QString node);

	void initChart();

	void setMinMax(const std::string& name, double minv, double maxv);
	void getMinMax(const std::string& name, double &minv, double &maxv);

	void dataAdded();
	void dataReset();

	void createTimeAxis();
	void createLegend();

	void seriesHover(QtCharts::QLineSeries* series, const QPointF& point, bool state);
	void seriesClick(QtCharts::QLineSeries* series, const QPointF& point);

	void editorDone();
	void editorAborted();

	QStringList computeAverages();
	QGraphicsLineItem* createLine(double stime, QPen p);
	void displayAverages();
	void clearAnnotations();
	int findClosest(QVector<QPointF>& pts, double v);

	void plotStateChanged(Plot::State oldst, Plot::State newst);

	bool attachPlot();
	void resetNodes();

	void plotAddedDetected(const QString& plotname);

private:
	// The plot we are displaying, may be null
	std::shared_ptr<Plot> plot_;

	// The plot manager
	std::shared_ptr<PlotMgr> plotmgr_;

	// The plot we are displaying
	QString plotname_;

	// The time axis for the graph
	QtCharts::QValueAxis* time_;

	// The first series for the graph
	QtCharts::QAbstractSeries* first_;

	// The legend for the graph
	QtCharts::QLegend* legend_;

	// The time min and max values
	double tminv_, tmaxv_;

	// The min and max values for each of the plotted nodes
	std::map<std::string, std::pair<double, double>> min_max_;

	int total_scroll_x_;
	int total_scroll_y_;

	// The current callout
	Callout* callout_;

	// The list of callouts displayed
	std::list<Callout*> callouts_;

	// The units for position, velocity, and acceleration units
	QString units_;

	// The title of this graph
	QString title_;

	// The number of axis on the left side of the graph
	int left_axis_count_;

	// The number of axis on the right side of the graph
	int right_axis_count_;

	// The Qt signal connection to listen to state changes in the plot
	QMetaObject::Connection connection_;

	// The editor to use to edit the tab name
	TabEditName* editor_;

	// The current mouse position
	QPointF mouse_;

	// The nodes added to the chart
	QStringList nodes_;

	bool first_point_;
	bool first_valid_;
	bool second_valid_;
	double first_value_;
	double second_value_;

	QGraphicsLineItem *first_line_;
	QGraphicsLineItem* second_line_;
	QGraphicsRectItem* rect_item_;

	std::list<QGraphicsSimpleTextItem*> text_items_;

	bool is_complete_;

	QMetaObject::Connection plot_added_connection_;

	int callno_;
};
