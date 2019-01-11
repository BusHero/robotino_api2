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

#ifndef _REC_RPC_SOCKET_H_
#define _REC_RPC_SOCKET_H_

#include "rec/rpc/defines.h"

#include <QtCore>
#include <QtNetwork>

namespace rec
{
	namespace rpc
	{
		class Socket : public QObject
		{
			Q_OBJECT
		public:
			enum EventType
			{
				PublishTopicEventId = QEvent::User + 200,
				UserEventId,
			};

			Socket( const QString& name );

			virtual ~Socket();

			void publishTopic( unsigned int id, const QByteArray& metaData, const QByteArray& source, const QByteArray& payload, bool enqueue );

			QAbstractSocket::SocketState state() const;
			QString errorString() const;
			QHostAddress localAddress() const;
			quint16 localPort() const;
			QHostAddress peerAddress() const;
			quint16 peerPort() const;

			QString socketName() const { return _socketName; }
			void setSocketName( const QString& name );

			bool isLocal() const { return NULL != localSocket(); }

		Q_SIGNALS:
			void stateChanged( QAbstractSocket::SocketState );
			void disconnected();
			void error( QAbstractSocket::SocketError );
			void log( const QString&, int level );

		protected:
			enum ReceiveState { GreetingReceiveState, HeaderReceiveState, MessageDataReceiveState };
			ReceiveState _receiveState;

			int _bytesToRead;
			quint8 _currentMessageId;

			QByteArray _messageBuf;

			QString _socketName;

			quint16 _localSocketId;

			void setTcpSocket( QTcpSocket* socket );
			void setLocalSocket( QLocalSocket* socket );

			void closeSocket();

			QTcpSocket* tcpSocket() const { return _tcpSocket; }
			QLocalSocket* localSocket() const { return _localSocket; }
			QIODevice* currentSocket() const { return _currentSocket; }

			void customEvent( QEvent* ev );

		private:
			class Id
			{
			public:
				static quint16 getNext();

			private:
				static QMutex mutex;
				static quint16 id;
			};

			enum PrivateEventType
			{
				TopicEvent = QEvent::User + 100,
			};

			QTcpSocket* _tcpSocket;
			QLocalSocket* _localSocket;
			QIODevice* _currentSocket;

			QMutex _topicMutex;
			QMap< unsigned int, QQueue< QByteArray > > _topicMap;
			QQueue< unsigned int > _topicQueue;

			void topicEvent();

			QTime* _writeBusyTimer;

		private Q_SLOTS:
			void on_localSocket_error( QLocalSocket::LocalSocketError err );
			void on_localSocket_stateChanged( QLocalSocket::LocalSocketState state );
			void on_tcpSocket_stateChanged( QAbstractSocket::SocketState );
			void on_tcpSocket_error( QAbstractSocket::SocketError );
		};

		class SendMessageEvent : public QEvent
		{
		public:
			SendMessageEvent( rec::rpc::Socket::EventType type, const QByteArray& message_ )
				: QEvent( (QEvent::Type)type )
				, message( message_ )
			{
			}

			QByteArray message;
		};

	}
}

#endif //_REC_RPC_SOCKET_H_
