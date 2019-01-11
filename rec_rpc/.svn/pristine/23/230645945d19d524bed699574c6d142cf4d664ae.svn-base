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

#include "Server.h"
#include "ROSHandler.h"

#include "rec/rpc/Exception.h"

Server::Server( ROSHandler* rosHandler )
: _rosHandler( rosHandler )
{
	// Register the RPC functions to allow clients to invoke them.
	REGISTER_FUNCTION( setPoseEstimate );
	REGISTER_FUNCTION( setGoal );

	ADD_TOPIC( log, 0 );
	ADD_TOPIC( odom, 0 );
	ADD_TOPIC( map, 0 );
	ADD_TOPIC( scan, 0 );
	ADD_TOPIC( plan, 0 );
	ADD_TOPIC( currentGoal, 0 );
	ADD_TOPIC( cmd_vel, 0 );
	ADD_TOPIC( moveBaseActionFeedback, 0 );
	
	setGreeting( "REC RPC run by rec_rpc_server_node" );

	setTopicsEnabled( true );

	log( "Server" );

	_rosHandler->setServer( this );

	bool success = true;

	success &= connect( this, SIGNAL( listening() ), SLOT( on_listening() ) );
	success &= connect( this, SIGNAL( serverError( QAbstractSocket::SocketError, const QString& ) ), SLOT( on_serverError( QAbstractSocket::SocketError, const QString& ) ) );

	Q_ASSERT( success );
}

void Server::publishLog( const QString& str )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( log, str );
	}
	catch( rec::rpc::Exception& e )
	{
		log( e.what() );
	}
}

void Server::publishOdom( const QPointF& point, float phi )
{
	try
	{
		PREPARE_TOPIC( odom );
		data.point() = point;
		data.phi() = phi;
		PUBLISH_TOPIC;
	}
	catch( rec::rpc::Exception& e )
	{
		log( e.what() );
	}
}

void Server::publishMap( const QImage& image, float resolution, float offsetx, float offsety )
{
	log( QString("%1 %2 %3").arg( resolution ).arg( offsetx ).arg( offsety ) );
	try
	{
		PREPARE_TOPIC( map );
		data.map() = image;
		data.resolution() = resolution;
		data.offsetx() = offsetx;
		data.offsety() = offsety;
		PUBLISH_TOPIC;
	}
	catch( rec::rpc::Exception& e )
	{
		log( e.what() );
	}
}

void Server::publishScan( const QVector< QPointF >& points )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( scan, points );
	}
	catch( rec::rpc::Exception& e )
	{
		log( e.what() );
	}
}

void Server::publishPlan( const QPolygonF& poly )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( plan, poly );
	}
	catch( rec::rpc::Exception& e )
	{
		log( e.what() );
	}
}

void Server::publishCurrentGoal( const QPointF& point, float phi )
{
	try
	{
		PREPARE_TOPIC( currentGoal );
		data.point() = point;
		data.phi() = phi;
		PUBLISH_TOPIC;
	}
	catch( rec::rpc::Exception& e )
	{
		log( e.what() );
	}
}

void Server::publishMoveBaseFeedback( unsigned int sequence, const QString& message )
{
	try
	{
		PREPARE_TOPIC( moveBaseActionFeedback );
		data.sequence() = sequence;
		data.feedback() = message;
		PUBLISH_TOPIC;
	}
	catch( rec::rpc::Exception& e )
	{
		log( e.what() );
	}
}

void Server::setTopicsEnabled( bool enabled )
{	// Register or unregister topic listener and topic info listener for topic "image".
	if ( enabled )
	{
		//REGISTER_TOPICINFOCHANGED( image );
		REGISTER_TOPICLISTENER( cmd_vel );
	}
	else
	{
		//UNREGISTER_TOPICINFOCHANGED( image );
		UNREGISTER_TOPICLISTENER( cmd_vel );
	}
}

void Server::on_listening()
{
	log( QString( "Listening on port %1" ).arg( serverPort() ) );
}

void Server::on_serverError( QAbstractSocket::SocketError error, const QString& errorString )
{
	log( errorString );
	qApp->quit();
}

void Server::log( const QString& str ) const
{
	qDebug() << str;
}

// Definition of the rpc functions. Parameters are accessible via "param", return values via "result".

BEGIN_FUNCTION_DEFINITION( Server, setPoseEstimate )
log( QString( "setPoseEstimate received from %1:%2 x:%3 y:%4 rot:%5" ).arg( address.toString() ).arg( port ).arg( param.pos.x() ).arg( param.pos.y() ).arg( param.rotDeg ) );
_rosHandler->setPoseEstimate( param.pos, param.rotDeg );
END_FUNCTION_DEFINITION

BEGIN_FUNCTION_DEFINITION( Server, setGoal )
log( QString( "setGoal received from %1:%2" ).arg( address.toString() ).arg( port ) );
_rosHandler->setGoal( param.pos, param.rotDeg );
END_FUNCTION_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, cmd_vel )
log( QString( "cmd_vel received from %1:%2" ).arg( address.toString() ).arg( port ) );
_rosHandler->setCmdVel( data.vx(), data.vy(), data.omega() );
END_TOPICLISTENER_DEFINITION

