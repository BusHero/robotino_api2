#ifndef _COM_H_
#define _COM_H_

#include <QtGui>
#include "rec/robotino3/grippercom/Com.h"

class Com : public QObject, public rec::robotino3::grippercom::Com
{
	Q_OBJECT
public:
	Com();

Q_SIGNALS:
	void hardwareVersion( const QString& );
	void softwareVersion( const QString& );
	void log( const QString& );
	void sensorReadings( float i1, float i2, float ad3, float ad4, float ad6 );

public Q_SLOTS:

private:
	void hardwareVersionCb( const char* version );
	void softwareVersionCb( const char* version );
	void logCb( const char* message );
	void sensorReadingsCb( float i1, float i2, float ad3, float ad4, float ad6 );
};

#endif //_COM_H_
