/*
Copyright (c) 2011, REC Robotics Equipment Corporation GmbH, Planegg, Germany
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
- Neither the name of the REC Robotics Equipment Corporation GmbH nor the names of
  its contributors may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _ROSHANDLER_H_
#define _ROSHANDLER_H_

#include "rec/rpc/ROSHelper.h"

#include <ros/ros.h>
#include <actionlib/client/simple_action_client.h>

#include <move_base_msgs/MoveBaseAction.h>

#include <std_msgs/Bool.h>
#include <roslib/Log.h>
#include <tf/transform_listener.h>

#include <nav_msgs/OccupancyGrid.h>
#include <nav_msgs/Odometry.h>
#include <nav_msgs/Path.h>
#include <sensor_msgs/LaserScan.h>

#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Twist.h>

#include <QtCore>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

class Server;

class ROSHandler : public QObject
{
	Q_OBJECT
public:
	ROSHandler();

	void setServer( Server* server );

	void start();

	void setCmdVel( float vx, float vy, float omega );
	void setPoseEstimate( const QPointF& pos, float rotDeg );
	void setGoal( const QPointF& pos, float rotDeg );

private Q_SLOTS:
	void on_spinTimer_timeout();

private:
	typedef enum { InfoLog, DebugLog, ErrorLog } LogType;

	void log( const QString& message, LogType type = InfoLog );

	void logCallback( const roslib::Log::ConstPtr& msg );
	void rosMapCallback( const nav_msgs::OccupancyGridConstPtr& msg );
	void scanCallback( const sensor_msgs::LaserScan& scanMsg );
	void odomCallback( const nav_msgs::OdometryConstPtr& msg );
	void planCallback( const nav_msgs::Path& msg );
	void currentGoalCallback( const geometry_msgs::PoseStampedConstPtr& msg );
	void move_base_client_active_callback();
	void move_base_client_done_callback( const actionlib::SimpleClientGoalState& state, const move_base_msgs::MoveBaseResultConstPtr& result );
	void move_base_client_feedback_callback( const move_base_msgs::MoveBaseFeedbackConstPtr& feedback );

	LogType _logLevel;
	int _feedback_time;

	Server* _server;
	tf::TransformListener* _tfListener;
	rec::rpc::MapInfo _mapInfo;

	ros::NodeHandle _node;

	MoveBaseClient _move_base_client;

	ros::Subscriber _log_sub;
	ros::Subscriber _laser_scan_sub;
	ros::Subscriber _odom_sub;
	ros::Subscriber _plan_sub;
	ros::Subscriber _current_goal_sub;
	ros::Subscriber _map_sub;

	ros::Publisher _posePublisherEstimate;
	ros::Publisher _move_base_goal;
	ros::Publisher _cmd_vel_pub;
};

#endif //_ROSHANDLER_H_

