#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <QtCore>
#include <QtNetwork>

class Client : public QObject
{
	Q_OBJECT
public:
	Client();

	void connectToServer( const QString& address );

private Q_SLOTS:
	void on_stateChanged( QAbstractSocket::SocketState );
	void on_readyRead();

private:
	QTcpSocket* _socket;
};

#endif //_CLIENT_H_
