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

#include "Client.h"

#include <iostream>

Q_DECLARE_METATYPE( QVector< QPointF > )
Q_DECLARE_METATYPE( QVector< double > )
Q_DECLARE_METATYPE( QPolygonF )
Q_DECLARE_METATYPE( QVector< QPolygonF > )
Q_DECLARE_METATYPE( QVector< bool > )
Q_DECLARE_METATYPE( QVector< unsigned int > )
Q_DECLARE_METATYPE( QVector< int > )
Q_DECLARE_METATYPE( QVector< QString > )

Client* Client::_impl = NULL;

Client::Client()
{
	REGISTER_NOTIFIER( setPoseEstimate );
	REGISTER_NOTIFIER( setGoal );

	qRegisterMetaType< QVector< QPointF > >();
	qRegisterMetaType< QVector< double > >();
	qRegisterMetaType< QVector< QPolygonF > >();
	qRegisterMetaType< QVector< bool > >();
	qRegisterMetaType< QVector< unsigned int > >();
	qRegisterMetaType< QVector< int > >();
	qRegisterMetaType< QVector< QString > >();
	qRegisterMetaType< QPolygonF >();

	setTopicsEnabled( true );
	setExpectedGreeting( "REC RPC run by rec_rpc_server_node" );


	bool ok = true;

	ok &= connect( this, SIGNAL( connected() ), SLOT( on_connected() ) );
	ok &= connect( this, SIGNAL( disconnected( rec::rpc::ErrorCode ) ), SLOT( on_disconnected( rec::rpc::ErrorCode ) ) );
	ok &= connect( this, SIGNAL( stateChanged( QAbstractSocket::SocketState ) ), SLOT( on_stateChanged( QAbstractSocket::SocketState ) ) );
	ok &= connect( this, SIGNAL( error( QAbstractSocket::SocketError, const QString& ) ), SLOT( on_error( QAbstractSocket::SocketError, const QString& ) ) );

	Q_ASSERT( ok );

	setAutoReconnectEnabled( true );
}

Client::~Client()
{
}

void Client::setTopicsEnabled( bool enabled )
{
	if ( enabled )
	{
		try
		{
			REGISTER_TOPICLISTENER( log );
			REGISTER_TOPICLISTENER( odom );
			REGISTER_TOPICLISTENER( map );
			REGISTER_TOPICLISTENER( scan );
			REGISTER_TOPICLISTENER( plan );
			REGISTER_TOPICLISTENER( currentGoal );
			REGISTER_TOPICLISTENER( moveBaseActionFeedback );
		}
		catch( const rec::rpc::Exception& e )
		{
			Q_EMIT logCb( QString( "Failed to register topic listeners: " ) + e.getMessage() );
		}
	}
	else
	{
		try
		{
			UNREGISTER_TOPICLISTENER( log );
			UNREGISTER_TOPICLISTENER( odom );
			UNREGISTER_TOPICLISTENER( map );
			UNREGISTER_TOPICLISTENER( scan );
			UNREGISTER_TOPICLISTENER( plan );
			UNREGISTER_TOPICLISTENER( currentGoal );
			UNREGISTER_TOPICLISTENER( moveBaseActionFeedback );
		}
		catch( const rec::rpc::Exception& e )
		{
			Q_EMIT logCb( QString( "Failed to unregister topic listeners: " ) + e.getMessage() );
		}
	}
}

void Client::on_connected()
{
	Q_EMIT logCb( "Connected" );
	Q_EMIT is_connected();
}

void Client::on_disconnected( rec::rpc::ErrorCode error )
{
	Q_EMIT logCb( "Disconnected" );
	Q_EMIT is_disconnected();
}

void Client::on_stateChanged( QAbstractSocket::SocketState state )
{
}

void Client::on_error( QAbstractSocket::SocketError socketError, const QString& errorString )
{
	Q_EMIT logCb( errorString );
}

bool Client::setCmdVel( float x, float y, float omega )
{
	try
	{
		PREPARE_TOPIC( cmd_vel );
		data.vx() = x;
		data.vy() = y;
		data.omega() = omega;
		PUBLISH_TOPIC;
	}
	catch( rec::rpc::Exception& e )
	{
		Q_EMIT logCb( QString( "Error setCmdVel: " ) + e.what() );
		return false;
	}
	return true;
}

void Client::setPoseEstimate( const QPointF& pos, qreal rotation )
{
	Q_EMIT logCb( QString( "Pose estimate: %1 %2 %3" ).arg( pos.x() ).arg( pos.y() ).arg( rotation ) );
	try
	{
		PREPARE( setPoseEstimate );
		param.pos = pos;
		param.rotDeg = rotation; 

		INVOKE( false );
	}
	catch( rec::rpc::Exception& e )
	{
		Q_EMIT logCb( QString( "Error setPoseEstimate: %1" ).arg( e.what() ) );
	}
}

BEGIN_NOTIFIER( Client, setPoseEstimate )
	Q_EMIT logCb( QString( "setPoseEstimate: %1" ).arg( rec::rpc::Exception::messageFromErrorCode( errorCode ) ) );
END_NOTIFIER

void Client::setGoal( const QPointF& pos, qreal rotation )
{
	try
	{
		PREPARE( setGoal );
		param.pos = pos;
		param.rotDeg = rotation;

		INVOKE( false );
	}
	catch( rec::rpc::Exception& e )
	{
		Q_EMIT logCb( QString( "Error setGoal: %1" ).arg( e.what() ) );
	}
}

BEGIN_NOTIFIER( Client, setGoal )
	Q_EMIT logCb( QString( "setGoal: %1" ).arg( rec::rpc::Exception::messageFromErrorCode( errorCode ) ) );
END_NOTIFIER

BEGIN_TOPICLISTENER_DEFINITION( Client, log )
	Q_EMIT logCb( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, odom )
	Q_EMIT odomCb( data.point(), data.phi() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, map )
	Q_EMIT mapCb( data.map(), data.resolution(), data.offsetx(), data.offsety() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, scan )
	Q_EMIT scanCb( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, plan )
	Q_EMIT planCb( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, currentGoal )
	Q_EMIT currentGoalCb( data.point(), data.phi() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, moveBaseActionFeedback )
	Q_EMIT logCb( QString( "moveBaseActionFeedback: %1 %2" ).arg( data.sequence() ).arg( data.feedback() ) );
	if( data.feedback() == "activated" )
	{
		Q_EMIT moveBaseActivatedCb();
	}
	else if( data.feedback() == "finished" )
	{
		Q_EMIT moveBaseDoneCb( true );
	}
	else if( data.feedback() == "aborted" )
	{
		Q_EMIT moveBaseDoneCb( false );
	}
END_TOPICLISTENER_DEFINITION
