#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "rec/robotino/rpc/Client.h"
#include "rec/robotino/rpc/GrapplerServoInfo.h"

class Client : public rec::robotino::rpc::Client
{
	Q_OBJECT
public:
	Client();

Q_SIGNALS:

public Q_SLOTS:
	void on_storePositions( const QVector< rec::robotino::rpc::GrapplerServoInfo >& );
	void on_setServos( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data );
	void on_setReadings( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data );
	void on_toggleTorque();

private Q_SLOTS:
	void on_connected();
	void on_disconnected();

private:
	QVector< rec::robotino::rpc::GrapplerServoInfo> _setServos;
};

#endif //_CLIENT_H_
