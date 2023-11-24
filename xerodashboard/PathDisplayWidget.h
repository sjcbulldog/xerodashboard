#pragma once

#include <QtCore/QList>
#include <QtWidgets/QWidget>
#include <networktables/NetworkTableEntry.h>
#include "Pose2d.h"

class GameFieldManager;
class GameField;
class NetworkTableManager;

class PathDisplayWidget : public QWidget
{
	Q_OBJECT

public:
	PathDisplayWidget(GameFieldManager &mgr, std::shared_ptr<NetworkTableManager> ntmgr, QWidget *parent = Q_NULLPTR);
	~PathDisplayWidget();

	void setUnits(const std::string& units);
	void setField(std::shared_ptr<GameField> field);

protected:
	void paintEvent(QPaintEvent* ev) override;
	void resizeEvent(QResizeEvent* ev) override;
	void contextMenuEvent(QContextMenuEvent* ev) override;

	QSize minimumSizeHint() const override;
	QSize sizeHint() const override;

private:
	void doPaint(QPainter& p);
	void createTransforms();
	void setFieldFromMenu(bool triggered, std::shared_ptr<GameField> field);

	void newDetected(const nt::TopicInfo& info);
	void entryUpdateDetected(const nt::ValueEventData& path);
	void disconnectDetected();
	void connectDetected();

	static constexpr const char* RobotLocationNetworkTableEntry = "/XeroLocation/Robot";
	static constexpr const char* PathLocationNetworkTableEntry = "/XeroLocation/Path";

private:
	GameFieldManager& mgr_;
	std::shared_ptr<NetworkTableManager> ntmgr_;
	nt::NetworkTableEntry robot_location_;
	nt::NetworkTableEntry path_location_;
	std::shared_ptr<GameField> field_; 
	QImage field_image_;
	QString units_;
	double image_scale_;
	QTransform world_to_window_;
	QTransform window_to_world_;
	bool connected_;

	QList<Pose2d> robot_track_;
	QList<Pose2d> path_track_;

	QMetaObject::Connection update_connection_;
	QMetaObject::Connection new_connection_;
	QMetaObject::Connection disconnect_connection_;
	QMetaObject::Connection connect_connection_;

	QMap<NT_Topic, QString> topics_;
};
