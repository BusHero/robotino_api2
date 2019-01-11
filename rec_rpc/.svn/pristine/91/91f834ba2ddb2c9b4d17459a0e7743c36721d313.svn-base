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

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <QtGui>

#include "serialization/LogMsg.h"
#include "serialization/OdomMsg.h"
#include "serialization/MapMsg.h"
#include "serialization/CmdVelMsg.h"
#include "serialization/ScanMsg.h"
#include "serialization/PlanMsg.h"
#include "serialization/CurrentGoalMsg.h"
#include "serialization/MoveBaseActionFeedbackMsg.h"

#include "serialization/SetPoseEstimateMsg.h"
#include "serialization/SetGoalMsg.h"

#include "rec/rpc/Client.h"

class Client : public rec::rpc::Client
{
	Q_OBJECT
public:
	static Client* singleton()
	{
		return _impl;
	}

	static void init()
	{
		_impl = new Client;
	}

	static void done()
	{
		delete _impl;
		_impl = NULL;
	}


	DECLARE_NOTIFIER( setPoseEstimate );
	DECLARE_NOTIFIER( setGoal );

	DECLARE_TOPICLISTENER( log )
	DECLARE_TOPICLISTENER( odom )
	DECLARE_TOPICLISTENER( map )
	DECLARE_TOPICLISTENER( scan )
	DECLARE_TOPICLISTENER( plan )
	DECLARE_TOPICLISTENER( currentGoal )
	DECLARE_TOPICLISTENER( moveBaseActionFeedback )

public Q_SLOTS:
	bool setCmdVel( float x, float y, float omega );
	
	void setPoseEstimate( const QPointF& pos, qreal rotation );

	void setGoal( const QPointF& pos, qreal rotation );

Q_SIGNALS:
	void is_connected();
	void is_disconnected();

	void logCb( const QString& );
	void mapCb( const QImage&, float, float, float );
	void scanCb( const QVector< QPointF >& );
	void odomCb( const QPointF&, float );
	void planCb( const QPolygonF& );
	void currentGoalCb( const QPointF&, double );

	void moveBaseActivatedCb();
	void moveBaseDoneCb( bool ok );

private:
	Client();
	~Client();

	static Client* _impl;

	void setTopicsEnabled( bool enabled );

private Q_SLOTS:
	void on_connected();
	void on_disconnected( rec::rpc::ErrorCode error );
	void on_stateChanged( QAbstractSocket::SocketState state );
	void on_error( QAbstractSocket::SocketError socketError, const QString& errorString );
};

#endif //_CLIENT_H_
