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

#ifndef _REC_RPC_SERVER_IMPL_H_
#define _REC_RPC_SERVER_IMPL_H_

#include "rec/rpc/defines.h"
#include "rec/rpc/rec_rpc_ClientInfo.h"
#include "rec/rpc/configuration/rec_rpc_configuration_Configuration.hpp"

#include "rec/rpc/server/rec_rpc_server_Config.hpp"

#include <QtCore>
#include <QtNetwork>

namespace rec
{
	namespace rpc
	{
		namespace server
		{
			class ServerThread;

			class Impl : public QObject
			{
				Q_OBJECT
			public:
				enum Event
				{
					ListenEventId = QEvent::User,
					CloseEventId,
					DisconnectAllClientsId,
					SendDataEventId,
					DisconnectClientEventId,
					PublishTopicEventId,
					SendRpcResponseEventId,
					SendHttpGetResponseEventId,
					SendCustomRequestResponseEventId,
				};

				Impl( rec::rpc::configuration::Configuration* configuration, const QString& greeting );
				
				~Impl();

				int numClientsConnected() const;

				void sendRPCResponse( const QByteArray& data, ServerThread* receiver );
				void publishTopic( unsigned int id, const QByteArray& metaData, const QByteArray& source, const QByteArray& payload, bool enqueue );
				void sendHTTPResponse( const QString& header, const QByteArray& content, ServerThread* receiver );
				void sendCustomRequestResponse( const QByteArray& data, ServerThread* receiver );

				void setGreeting( const QString& greeting );

				QHostAddress serverAddress() const;
				quint16 serverPort() const;
				bool isListening() const;

				void disconnectAllClients();

			Q_SIGNALS:
				void clientConnected( const rec::rpc::ClientInfo& );
				void clientDisconnected( const rec::rpc::ClientInfo&, const QList< unsigned int >& );
				void serverError( QAbstractSocket::SocketError error, const QString& errorString );
				void clientError( QAbstractSocket::SocketError error, const QString& errorString );
				void rpcRequestReceived( const QString&, quint32, const QByteArray&, quintptr, const rec::rpc::ClientInfo& );
				void topicReceived( unsigned int, const rec::rpc::ClientInfo&, const QByteArray&, const QByteArray& );
				void channelRegistered( unsigned int, const rec::rpc::ClientInfo& );
				void channelUnregistered( unsigned int, const rec::rpc::ClientInfo& );
				void listening();
				void closed();
				void numClientsConnectedChanged( int );
				void log( const QString& message, int level );

				void httpGetRequestReceived( const QString&, const QString&, quintptr, const rec::rpc::ClientInfo& );
				void customRequestReceived( const QByteArray&, quintptr, const rec::rpc::ClientInfo& );

			private Q_SLOTS:
				void on_rpcRequestReceived( rec::rpc::server::ServerThread* receiver, const QString& name, quint32 seqNum, const QByteArray& serParam, const rec::rpc::ClientInfo& client );
				void on_topicReceived( unsigned int id, const rec::rpc::ClientInfo& source, const QByteArray& serSource, const QByteArray& serData );
				void on_configuration_changed();
				void on_connected( const rec::rpc::ClientInfo& );
				void on_disconnected( const rec::rpc::ClientInfo&, const QList< unsigned int >& );
				void on_client_error( QAbstractSocket::SocketError error, const QString& errorString );
				void on_channelRegistered( rec::rpc::server::ServerThread* thread, unsigned int id );
				void on_channelUnregistered( rec::rpc::server::ServerThread* thread, unsigned int id );
				void on_tcpServer_newConnection();
				void on_localServer_newConnection();

				void on_rpcClientReady( rec::rpc::server::ServerThread* thread );

				void on_httpGetRequestReceived( rec::rpc::server::ServerThread* thread, const QString& request, const QString& host, const rec::rpc::ClientInfo& client );
				void on_customRequestReceived( rec::rpc::server::ServerThread* thread, const QByteArray& request, const rec::rpc::ClientInfo& client );

				void on_serverThread_finished();

			private:
				void customEvent( QEvent* e );

				void sendConfiguration( const QByteArray& data );

				void initThread( ServerThread* thread );

				QTcpServer* _tcpServer;
				QLocalServer* _localServer;

				rec::rpc::configuration::Configuration* _configuration;

				QMutex _configurationMessageMutex;
				QByteArray _configurationMessage;

				QMutex _greetingMessageMutex;
				QByteArray _greetingMessage;

				mutable QMutex _serverThreadsMutex;
				QList< ServerThread* > _serverThreads;

				Config _config;
			};

			class ListenEvent : public QEvent
			{
			public:
				ListenEvent( int port_, bool localIPCEnabled_, int clientMsgWaitTime_, int httpKeepAliveTimeout_, int httpKeepAliveMaxRequests_, int customTimeout_ )
					: QEvent( (QEvent::Type)Impl::ListenEventId )
					, port( port_ )
					, localIPCEnabled( localIPCEnabled_ )
					, config( clientMsgWaitTime_, httpKeepAliveTimeout_, httpKeepAliveMaxRequests_, customTimeout_ )
				{
				}

				int port;
				bool localIPCEnabled;
				Config config;
			};

			class CloseEvent : public QEvent
			{
			public:
				CloseEvent()
					: QEvent( (QEvent::Type)Impl::CloseEventId )
				{
				}
			};

			class DisconnectAllClients : public QEvent
			{
			public:
				DisconnectAllClients()
					: QEvent( (QEvent::Type)Impl::DisconnectAllClientsId )
				{
				}
			};

			class DisconnectClientEvent : public QEvent
			{
			public:
				DisconnectClientEvent( const QHostAddress& peerAddress_, quint16 peerPort_ )
					: QEvent( (QEvent::Type)Impl::DisconnectClientEventId )
					, peerAddress( peerAddress_ )
					, peerPort( peerPort_ )
				{
				}

				QHostAddress peerAddress;
				quint16 peerPort;
			};
		}
	}
}

#endif //_REC_RPC_SERVER_SERVER_H_
