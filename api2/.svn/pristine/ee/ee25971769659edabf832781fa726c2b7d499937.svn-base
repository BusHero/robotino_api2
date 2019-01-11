#ifndef _CLIENTSCONNECTED_H_
#define _CLIENTSCONNECTED_H_

#include <QtGui>
#include "rec/rpc/ClientInfo.h"

class ClientsConnected : public QListWidget
{
	Q_OBJECT
public:
	ClientsConnected();

public Q_SLOTS:
	void setClients( const QVector< rec::rpc::ClientInfo >& clients );
	void setCurrentController( const rec::rpc::ClientInfo& info );

private:
	QVector< rec::rpc::ClientInfo > _clients;
	rec::rpc::ClientInfo _currentController;
};

#endif //_CLIENTSCONNECTED_H_
