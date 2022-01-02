#pragma once

#include <QtWidgets/QWidget>

class XeroItemFrame : public QWidget
{
	Q_OBJECT

public:
	XeroItemFrame(QWidget *parent = Q_NULLPTR);
	~XeroItemFrame();

	void setTitle(const QString& title) {
		title_ = title;
	}

	void setWidget(QWidget* child) {
		child_ = child;
		child_->setVisible(true);
		layout();
	}

	void layout();

protected:
	void paintEvent(QPaintEvent* ev);
	void mousePressEvent(QMouseEvent* ev);
	void mouseMoveEvent(QMouseEvent* ev);
	void mouseReleaseEvent(QMouseEvent* ev);
	void resizeEvent(QResizeEvent* ev);
	void changeEvent(QEvent* ev);

private:
	QRect closeBoxRect();
	QRect headerRect();

private:
	static constexpr const int BorderThickness = 1;

private:
	QString title_;
	QWidget* child_;
	int header_height_;
	bool dragging_;
	QPoint mouse_;
	QPoint window_;
};
