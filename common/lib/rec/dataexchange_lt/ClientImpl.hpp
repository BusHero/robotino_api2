#ifndef _REC_DATAEXCHANGE_CLIENTIMPL_H_
#define _REC_DATAEXCHANGE_CLIENTIMPL_H_

#include "rec/dataexchange_lt/client/Client.h"

namespace rec
{
	namespace dataexchange_lt
	{
		class Client;

		class ClientImpl : public QObject
		{
			Q_OBJECT
		public:
			ClientImpl( Client* parent );

			~ClientImpl();

			rec::dataexchange_lt::client::Client* client();

		private Q_SLOTS:
			void on_client_stateChanged( QAbstractSocket::SocketState );
			void on_client_error( QAbstractSocket::SocketError socketError, const QString& );
			void on_client_dataReceived( const rec::dataexchange_lt::messages::Data& value );
			void on_client_connected();
			void on_client_disconnected();

		private:
			Client* _parent;
			rec::dataexchange_lt::client::Client* _client;
		};

		inline rec::dataexchange_lt::client::Client* ClientImpl::client()
		{
			return _client;
		}
	}
}

#endif //_REC_DATAEXCHANGE_CLIENTIMPL_H_

