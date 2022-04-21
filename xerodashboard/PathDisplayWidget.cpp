#include "PathDisplayWidget.h"
#include "PathDisplayWidget.h"
#include <QtGui/QPainter>
#include <QtWidgets/QMenu>
#include <QtGui/QContextMenuEvent>
#include "NetworkTableManager.h"
#include "GameFieldManager.h"
#include "GameField.h"

PathDisplayWidget::PathDisplayWidget(GameFieldManager &mgr, std::shared_ptr<NetworkTableManager> ntmgr, QWidget *parent) : QWidget(parent), mgr_(mgr)
{
	image_scale_ = 1.0;
	setField(mgr_.getDefaultField());
	setContextMenuPolicy(Qt::DefaultContextMenu) ;

	setUnits("m");

	ntmgr_ = ntmgr;
	connected_ = false;

	robot_location_ = ntmgr_->getEntry(RobotLocationNetworkTableEntry);
	path_location_ = ntmgr_->getEntry(PathLocationNetworkTableEntry);

	update_connection_ = connect(ntmgr.get(), &NetworkTableManager::updatedEntry, this, &PathDisplayWidget::entryUpdateDetected);
	update_connection_ = connect(ntmgr.get(), &NetworkTableManager::newEntry, this, &PathDisplayWidget::newDetected);
	disconnect_connection_ = connect(ntmgr.get(), &NetworkTableManager::disconnected, this, &PathDisplayWidget::disconnectDetected);
	connect_connection_ = connect(ntmgr.get(), &NetworkTableManager::connected, this, &PathDisplayWidget::connectDetected);

	setMinimumSize(400, 200);
}

PathDisplayWidget::~PathDisplayWidget()
{
	disconnect(update_connection_);
	disconnect(new_connection_);
	disconnect(disconnect_connection_);
	disconnect(connect_connection_);
}

void PathDisplayWidget::disconnectDetected()
{
	if (connected_ != false)
	{
		connected_ = false;
		robot_track_.clear();
		path_track_.clear();
	}
}

void PathDisplayWidget::connectDetected()
{
	if (connected_ != true)
	{
		connected_ = true;
		robot_track_.clear();
		path_track_.clear();
	}
}

void PathDisplayWidget::entryUpdateDetected(const QString& name)
{
	if (name == RobotLocationNetworkTableEntry)
	{
		auto pos = robot_location_.GetDoubleArray({ -1000000.0, -1000000.0, -1000000.0 });
		if (pos.at(0) > -1000.0 && pos.at(1) > -1000 && pos.at(2) > -1000) 
		{
			Pose2d pose(pos.at(0), pos.at(1), Rotation2d::fromDegrees(pos.at(2)));
			robot_track_.push_back(pose);
			update();
		}
	}
	else if (name == PathLocationNetworkTableEntry)
	{
		auto pos = path_location_.GetDoubleArray({ -1000000.0, -1000000.0, -1000000.0 });
		if (pos.at(0) > -1000.0 && pos.at(1) > -1000 && pos.at(2) > -1000)
		{
			Pose2d pose(pos.at(0), pos.at(1), Rotation2d::fromDegrees(pos.at(2)));
			path_track_.push_back(pose);
			update();
		}
	}
}

void PathDisplayWidget::newDetected(const QString& name)
{
	if (name == RobotLocationNetworkTableEntry)
	{
		auto pos = robot_location_.GetDoubleArray({ -1000000.0, -1000000.0, -1000000.0 });
		if (pos.at(0) > -1000.0 && pos.at(1) > -1000 && pos.at(2) > -1000)
		{
			Pose2d pose(pos.at(0), pos.at(1), Rotation2d::fromDegrees(pos.at(2)));
			robot_track_.push_back(pose);
			update();
		}
	}
	else if (name == PathLocationNetworkTableEntry)
	{
		auto pos = robot_location_.GetDoubleArray({ -1000000.0, -1000000.0, -1000000.0 });
		if (pos.at(0) > -1000.0 && pos.at(1) > -1000 && pos.at(2) > -1000)
		{
			Pose2d pose(pos.at(0), pos.at(1), Rotation2d::fromDegrees(pos.at(2)));
			path_track_.push_back(pose);
			update();
		}
	}
}


void PathDisplayWidget::paintEvent(QPaintEvent* ev)
{
	//
	// Draw the field
	//
	if (field_ != nullptr) {
		QPainter paint(this);
		doPaint(paint);
	}
}

void PathDisplayWidget::resizeEvent(QResizeEvent* event)
{
	if (field_ != nullptr)
		createTransforms();
}

void PathDisplayWidget::contextMenuEvent(QContextMenuEvent* ev)
{
	QMenu menu;

	for (auto field : mgr_.getFields())
	{
		QAction* act = menu.addAction(QString::fromStdString(field->getName()));
		auto fn = std::bind(&PathDisplayWidget::setFieldFromMenu, this, std::placeholders::_1, field);
		(void)connect(act, &QAction::triggered, fn);
	}

	menu.exec(ev->globalPos());
}

void PathDisplayWidget::setFieldFromMenu(bool checked, std::shared_ptr<GameField> field)
{
	setField(field);
}

void PathDisplayWidget::doPaint(QPainter& paint)
{
	QBrush background(QColor(64, 64, 64));
	paint.setBrush(background);
	paint.drawRect(0, 0, width(), height());

	static const int rsize = 8;
	QRectF rect(0.0f, 0.0f, field_image_.width() * image_scale_, field_image_.height() * image_scale_);
	paint.drawImage(rect, field_image_);

	QBrush b(QColor(0, 255, 0));
	paint.save();

	QPointF p1(0, 0);
	QPointF p2(rsize, 0);

	double size = window_to_world_.map(p1).x() - window_to_world_.map(p2).x();

	paint.setBrush(b);
	for (const Pose2d& rpos : robot_track_) {
		QPointF ptf = world_to_window_.map(QPointF(rpos.getTranslation().getX(), rpos.getTranslation().getY()));
		QRectF r(ptf.x() - rsize / 2, ptf.y()- rsize / 2, rsize, rsize);
		paint.drawEllipse(r);

		QTransform mm;
		mm.translate(rpos.getTranslation().getX(), rpos.getTranslation().getY());
		double ang = rpos.getRotation().toRadians();
		mm.rotateRadians(ang);
		QPointF pt1(0, 0);
		QPointF pt2(-size * 2, 0);
		QPointF ptf1 = world_to_window_.map(mm.map(pt1));
		QPointF ptf2 = world_to_window_.map(mm.map(pt2));

		paint.save();
		QPen pen(QColor(255, 0, 0));
		paint.setPen(pen);
		paint.drawLine(ptf1, ptf2);

		paint.restore();
	}

	b = QBrush(QColor(0, 0, 255));
	paint.setBrush(b);
	for (const Pose2d& rpos : path_track_) {
		QPointF ptf = world_to_window_.map(QPointF(rpos.getTranslation().getX(), rpos.getTranslation().getY()));
		QRectF r(ptf.x() - rsize / 2, ptf.y() - rsize / 2, rsize, rsize);
		paint.drawEllipse(r);
	}
	paint.restore();
}

void PathDisplayWidget::setUnits(const std::string& units)
{
	units_ = QString::fromStdString(units);
	createTransforms();
	repaint();
}

QSize PathDisplayWidget::minimumSizeHint() const
{
	return QSize(field_image_.size().width() / 4, field_image_.size().height() / 4);
}

QSize PathDisplayWidget::sizeHint() const
{
	return field_image_.size();
}

void PathDisplayWidget::setField(std::shared_ptr<GameField> field)
{
	field_ = field;

	//
	// Load the image,
	//
	QImage image(field_->getImageFile());
	field_image_ = image;

	//
	// Create new transforms
	//
	createTransforms();

	//
	// Redraw the widget
	//
	repaint(geometry());
}

void PathDisplayWidget::createTransforms()
{
	//
	// First calculate the scale to use to scale the image to the window
	// without changing the aspec ratio.  Pick the largest scale factor that will fit
	// the most constrained dimension
	//
	double sx = (double)width() / (double)field_image_.width();
	double sy = (double)height() / (double)field_image_.height();

	if (sx < sy)
		image_scale_ = sx;
	else
		image_scale_ = sy;

	//
	// Now, we know now to scale the image to window pixels, scale the top left and bottom
	// right coordiantes from the image to the window
	//
	QPointF tl = image_scale_ * field_->getTopLeft();
	QPointF br = image_scale_ * field_->getBottomRight();

	//
	// Now we know two points on the field and how they coorespond to two points on the window.
	// The field point (0, FieldSize.Y) cooresponds to the point tl.
	// The field point (FieldsSize.X, 0) coorespons to the point br.
	//
	// Compute the transform based on this.  
	//
	world_to_window_ = QTransform();

	world_to_window_.translate(tl.x(), br.y());
	double scale = (br.x() - tl.x()) / field_->getSize().x();
	world_to_window_.scale(scale, -scale);
	window_to_world_ = world_to_window_.inverted();
}