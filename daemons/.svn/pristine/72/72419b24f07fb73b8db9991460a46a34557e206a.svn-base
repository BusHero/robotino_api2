#include "MyRPLidar.h"
#include "MyRPLidarImpl.h"

MyRPLidar::MyRPLidar()
	: _impl(new MyRPLidarImpl)
{
	bool ok = connect(_impl, SIGNAL(scan(const rec::LaserScanMessage&)), SIGNAL(scan(const rec::LaserScanMessage&)));
	Q_ASSERT(ok);
}

MyRPLidar::~MyRPLidar()
{
	closeDevice();
	delete _impl;
}
	
void MyRPLidar::setFieldOfView( const double minimumAngle, const double maximumAngle )
{
	_impl->setFieldOfView(minimumAngle, maximumAngle);
}

bool MyRPLidar::isDeviceOpen() const
{
	return _impl->isDeviceOpen();
}

bool MyRPLidar::openDevice( const QString& port, unsigned int speed )
{
	return _impl->openDevice(port, speed);
}

void MyRPLidar::closeDevice()
{
	_impl->closeDevice();
}
