#ifndef _MYHOKUYO_H_
#define _MYHOKUYO_H_

#include "MyScanner.h"
#include "rec/laserrangefinder/Hokuyo.h"

class MyHokuyo;

class MyHokuyoImpl : public rec::laserrangefinder::Hokuyo
{
public:
	MyHokuyoImpl( MyHokuyo* scanner );

	void logCallback( const std::string& message );
	void scanCallback( const rec::LaserScanMessage& scan );

private:
	MyHokuyo* _scanner;
};

class MyHokuyo : public MyScanner
{
	Q_OBJECT
public:
	MyHokuyo();
	~MyHokuyo();

	void setFieldOfView( const double minimumAngle, const double maximumAngle );
	bool isDeviceOpen() const;
	bool openDevice( const QString& port, unsigned int speed );
	void closeDevice();

	void scanCb( const rec::LaserScanMessage& );
	void logCb( const QString& );

private:
	MyHokuyoImpl _hokuyo;
};

#endif //_MYHOKUYO_H_