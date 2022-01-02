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

	void layout() {
		child_->setGeometry(BorderThickness, header_height_, width() - BorderThickness * 2, height() - header_height_ - BorderThickness);
	}

protected:
	void paintEvent(QPaintEvent* ev);
	void mousePressEvent(QMouseEvent* ev);
	void resizeEvent(QResizeEvent* ev);
	void changeEvent(QEvent* ev);

private:
	QRect closeBoxRect();

private:
	static constexpr const int BorderThickness = 1;

private:
	QString title_;
	QWidget* child_;
	int header_height_;
};
