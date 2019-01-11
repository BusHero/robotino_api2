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

#ifndef _REC_RPC_CLIENT_CLIENT_H_
#define _REC_RPC_CLIENT_CLIENT_H_

#include "rec/rpc/rec_rpc_Client.h"
#include "rec/rpc/configuration/rec_rpc_configuration_Configuration.hpp"

#include <QtCore>
#include <QtNetwork>

namespace rec
{
	namespace rpc
	{
		namespace client
		{
			class Socket;
			class Serializer;

			class PendingRequest : public QObject
			{
				Q_OBJECT
			public:
				PendingRequest( const QString& funcName, quint32 seqNum_, bool blocking_, serialization::SerializablePtr result_, unsigned int timeout );

				void start();
				void stop();

				QString funcName;
				quint32 seqNum;
				bool blocking;
				ErrorCode errorCode;
				serialization::SerializablePtr result;

				QMutex mutex;
				QWaitCondition cond;

			Q_SIGNALS:
				void timeout( PendingRequest* );
				void startTimer();
				void stopTimer();

			private Q_SLOTS:
				void on_timer_timeout();

			private:
				QTimer timer;
			};

			class SequenceNumber
			{
			public:
				SequenceNumber();

				operator quint32();

			private:
				QMutex mutex;
				quint32 num;
			};

			class Client : public QThread
			{
				Q_OBJECT
			public:
				Client( QObject* parent = NULL );

				~Client();

				QString name() const { return _name; }
				void setName( const QString& name );

				bool isMultiThreadedSerializationEnabled() const { return _multiThreadedSerialization; }
				void setMultiThreadedSerializationEnabled( bool enabled );

				bool isLocalIPCEnabled() const { return _localIPCEnabled; }
				void setLocalIPCEnabled( bool enabled );

				rec::rpc::configuration::Configuration* configuration() { return &_configuration; }

				void connectToServer( unsigned int msTimeout );
				void disconnectFromServer();

				void setAutoReconnectEnabled( bool enable, unsigned int ms );

				bool isConnected() const;

				QString address() const;
				void setAddress( const QString& address );

				QHostAddress localAddress() const;
				quint16 localPort() const;

				QHostAddress peerAddress() const;
				quint16 peerPort() const;

				QString expectedGreeting() const;
				void setExpectedGreeting( const QString& greeting );

				unsigned int msTimeout() const;
				void setMsTimeout( unsigned int timeout );

				void getServerVersion( int* major, int* minor, int* patch, int* date, QString* suffix );

				void registerNotifier( const QString& name, rec::rpc::NotifierBasePtr notifier );
				void unregisterNotifier( const QString& name );
				bool isNotifierRegistered( const QString& name ) const;

				void registerTopicListener( const QString& name, rec::rpc::TopicListenerBasePtr listener );
				void unregisterTopicListener( const QString& name );
				bool isTopicListenerRegistered( const QString& name ) const;

				void invoke( const QString& name, rec::rpc::serialization::SerializablePtrConst param, rec::rpc::serialization::SerializablePtr result, bool blocking );
				void publishTopic( const QString& name, rec::rpc::serialization::SerializablePtrConst data );

			Q_SIGNALS:
				void stateChanged( QAbstractSocket::SocketState );
				void error( QAbstractSocket::SocketError socketError, const QString& );
				void connected();
				void disconnected( rec::rpc::ErrorCode );
				void log( const QString&, int level );

			private Q_SLOTS:
				void on_stateChanged( QAbstractSocket::SocketState );
				void on_error( QAbstractSocket::SocketError );
				void on_greetingReceived( const QString& greeting );
				void on_configurationReceived( const rec::rpc::configuration::Configuration& );
				void on_disconnected();
				void on_topicReceived( unsigned int id, const rec::rpc::ClientInfo& source, const QByteArray& );
				void on_rpcResponseReceived( const QString& name, quint32 seqNum, quint16 errorCode, const QByteArray& serResult );
				void on_pendingRequest_timeout( PendingRequest* req );
				void on_connectTimer_timeout();
				void on_autoReconnectTimer_timeout();

				void on_RPCRequestSerialized( const QString& name, quint32 seqNum, const QByteArray& serParam );
				void on_topicDataSerialized( unsigned int id, const QByteArray& serData );

				void on_crashTimerTimeout();

			private:
				struct SharedMemInfo
				{
					SharedMemInfo() : seqNum( 0 ) { }
					unsigned int seqNum;
					QSharedPointer< QSharedMemory > mem;
				};

				enum Event
				{
					AutoReconnectEventId = QEvent::User,
					ConnectToHostEventId,
					StopConnectTimerEventId,
					NotifyEventId,
					ListenTopicEventId,
					SetNameEventId,
				};

				void run();

				void customEvent( QEvent* e );

				void manageTopicListener( unsigned int id, bool add );

				bool publishTopicLocal( unsigned int id, const QByteArray& serData );
				void publishTopicRemote( unsigned int id, const QByteArray& serData, bool enqueue );

				void createSharedMem( unsigned int id );

				void startAutoReconnect();

				QString _name;

				rec::rpc::NotifierBasePtr getNotifier( const QString& name );

				QString _expectedGreeting;

				rec::rpc::configuration::Configuration _configuration;
				bool _isConfigurationReceived;

				QHostAddress _address;
				int _port;

				mutable QMutex _socketMutex;

				Socket* _socket;

				bool _multiThreadedSerialization;
				Serializer* _serializer;

				QSemaphore _startSemaphore;

				QMap< QString, rec::rpc::NotifierBasePtr > _notifiers;
				mutable QMutex _notifiersMutex;
				QMap< unsigned int, rec::rpc::TopicListenerBasePtr > _topicListeners;
				mutable QMutex _topicListenersMutex;

				QMap< QString, rec::rpc::TopicListenerBasePtr > _topicsToRegister;

				QTimer _connectTimer;

				QMutex _pendingRequestsMutex;
				QMap< unsigned int, PendingRequest* > _pendingRequests;

				SequenceNumber _seqNum;

				unsigned int _timeout;

				QMap< unsigned int, SharedMemInfo > _topicLocalMemories;

				ErrorCode _lastError;

				int _msAutoReconnect;

				QTimer* _autoReconnectTimer;

				bool _localIPCEnabled;

				static bool _once;

				class ConnectToHostEvent : public QEvent
				{
				public:
					ConnectToHostEvent( const QHostAddress& address_, int port_, int msTimeout_ )
						: QEvent( (QEvent::Type)ConnectToHostEventId )
						, address( address_ )
						, port( port_ )
						, msTimeout( msTimeout_ )
					{
					}

					const QHostAddress address;
					int port;
					int msTimeout;
				};

				class NotifyEvent : public QEvent
				{
				public:
					NotifyEvent( rec::rpc::NotifierBasePtr notifier_, rec::rpc::serialization::SerializablePtrConst result_, rec::rpc::ErrorCode errorCode_ )
						: QEvent( (QEvent::Type)NotifyEventId )
						, notifier( notifier_ )
						, result( result_ )
						, errorCode( errorCode_ )
					{
					}

					rec::rpc::NotifierBasePtr notifier;
					rec::rpc::serialization::SerializablePtrConst result;
					rec::rpc::ErrorCode errorCode;
				};

				class ListenTopicEvent : public QEvent
				{
				public:
					ListenTopicEvent(
						rec::rpc::TopicListenerBasePtr listener_,
						rec::rpc::serialization::SerializablePtrConst data_,
						const rec::rpc::ClientInfo client_,
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

				class SetNameEvent : public QEvent
				{
				public:
					SetNameEvent( const QString& name_ )
						: QEvent( (QEvent::Type)SetNameEventId )
						, name( name_ )
					{
					}

					QString name;
				};
			};

			inline unsigned int Client::msTimeout() const
			{
				return _timeout;
			}

			inline void Client::setMsTimeout( unsigned int timeout )
			{
				_timeout = timeout;
			}
		}
	}
}

#endif //_REC_RPC_CLIENT_CLIENT_H_
