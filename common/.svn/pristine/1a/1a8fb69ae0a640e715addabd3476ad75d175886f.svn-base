#ifndef _REC_DATAEXCHANGE_CLIENT_SOCKET_H_
#define _REC_DATAEXCHANGE_CLIENT_SOCKET_H_

#include "rec/dataexchange_lt/defines.h"
#include "rec/dataexchange_lt/types.h"
#include "rec/dataexchange_lt/configuration/Configuration.h"
#include "rec/dataexchange_lt/messages/Data.h"

#include <QtCore>
#include <QtNetwork>

namespace rec
{
	namespace dataexchange_lt
	{
		namespace client
		{
			class Socket : public QTcpSocket
			{
				Q_OBJECT
			public:
				enum {
					SendDataEventId = QEvent::User,
					ConnectToHostEventId,
					CloseEventId,
					RegisterChannelEventId,
					UnregisterChannelEventId,
				};

				Socket();

			Q_SIGNALS:
				void configurationReceived( const rec::dataexchange_lt::configuration::Configuration& config );
				void dataReceived( const rec::dataexchange_lt::messages::Data& value );
				void registerChannelReceived( const QString& );
				void unregisterChannelReceived( const QString& );

			private Q_SLOTS:
				void on_readyRead();

			private:
				void customEvent( QEvent* e );

				void readHeader();
				void readMessage();

				typedef enum { HeaderReceiveState, MessageDataReceiveState } ReceiveState;
				ReceiveState _receiveState;

				int _bytesToRead;
				quint8 _currentMessageId;
			};

			class SendDataEvent : public QEvent
			{
			public:
				SendDataEvent( const QByteArray& message_ )
					: QEvent( (QEvent::Type)Socket::SendDataEventId )
					, message( message_ )
				{
				}

				QByteArray message;
			};

			class ConnectToHostEvent : public QEvent
			{
			public:
				ConnectToHostEvent( const QHostAddress& address_, int port_ )
					: QEvent( (QEvent::Type)Socket::ConnectToHostEventId )
					, address( address_ )
					, port( port_ )
				{
				}

				const QHostAddress address;
				int port;
			};

			class CloseEvent : public QEvent
			{
			public:
				CloseEvent()
					: QEvent( (QEvent::Type)Socket::CloseEventId )
				{
				}
			};

			class RegisterChannelEvent : public QEvent
			{
			public:
				RegisterChannelEvent( const QByteArray& message_ )
					: QEvent( (QEvent::Type)Socket::RegisterChannelEventId )
					, message( message_ )
				{
				}

				QByteArray message;
			};

			class UnregisterChannelEvent : public QEvent
			{
			public:
				UnregisterChannelEvent( const QByteArray& message_ )
					: QEvent( (QEvent::Type)Socket::UnregisterChannelEventId )
					, message( message_ )
				{
				}

				QByteArray message;
			};
		}
	}
}

#endif //_REC_DATAEXCHANGE_CLIENT_SOCKET_H_

