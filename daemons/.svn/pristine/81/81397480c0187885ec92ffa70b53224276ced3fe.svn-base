#ifndef _MYSCANNER_H_
#define _MYSCANNER_H_

#include <QtCore>
#include <rec/LaserScanMessage.h>

class MyScanner : public QObject
{
	Q_OBJECT
public:
	MyScanner()
	{
		
	}
	
	virtual ~MyScanner()
	{

	}

	virtual void setFieldOfView( const double minimumAngle, const double maximumAngle ) = 0;
	virtual bool isDeviceOpen() const = 0;
	virtual bool openDevice( const QString& port, unsigned int speed ) = 0;
	virtual void closeDevice() = 0;

Q_SIGNALS:
	void scan( const rec::LaserScanMessage& );
};

#endif //_MYSCANNER_H_
