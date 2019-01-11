#ifndef _REC_DATAEXCHANGE_SERVER_SERVERTHREAD_H_
#define _REC_DATAEXCHANGE_SERVER_SERVERTHREAD_H_

#include "rec/dataexchange_lt/client/Socket.hpp"

#include <QtCore>
#include <QtNetwork>

namespace rec
{
	namespace dataexchange_lt
	{
		namespace server
		{
			class ServerThread : public QThread
			{
				Q_OBJECT
			public:
				ServerThread( QObject* parent, int socket );

				~ServerThread();

				void start();

				void send( const QString& name, const QByteArray& data );

			Q_SIGNALS:
				void dataReceived( rec::dataexchange_lt::server::ServerThread* thread,
					const rec::dataexchange_lt::messages::Data& value );

				void error( QAbstractSocket::SocketError, const QString& );

				void connected( quint32 );
				void disconnected( quint32 );

				void channelRegistered( rec::dataexchange_lt::server::ServerThread*, const QString& name );

			private Q_SLOTS:
				void on_disconnected();
				void on_error( QAbstractSocket::SocketError );
				void on_dataReceived( const rec::dataexchange_lt::messages::Data& );
				void on_registerChannelReceived( const QString& );
				void on_unregisterChannelReceived( const QString& );

			private:
				void run();

				int _socketDescriptor;

				QMutex _socketMutex;
				rec::dataexchange_lt::client::Socket* _socket;

				QSemaphore _startSemaphore;

				QHostAddress _peerAddress;

				QMutex _registeredChannelsMutex;
				QSet< QString > _registeredChannels;
			};
		}
	}
}

#endif //_REC_DATAEXCHANGE_SERVER_SERVERTHREAD_H_
