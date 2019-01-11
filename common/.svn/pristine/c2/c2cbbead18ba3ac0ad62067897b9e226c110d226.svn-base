#ifndef _REC_DATAEXCHANGE_UDP_SOCKET_HPP_
#define _REC_DATAEXCHANGE_UDP_SOCKET_HPP_

#include <QtCore>
#include <QtNetwork>
#include "rec/dataexchange_lt/udp/Listener.h"

namespace rec
{
	namespace dataexchange_lt
	{
		namespace udp
		{
			class Socket : public QObject
			{
				Q_OBJECT
			public:
				enum {
					ListenEventId = QEvent::User,
					CloseEventId,
					SendDataEventId
				};

				Socket();
				
				~Socket();

				bool isListening() const;

				quint16 localPort() const;

			Q_SIGNALS:
				void listening();
				void closed();
				void error( QAbstractSocket::SocketError error, const QString& errorString );
				void message0Received( const QByteArray& data );
				void message1Received( const QByteArray& data );

			private Q_SLOTS:
				void on_udpSocket_error( QAbstractSocket::SocketError /*socketError*/ );
				void on_udpSocket_stateChanged( QAbstractSocket::SocketState /*socketState*/ );
				void on_udpSocket_readyRead();

			private:
				void customEvent( QEvent* e );

				QUdpSocket* _udpSocket;
			};

			class ListenEvent : public QEvent
			{
			public:
				ListenEvent( int port_ )
					: QEvent( (QEvent::Type)Socket::ListenEventId )
					, port( port_ )
				{
				}

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

			class SendDataEvent : public QEvent
			{
			public:
				SendDataEvent( const QByteArray& message_, const ListenerList listeners_ )
					: QEvent( (QEvent::Type)Socket::SendDataEventId )
					, message( message_ )
					, listeners( listeners_ )
				{
				}

				QByteArray message;
				ListenerList listeners;
			};
		}
	}
}

#endif //_REC_DATAEXCHANGE_UDP_SOCKET_HPP_
