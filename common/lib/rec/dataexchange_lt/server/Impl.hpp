#ifndef _REC_DATAEXCHANGE_SERVER_IMPL_H_
#define _REC_DATAEXCHANGE_SERVER_IMPL_H_

#include "rec/dataexchange_lt/defines.h"
#include "rec/dataexchange_lt/configuration/Configuration.h"
#include "rec/dataexchange_lt/messages/Data.h"

#include <QtCore>
#include <QtNetwork>

namespace rec
{
	namespace dataexchange_lt
	{
		namespace server
		{
			class ServerThread;

			class Impl : public QTcpServer
			{
				Q_OBJECT
			public:
				enum {
					ListenEventId = QEvent::User,
					CloseEventId,
					DisconnectAllClientsId,
					SendDataEventId
				};

				Impl( rec::dataexchange_lt::configuration::Configuration* configuration );
				
				~Impl();

				int numClientsConnected() const;

				void sendToAllClient( const QString& name, const QByteArray& data, ServerThread* exclude = NULL );

			Q_SIGNALS:
				void clientConnected( quint32 );
				void clientDisconnected( quint32 );
				void serverError( QAbstractSocket::SocketError error, const QString& errorString );
				void clientError( QAbstractSocket::SocketError error, const QString& errorString );
				void dataReceived( const rec::dataexchange_lt::messages::Data& value );
				void listening();
				void closed();

			private Q_SLOTS:
				void on_dataReceived( rec::dataexchange_lt::server::ServerThread*, const rec::dataexchange_lt::messages::Data& value );
				void on_configuration_changed();
				void on_connected( quint32 );
				void on_disconnected( quint32 );
				void on_client_error( QAbstractSocket::SocketError error, const QString& errorString );
				void on_channelRegistered( rec::dataexchange_lt::server::ServerThread*, const QString& name );

			private:
				void customEvent( QEvent* e );

				void incomingConnection( int socketDescriptor );
				void disconnectAllClients();

				rec::dataexchange_lt::configuration::Configuration* _configuration;

				QMutex _configurationMessageMutex;
				QByteArray _configurationMessage;

				int _numClientsConnected;
			};

			class ListenEvent : public QEvent
			{
			public:
				ListenEvent( int port_ )
					: QEvent( (QEvent::Type)Impl::ListenEventId )
					, port( port_ )
				{
				}

				int port;
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

			class SendDataEvent : public QEvent
			{
			public:
				SendDataEvent( const QString& name_, const QByteArray& message_ )
					: QEvent( (QEvent::Type)Impl::SendDataEventId )
					, name( name_ )
					, message( message_ )
				{
				}

				const QString name;
				QByteArray message;
			};
		}
	}
}

#endif //_REC_DATAEXCHANGE_SERVER_SERVER_H_
