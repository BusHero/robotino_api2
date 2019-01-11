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

#ifndef _REC_RPC_CLIENT_SOCKET_H_
#define _REC_RPC_CLIENT_SOCKET_H_

#include "rec/rpc/defines.h"
#include "rec/rpc/rec_rpc_Socket.hpp"
#include "rec/rpc/configuration/rec_rpc_configuration_Configuration.hpp"

#include <QtCore>
#include <QtNetwork>

namespace rec
{
	namespace rpc
	{
		namespace client
		{
			class Socket : public rec::rpc::Socket
			{
				Q_OBJECT
			public:
				enum EventType
				{
					SendRPCRequestEventId = rec::rpc::Socket::UserEventId,
					ConnectToHostEventId,
					CloseEventId,
					RegisterChannelEventId,
					UnregisterChannelEventId
				};

				Socket( const QString& name );

				void setTcpSocket( QTcpSocket* socket );
				void setLocalSocket( QLocalSocket* socket );

			Q_SIGNALS:
				void greetingReceived( const QString& );
				void configurationReceived( const rec::rpc::configuration::Configuration& config );
				void rpcResponseReceived( const QString&, quint32, quint16, const QByteArray& );
				void topicReceived( unsigned int, const rec::rpc::ClientInfo&, const QByteArray& );

			private Q_SLOTS:
				void on_stateChanged( QAbstractSocket::SocketState );
				void on_readyRead();

			private:
				void customEvent( QEvent* e );

				void readGreeting();
				void readHeader();
				void readMessage();
			};

			class RegisterChannelEvent : public SendMessageEvent
			{
			public:
				RegisterChannelEvent( const QByteArray& message_ )
					: SendMessageEvent( (rec::rpc::Socket::EventType)Socket::RegisterChannelEventId, message_ )
				{
				}
			};

			class UnregisterChannelEvent : public SendMessageEvent
			{
			public:
				UnregisterChannelEvent( const QByteArray& message_ )
					: SendMessageEvent( (rec::rpc::Socket::EventType)Socket::UnregisterChannelEventId, message_ )
				{
				}
			};

			class SendRPCRequestEvent : public SendMessageEvent
			{
			public:
				SendRPCRequestEvent( const QByteArray& message_ )
					: SendMessageEvent( (rec::rpc::Socket::EventType)Socket::SendRPCRequestEventId, message_ )
				{
				}
			};

			class ConnectToHostEvent : public QEvent
			{
			public:
				ConnectToHostEvent( const QHostAddress& address_, int port_, bool localIPCEnabled_ )
					: QEvent( (QEvent::Type)Socket::ConnectToHostEventId )
					, address( address_ )
					, port( port_ )
					, localIPCEnabled( localIPCEnabled_ )
				{
				}

				const QHostAddress address;
				int port;
				bool localIPCEnabled;
			};

			class CloseEvent : public QEvent
			{
			public:
				CloseEvent()
					: QEvent( (QEvent::Type)Socket::CloseEventId )
				{
				}
			};
		}
	}
}

#endif //_REC_RPC_CLIENT_SOCKET_H_
