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

#include "ROSHandler.h"
#include "Server.h"
#include "rec/rpc/ROSHelper.h"

ROSHandler::ROSHandler()
: _logLevel( InfoLog )
, _feedback_time( 1000 )
, _server( NULL )
, _tfListener( new tf::TransformListener )
, _node( "~" )
, _move_base_client(  "/move_base", false )
{
	_log_sub = _node.subscribe( "/rosout_agg", 100, &ROSHandler::logCallback, this );
	_laser_scan_sub = _node.subscribe( "/scan", 1, &ROSHandler::scanCallback, this );
	_odom_sub = _node.subscribe("/odom", 1, &ROSHandler::odomCallback, this );
	_plan_sub = _node.subscribe( "/move_base/NavfnROS/plan", 1, &ROSHandler::planCallback, this );
	_current_goal_sub = _node.subscribe("/move_base/current_goal", 1, &ROSHandler::currentGoalCallback, this );
	_map_sub = _node.subscribe( "/map", 1, &ROSHandler::rosMapCallback, this );

	_posePublisherEstimate = _node.advertise<geometry_msgs::PoseWithCovarianceStamped>("/initialpose", 1);
	_move_base_goal = _node.advertise<geometry_msgs::PoseStamped>( "/move_base_simple/goal", 1 );
	_cmd_vel_pub = _node.advertise<geometry_msgs::Twist>( "/cmd_vel", 1 );
}

void ROSHandler::setServer( Server* server )
{
	_server = server;
}

void ROSHandler::start()
{
	log( "Start" );
	QTimer* spinTimer = new QTimer( this );
	spinTimer->setSingleShot( false );
	spinTimer->setInterval( 20 );

	bool connected = true;
	
	connected &= connect( spinTimer, SIGNAL( timeout() ), SLOT( on_spinTimer_timeout() ) );

	Q_ASSERT( connected );

	spinTimer->start();
}

void ROSHandler::setCmdVel( float vx, float vy, float omega )
{
	geometry_msgs::Twist twist;
	twist.linear.x = vx;
	twist.angular.z = omega;
	_cmd_vel_pub.publish( twist );
}

void ROSHandler::setPoseEstimate( const QPointF& pos, float rotDeg )
{
	geometry_msgs::Pose pose = rec::rpc::toPose( _mapInfo, pos, rotDeg );

	geometry_msgs::PoseWithCovarianceStamped msg;
	msg.header.stamp = ros::Time( 0 );
	msg.header.frame_id = "/map";
	msg.pose.pose = pose;

	_posePublisherEstimate.publish( msg );
}

void ROSHandler::setGoal( const QPointF& pos, float rotDeg )
{
	geometry_msgs::Pose pose = rec::rpc::toPose( _mapInfo, pos, rotDeg );

	//geometry_msgs::PoseStamped msg;
	//msg.header.stamp = ros::Time( 0 );
	//msg.header.frame_id = "/map";
	//msg.pose = pose;

	//_move_base_goal.publish( msg );

	move_base_msgs::MoveBaseGoal goal;
	goal.target_pose.header.stamp = ros::Time( 0 );
	goal.target_pose.header.frame_id = "/map";
	goal.target_pose.pose = pose;

	_move_base_client.sendGoal(
		goal,
		boost::bind( &ROSHandler::move_base_client_done_callback, this, _1, _2 ),
		boost::bind( &ROSHandler::move_base_client_active_callback, this ),
		boost::bind( &ROSHandler::move_base_client_feedback_callback, this, _1 ) );
}

void ROSHandler::on_spinTimer_timeout()
{
	ros::spinOnce();
}

void ROSHandler::log( const QString& message, LogType type )
{
	switch( _logLevel )
	{
	case DebugLog:
		if( DebugLog == type )
		{
			qDebug() << "Debug: " << message;
		}

	case InfoLog:
		if( InfoLog == type )
		{
			qDebug() << "Info: " << message;
		}
		else if( ErrorLog == type )
		{
			qDebug() << "Error: " << message;
		}
		break;

	default:
		break;
	}
}

void ROSHandler::logCallback( const roslib::Log::ConstPtr& msg )
{
	if( NULL == _server ) { log( "Server not ready" ); return; }
	QString str = QString( "%1 %2" ).arg( msg->name.c_str() ).arg( msg->msg.c_str() );
	_server->publishLog( str );
	qDebug() << str;
	//recQtLog.postLog( str );
}

void ROSHandler::rosMapCallback( const nav_msgs::OccupancyGridConstPtr& msg )
{
	log( "rosMapCallback" );
	if( NULL == _server ) { log( "Server not ready" ); return; }

	_mapInfo = rec::rpc::MapInfo( msg );

	QImage image = rec::rpc::fromOccupancyGrid( msg );

	_server->publishMap( image, _mapInfo.resolution, _mapInfo.offset[0], _mapInfo.offset[1] );
}

void ROSHandler::scanCallback( const sensor_msgs::LaserScan& scanMsg )
{
	log( "Scan", DebugLog );

	if( _mapInfo.isEmpty() )
	{
		log( "No map item" );
		return;
	}

	if( NULL == _server ) { log( "Server not ready" ); return; }

	sensor_msgs::PointCloud pointCloud;
	pointCloud.header.frame_id = scanMsg.header.frame_id;
	pointCloud.header.stamp = scanMsg.header.stamp;

	pointCloud.points.resize( scanMsg.ranges.size() );

	for( unsigned int scan=0; scan < scanMsg.ranges.size(); ++scan )
	{
		float scan_angle = scanMsg.angle_min + scanMsg.angle_increment * scan;
		const float& distance = scanMsg.ranges[scan];

		geometry_msgs::Point32 p;
		p.x = cos( scan_angle ) * distance;
		p.y = sin( scan_angle ) * distance;
		p.z = 0;

		pointCloud.points[scan] = p;
	}

	QVector< QPointF > points = rec::rpc::getPoints( _tfListener, _mapInfo, pointCloud );

	_server->publishScan( points );
}

void ROSHandler::odomCallback( const nav_msgs::OdometryConstPtr& msg )
{
	log( "odom", DebugLog );

	if( _mapInfo.isEmpty() )
	{
		log( "No map item" );
		return;
	}

	if( NULL == _server ) { log( "Server not ready" ); return; }

	QPointF point;
	double yaw;

	if( rec::rpc::poseToMap( _tfListener, _mapInfo, msg->pose.pose, msg->header.frame_id, ros::Time( 0 ), &point, &yaw ) )
	{
		_server->publishOdom( point, yaw );
	}
}

void ROSHandler::planCallback( const nav_msgs::Path& msg )
{
	if( _mapInfo.isEmpty() )
	{
		log( "No map item" );
		return;
	}

	if( NULL == _server ) { log( "Server not ready" ); return; }

	QPolygonF poly;
	for( unsigned int i = 0; i < msg.poses.size(); ++i )
	{
		poly.append( rec::rpc::getPoint( _mapInfo, msg.poses[i].pose.position ) );

	}

	_server->publishPlan( poly );
}

void ROSHandler::currentGoalCallback( const geometry_msgs::PoseStampedConstPtr& msg )
{
	if( _mapInfo.isEmpty() )
	{
		log( "No map item" );
		return;
	}
	if( NULL == _server ) { log( "Server not ready" ); return; }

	QPointF point;
	double yaw;

	if( rec::rpc::poseToMap( _tfListener, _mapInfo, msg->pose, msg->header.frame_id, ros::Time( 0 ), &point, &yaw ) )
	{
		_server->publishCurrentGoal( point, yaw );
	}
}

void ROSHandler::move_base_client_active_callback()
{
	qDebug() << "move_base_client_active_callback";
	_server->publishMoveBaseFeedback( 0, "activated" );
}

void ROSHandler::move_base_client_done_callback( const actionlib::SimpleClientGoalState& state, const move_base_msgs::MoveBaseResultConstPtr& result )
{
	qDebug() << "move_base " << ( actionlib::SimpleClientGoalState::SUCCEEDED == state.state_ ? "finished" : "aborted" );
	_server->publishMoveBaseFeedback( 0, ( actionlib::SimpleClientGoalState::SUCCEEDED == state.state_ ? "finished" : "aborted" ) );
}

void ROSHandler::move_base_client_feedback_callback( const move_base_msgs::MoveBaseFeedbackConstPtr& feedback )
{
	qDebug() << "move_base_client_feedback_callback";

	_server->publishMoveBaseFeedback( 0, "feedback" );
}