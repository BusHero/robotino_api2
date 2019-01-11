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

#ifndef _REC_RPC_SERVER_SERVER_H_
#define _REC_RPC_SERVER_SERVER_H_

#include "rec/rpc/rec_rpc_Server.h"
#include "rec/rpc/configuration/rec_rpc_configuration_Configuration.hpp"

#include <QtCore>
#include <QtNetwork>

namespace rec
{
	namespace rpc
	{
		namespace serialization
		{
			class Serializer;
		}

		namespace server
		{
			class Impl;

			class Server : public QThread
			{
				Q_OBJECT
			public:
				Server( QObject* parent = NULL );
				
				~Server();

				bool isMultiThreadedSerializationEnabled() const { return _multiThreadedSerialization; }
				void setMultiThreadedSerializationEnabled( bool enabled );

				int port() const { return _port; }
				void setPort( int port );

				bool isLocalIPCEnabled() const { return _localIPCEnabled; }
				void setLocalIPCEnabled( bool enabled );

				int clientMsgWaitTime() const { return _clientMsgWaitTime; }
				void setClientMsgWaitTime( int clientMsgWaitTime );

				int httpKeepAliveTimeout() const { return _httpKeepAliveTimeout; }
				void setHttpKeepAliveTimeout( int httpKeepAliveTimeout );

				int httpKeepAliveMaxRequests() const { return _httpKeepAliveMaxRequests; }
				void setHttpKeepAliveMaxRequests( int httpKeepAliveMaxRequests );

				int customTimeout() const { return _customTimeout; }
				void setCustomTimeout( int customTimeout );

				bool listen( bool blocking );

				void close( bool blocking );

				bool isListening() const;

				quint16 serverPort() const;

				QString greeting() const;
				void setGreeting( const QString& greeting );

				void exit();

				void registerFunction( const QString& name, rec::rpc::RPCFunctionBasePtr function );
				void unregisterFunction( const QString& name );
				bool isFunctionRegistered( const QString& name ) const;

				void registerTopicListener( const QString& name, rec::rpc::TopicListenerBasePtr listener );
				void unregisterTopicListener( const QString& name );
				bool isTopicListenerRegistered( const QString& name ) const;

				void registerHttpGetHandler( rec::rpc::HTTPGetHandlerBasePtr handler );

				void registerCustomRequestHandler( rec::rpc::CustomRequestHandlerBasePtr handler );

				void addTopic( const QString& name, int sharedMemorySize, bool serverOnly, bool permanent, bool enqueued );

				void beginAddTopicGroup();
				void endAddTopicGroup();

				void publishTopic( const QString& name, rec::rpc::serialization::SerializablePtrConst data );

				int numClientsConnected() const;

				void disconnectAllClients();

				void disconnectClient( const QHostAddress& peerAddress, quint16 peerPort );

				rec::rpc::configuration::Configuration* configuration() { return &_configuration; }

			Q_SIGNALS:
				void clientConnected( const rec::rpc::ClientInfo& );
				void clientDisconnected( const rec::rpc::ClientInfo& );
				void serverError( QAbstractSocket::SocketError error, const QString& errorString );
				void clientError( QAbstractSocket::SocketError error, const QString& errorString );
				void registeredTopicListener( const QString&, const rec::rpc::ClientInfo& );
				void unregisteredTopicListener( const QString&, const rec::rpc::ClientInfo& );
				void closed();
				void listening();
				void numClientsConnectedChanged( int );
				void log( const QString& message, int level );

			private Q_SLOTS:
				void on_clientConnected( const rec::rpc::ClientInfo& );
				void on_clientDisconnected( const rec::rpc::ClientInfo&, const QList< unsigned int >& );
				void on_channelRegistered( unsigned int id, const rec::rpc::ClientInfo& info );
				void on_channelUnregistered( unsigned int id, const rec::rpc::ClientInfo& info );
				void on_log( const QString& message, int level );
				void on_serverError( QAbstractSocket::SocketError error, const QString& errorString );
				void on_clientError( QAbstractSocket::SocketError error, const QString& errorString );
				void on_listening();
				void on_closed();
				void on_numClientsConnectedChanged( int num );

				void on_rpcRequestReceived( const QString& name, quint32 seqNum, const QByteArray& serParam, quintptr receiver, const rec::rpc::ClientInfo& client );
				void on_topicReceived( unsigned int id, const rec::rpc::ClientInfo& source, const QByteArray& serSource, const QByteArray& serData );

				void on_httpGetRequestReceived( const QString& request, const QString& host, quintptr receiver, const rec::rpc::ClientInfo& client );
				void on_customRequestReceived( const QByteArray& request, quintptr receiver, const rec::rpc::ClientInfo& client );

				void on_RPCResponseSerialized( const QString& name, quint32 seqNum, rec::rpc::ErrorCode errorCode, const QByteArray& serResult, quintptr receiver );
				void on_topicDataSerialized( unsigned int id, const QByteArray& serData );

			private:
				struct SharedMemInfo
				{
					SharedMemInfo() : seqNum( 0 ), keySuffix( 0 ), initialSize( 0 ) { }
					unsigned int seqNum;
					unsigned int keySuffix;
					int initialSize;
					QSharedPointer< QSharedMemory > mem;
				};

				enum Event
				{
					InvokeRpcFunctionEventId = QEvent::User,
					ListenTopicEventId,
					InvokeHttpGetHandlerEventId,
					InvokeCustomRequestHandlerEventId,
				};

				static unsigned int nextInstanceId();

				void customEvent( QEvent* e );

				//static QString getInfoName( const QString& name );
				QString getMemName( const QString& name, unsigned int suffix ) const;

				void run();
				void setupServer();

				void publishTopic( unsigned int id, rec::rpc::serialization::SerializablePtrConst data );
				void publishTopicLocal( unsigned int id, const rec::rpc::ClientInfo& source, const QByteArray& serSource, const QByteArray& serData );
				void publishTopicRemote( unsigned int id, const rec::rpc::ClientInfo& source, const QByteArray& serSource, const QByteArray& serData );

				void invokeHttpGetHandler( const QUrl& url, const QString& host, quintptr receiver, const rec::rpc::ClientInfo& client );

				rec::rpc::TopicListenerBasePtr getTopicListener( unsigned int id );

				const unsigned int _instanceId;

				bool _run;

				Impl* _impl;

				bool _multiThreadedSerialization;
				rec::rpc::serialization::Serializer* _serializer;

				int _port;
				bool _localIPCEnabled;
				int _clientMsgWaitTime;
				int _httpKeepAliveTimeout;
				int _httpKeepAliveMaxRequests;
				int _customTimeout;

				QSemaphore _startSemaphore;

				mutable QMutex _mutex;
				QMutex _listenCloseMutex;
				QWaitCondition _listenCloseCondition;

				rec::rpc::configuration::Configuration _configuration;

				QMap< QString, rec::rpc::RPCFunctionBasePtr > _rpcFunctions;
				mutable QMutex _rpcFunctionsMutex;
				QMap< unsigned int, QSharedPointer< rec::rpc::TopicListenerBase > > _topicListeners;
				mutable QMutex _topicListenersMutex;

				rec::rpc::HTTPGetHandlerBasePtr _httpGetHandler;
				QMutex _httpGetHandlerMutex;
				rec::rpc::CustomRequestHandlerBasePtr _customRequestHandler;
				QMutex _customRequestHandlerMutex;

				QMap< unsigned int, SharedMemInfo > _topicLocalMemories;

				QString _greeting;

				static bool _once;

				static rec::rpc::ClientInfo _serverInfo;
				static QByteArray _serServerInfo;

				static unsigned int _nextInstanceId;
				static QMutex _nextInstanceIdMtx;

				class InvokeRpcFunctionEvent : public QEvent
				{
				public:
					InvokeRpcFunctionEvent(
						const QString& name_,
						quint32 seqNum_,
						rec::rpc::RPCFunctionBasePtr func_,
						rec::rpc::serialization::SerializablePtrConst param_,
						rec::rpc::serialization::SerializablePtr result_,
						quintptr receiver_,
						const rec::rpc::ClientInfo& client_ )
						: QEvent( (QEvent::Type) InvokeRpcFunctionEventId )
						, name( name_ )
						, seqNum( seqNum_ )
						, func( func_ )
						, param( param_ )
						, result( result_ )
						, receiver( receiver_ )
						, client( client_ )
					{
					}

					const QString name;
					quint32 seqNum;
					rec::rpc::RPCFunctionBasePtr func;
					rec::rpc::serialization::SerializablePtrConst param;
					rec::rpc::serialization::SerializablePtr result;
					quintptr receiver;
					const rec::rpc::ClientInfo client;
				};

				class ListenTopicEvent : public QEvent
				{
				public:
					ListenTopicEvent(
						rec::rpc::TopicListenerBasePtr listener_,
						rec::rpc::serialization::SerializablePtrConst data_,
						const rec::rpc::ClientInfo& client_,
						rec::rpc::ErrorCode errorCode_ )
						: QEvent( (QEvent::Type)ListenTopicEventId )
						, listener( listener_ )
						, data( data_ )
						, client( client_ )
						, errorCode( errorCode_ )
					{
					}

					rec::rpc::TopicListenerBasePtr listener;
					rec::rpc::serialization::SerializablePtrConst data;
					const rec::rpc::ClientInfo client;
					rec::rpc::ErrorCode errorCode;
				};

				class InvokeHttpGetHandlerEvent : public QEvent
				{
				public:
					InvokeHttpGetHandlerEvent( const QUrl& url_, const QString& host_, quintptr receiver_, const rec::rpc::ClientInfo& client_ )
						: QEvent( (QEvent::Type)InvokeHttpGetHandlerEventId )
						, url( url_ )
						, host( host_ )
						, receiver( receiver_ )
						, client( client_ )
					{
					}

					const QUrl url;
					const QString host;
					quintptr receiver;
					const rec::rpc::ClientInfo client;
				};

				class InvokeCustomRequestHandlerEvent : public QEvent
				{
				public:
					InvokeCustomRequestHandlerEvent( const QByteArray& request_, quintptr receiver_, const rec::rpc::ClientInfo& client_ )
						: QEvent( (QEvent::Type)InvokeCustomRequestHandlerEventId )
						, request( request_ )
						, receiver( receiver_ )
						, client( client_ )
					{
					}
					
					const QByteArray request;
					quintptr receiver;
					const rec::rpc::ClientInfo client;
				};
			};
		}
	}
}

#endif //_REC_RPC_SERVER_SERVER_H_
