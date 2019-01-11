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

#ifndef _REC_RPC_SERVER_SERVERTHREAD_H_
#define _REC_RPC_SERVER_SERVERTHREAD_H_

#include "rec/rpc/rec_rpc_ClientInfo.h"
#include "rec/rpc/server/rec_rpc_server_Socket.hpp"

#include <QtCore>
#include <QtNetwork>

namespace rec
{
	namespace rpc
	{
		namespace server
		{
			class Config;

			class ServerThread : public QThread
			{
				Q_OBJECT
			public:
				ServerThread( QObject* parent, QTcpSocket* tcpSocket, const Config& config );
				ServerThread( QObject* parent, QLocalSocket* localSocket, const Config& config );

				~ServerThread();

				void start();

				void exit();

				void sendGreeting( const QByteArray& greetingMessage );
				void sendConfiguration( const QByteArray& data );
				void sendRPCResponse( const QByteArray& data );
				void publishTopic( unsigned int id, const QByteArray& metaData, const QByteArray& serSource, const QByteArray& payload, bool enqueue );
				void sendHTTPResponse( const QString& header, const QByteArray& content );
				void sendCustomRequestResponse( const QByteArray& data );

				bool isLocalConnection() const;

				bool isConnected() const;

				rec::rpc::ClientInfo clientInfo() const { return _clientInfo; }

				QHostAddress peerAddress() const { return _clientInfo.address; }
				quint16 peerPort() const { return _clientInfo.port; }

				rec::rpc::server::Socket::Protocol protocol() const { return _socket->protocol(); }

			Q_SIGNALS:
				void rpcRequestReceived( rec::rpc::server::ServerThread*, const QString&, quint32, const QByteArray&, const rec::rpc::ClientInfo& );
				void topicReceived( unsigned int, const rec::rpc::ClientInfo&,  const QByteArray&, const QByteArray& );

				void error( QAbstractSocket::SocketError, const QString& );

				void connected( const rec::rpc::ClientInfo& );
				void disconnected( const rec::rpc::ClientInfo&, const QList< unsigned int >& );

				void channelRegistered( rec::rpc::server::ServerThread*, unsigned int );
				void channelUnregistered( rec::rpc::server::ServerThread*, unsigned int );

				void rpcClientReady( rec::rpc::server::ServerThread* );

				void httpGetRequestReceived( rec::rpc::server::ServerThread*, const QString&, const QString&, const rec::rpc::ClientInfo& );

				void customRequestReceived( rec::rpc::server::ServerThread*, const QByteArray&, const rec::rpc::ClientInfo& );

				void log( const QString&, int );

			private Q_SLOTS:
				void on_disconnected();
				void on_error( QAbstractSocket::SocketError );
				void on_rpcRequestReceived( const QString& name, quint32 seqNum, const QByteArray& serParam );
				void on_topicReceived( unsigned int id, const QByteArray& serData );
				void on_registerChannelReceived( unsigned int id );
				void on_unregisterChannelReceived( unsigned int id );

				void on_protocolReceived( rec::rpc::server::Socket::Protocol protocol );

				void on_httpGetRequestReceived( const QString& request, const QString& host );
				void on_customRequestReceived( const QByteArray& request );

			private:
				void run();

				QTcpSocket* _tcpSocket;
				QLocalSocket* _localSocket;

				QMutex _socketMutex;
				rec::rpc::server::Socket* _socket;

				QSemaphore _startSemaphore;

				mutable QMutex _clientInfoMutex;
				rec::rpc::ClientInfo _clientInfo;
				QByteArray _serClientInfo;

				mutable QMutex _registeredChannelsMutex;
				QSet< unsigned int > _registeredChannels;

				QTime _requestTimer;
				int _minRequestDelay;

				const Config& _config;

				bool _isDisconnected;
			};
		}
	}
}

#endif //_REC_RPC_SERVER_SERVERTHREAD_H_
