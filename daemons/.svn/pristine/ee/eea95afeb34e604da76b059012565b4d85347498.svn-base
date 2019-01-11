#ifndef _MYRPLIDAR_H_
#define _MYRPLIDAR_H_

#include "MyScanner.h"

class MyRPLidarImpl;

class MyRPLidar : public MyScanner
{
	Q_OBJECT
public:
	MyRPLidar();
	~MyRPLidar();
	
	void setFieldOfView( const double minimumAngle, const double maximumAngle );
	bool isDeviceOpen() const;
	bool openDevice( const QString& port, unsigned int speed );
	void closeDevice();
	
private:
	MyRPLidarImpl* _impl;
};

#endif //_MYRPLIDAR_H_
