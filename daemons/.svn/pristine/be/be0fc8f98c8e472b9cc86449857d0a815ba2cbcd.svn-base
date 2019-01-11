#ifndef _CRUIZECORE_H_
#define _CRUIZECORE_H_

#include <QtCore>
#include "rec/cruizcore/Com.h"
#include "rec/robotino/shm/Gyroscope.h"
#include "rec/rpc/ClientInfo.h"

class CruizCore : public QObject, public rec::cruizcore::Com
{
	Q_OBJECT
public:
	CruizCore();

	bool open( const QString& device );

	void setRate( double rate );

public Q_SLOTS:
	void start();

private Q_SLOTS:
	void on_client_gyroscope_ext_info_changed( const rec::rpc::ClientInfoSet& );

Q_SIGNALS:
	void report( float angle, float rate );

private:
	void dataReceivedEvent( float rate, float angle );
	void errorReport( const char* error );
	void stopped();

	QString _device;
	int _numListeners;

	QTime _rate;
	int _minTimeElapsed;

	rec::robotino::shm::Gyroscope _shm;
};

#endif //_CRUIZECORE_H_
