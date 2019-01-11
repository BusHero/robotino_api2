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

#include <QtCore>

#include "Server.h"

class Handler : public QObject
{
	Q_OBJECT
public:
	Handler( int port, bool localIPCEnabled, bool multiThreadedSerialization );
	~Handler();

	static QTextStream out;
	static QTextStream err;

private:
	Server _server;

	void parseCmdLine();

private Q_SLOTS:
	void on_server_listening();
	void on_server_closed();
	void on_server_finished();
	void on_server_clientConnected( const rec::rpc::ClientInfo& info );
	void on_server_clientDisconnected( const rec::rpc::ClientInfo& info );
	void on_server_serverError( QAbstractSocket::SocketError error, const QString& errorString );
	void on_server_clientError( QAbstractSocket::SocketError error, const QString& errorString );
	void on_server_registeredTopicListener( const QString& name, const rec::rpc::ClientInfo& info );
	void on_server_unregisteredTopicListener( const QString& name, const rec::rpc::ClientInfo& info );
};
