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

#ifndef _REC_RPC_SERVER_SOCKET_H_
#define _REC_RPC_SERVER_SOCKET_H_

#include "rec/rpc/defines.h"
#include "rec/rpc/rec_rpc_Socket.hpp"
#include "rec/rpc/rec_rpc_ClientInfo.h"
#include "rec/rpc/configuration/rec_rpc_configuration_Configuration.hpp"

#include <QtCore>
#include <QtNetwork>

namespace rec
{
	namespace rpc
	{
		namespace server
		{
			class Config;

			class Socket : public rec::rpc::Socket
			{
				Q_OBJECT
			public:
				enum EventType
				{
					SendGreetingEventId = rec::rpc::Socket::UserEventId,
					SendConfigurationEventId,
					SendRPCResponseEventId,
					SendHTTPResponseEventId,
					SendCustomRequestResponseEventId,
				};

				enum Protocol
				{
					UnknownProtocol,
					REC_RPC,
					HTTP,
					CUSTOM
				};

				Socket( const Config& config );

				void setTcpSocket( QTcpSocket* socket );
				void setLocalSocket( QLocalSocket* socket );

				Protocol protocol() const { return _protocol; }

			Q_SIGNALS:
				void greetingReceived( const QString& );
				void rpcRequestReceived( const QString&, quint32, const QByteArray& );
				void rpcResponseReceived( const QString&, quint32, quint16, const QByteArray& );
				void topicReceived( unsigned int, const QByteArray& );
				void registerChannelReceived( unsigned int );
				void unregisterChannelReceived( unsigned int );

				void protocolReceived( rec::rpc::server::Socket::Protocol );

				void httpGetRequestReceived( const QString&, const QString& );
				void customRequestReceived( const QByteArray& );

			private Q_SLOTS:
				void on_closeTimer_timeout();
				void on_readyRead();

			private:
				void customEvent( QEvent* e );

				void readGreeting();
				void readHeader();
				void readMessage();

				Protocol _protocol;

				QTimer* _closeTimer;

				int _httpKeepAlive; // -1 means no info (HTTP 1.0?), 0 means close, 1 means keep-alive
				int _httpNumRequests;

				const Config& _config;
			};

			class SendGreetingEvent : public SendMessageEvent
			{
			public:
				SendGreetingEvent( const QByteArray& message_ )
					: SendMessageEvent( (rec::rpc::Socket::EventType)Socket::SendGreetingEventId, message_ )
				{
				}
			};

			class SendConfigurationEvent : public SendMessageEvent
			{
			public:
				SendConfigurationEvent( const QByteArray& message_ )
					: SendMessageEvent( (rec::rpc::Socket::EventType)Socket::SendConfigurationEventId, message_ )
				{
				}
			};

			class SendRPCResponseEvent : public SendMessageEvent
			{
			public:
				SendRPCResponseEvent( const QByteArray& message_ )
					: SendMessageEvent( (rec::rpc::Socket::EventType)Socket::SendRPCResponseEventId, message_ )
				{
				}
			};

			class SendCustomRequestResponseEvent : public SendMessageEvent
			{
			public:
				SendCustomRequestResponseEvent( const QByteArray& message_ )
					: SendMessageEvent( (rec::rpc::Socket::EventType)Socket::SendCustomRequestResponseEventId, message_ )
				{
				}
			};

			class SendHTTPResponseEvent : public QEvent
			{
			public:
				SendHTTPResponseEvent( const QString& header_, const QByteArray& content_ )
					: QEvent( (QEvent::Type)Socket::SendHTTPResponseEventId )
					, header( header_ )
					, content( content_ )
				{
				}

				QString header;
				QByteArray content;
			};
		}
	}
}

#endif //_REC_RPC_SERVER_SOCKET_H_
