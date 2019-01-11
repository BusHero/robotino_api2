#ifndef _REC_DATAEXCHANGE_SERVERIMPL_H_
#define _REC_DATAEXCHANGE_SERVERIMPL_H_

#include "rec/dataexchange_lt/server/Server.h"
#include "rec/dataexchange_lt/messages/Data.h"

namespace rec
{
	namespace dataexchange_lt
	{
		class Server;

		class ServerImpl : public QObject
		{
			Q_OBJECT
		public:
			ServerImpl( Server* parent );
			
			~ServerImpl();

			rec::dataexchange_lt::server::Server* server();

		private Q_SLOTS:
			void on_server_clientConnected( quint32 );
			void on_server_clientDisconnected( quint32 );
			void on_server_serverError( QAbstractSocket::SocketError error, const QString& errorString );
			void on_server_clientError( QAbstractSocket::SocketError error, const QString& errorString );
			void on_server_dataReceived( const rec::dataexchange_lt::messages::Data& value );
			void on_server_closed();
			void on_server_listening();

		private:
			Server* _parent;
			rec::dataexchange_lt::server::Server* _server;
		};
			
		inline rec::dataexchange_lt::server::Server* ServerImpl::server()
		{
			return _server;
		}
	}
}

#endif //_REC_DATAEXCHANGE_SERVERIMPL_H_

