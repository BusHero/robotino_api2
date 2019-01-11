#ifndef _REC_DATAEXCHANGE_UDP_COMIMPL_HPP_
#define _REC_DATAEXCHANGE_UDP_COMIMPL_HPP_

#include "rec/dataexchange_lt/udp/Com.h"
#include "rec/dataexchange_lt/udp/Message0.hpp"
#include "rec/dataexchange_lt/udp/Message1.hpp"
#include "rec/dataexchange_lt/udp/ByteArrayMessage.hpp"
#include "rec/dataexchange_lt/udp/Listener.h"

#include <QtCore>
#include <QtNetwork>

namespace rec
{
	namespace dataexchange_lt
	{
		namespace udp
		{
			class Socket;

			class ComImpl : public QThread
			{
				Q_OBJECT
			public:
				ComImpl( Com* com );

				virtual ~ComImpl();

				bool listen( int port, bool blocking );

				bool isListening() const;

				int serverPort() const;

				void close();

				void addListener( const QString& listenerIpAddress );

				void removeListener( const QString& listenerIpAddress );

				/*
				@param interval Interval for sending message in milliseconds.
				0 : Send message whenever data has changed
				-1: Turn of sending message
				*/
				void setSendingInterval( int interval );

				bool isSendingEnabled( Com::MessageType message ) const;
				void setSendingEnabled( Com::MessageType message, bool enable );

				int message0ReceivedData( int n ) const;

				void sendMessage0Data( const QVector< int >& data );

				void sendMessage0Data( int n, int value );

				int message1ReceivedData( int n ) const;

				void sendMessage1Data( const QVector< int >& data );

				void sendMessage1Data( int n, int value );

				void sendByteArray( const char* data, unsigned int dataSize );

			private Q_SLOTS:
				void on_socket_listening();
				void on_socket_closed();

				void on_socket_message0Received( const QByteArray& data );
				void on_socket_message1Received( const QByteArray& data );

				void on_socket_error( QAbstractSocket::SocketError error, const QString& errorString );

				void on_sendTimer_timeout();

				void on_message0Input_dataChanged();
				void on_message1Input_dataChanged();

				void on_message0Output_dataChanged();
				void on_message1Output_dataChanged();

				void on_byteArrayInput_dataChanged();
				void on_byteArrayOutput_dataChanged();

			private:
				void run();
				void setupServer();

				Com* _com;

				bool _run;
				QSemaphore _startSemaphore;

				Socket* _socket;
				Message0 _message0Output;
				Message0 _message0Input;
				Message1 _message1Output;
				Message1 _message1Input;
				ByteArrayMessage _byteArrayOutput;
				ByteArrayMessage _byteArrayInput;

				mutable QMutex _socketMutex;
				QWaitCondition _listenCondition;

				QTimer* _sendTimer;
				int _sendingInterval;
				const int _idleInterval;

				ListenerList _listeners;
				mutable QMutex _listenersMutex;

				std::string _errorString;

				QMap< Com::MessageType, bool > _isSendingEnabled;
			};
		}
	}
}

#endif //_REC_DATAEXCHANGE_UDP_COMIMPL_HPP_
