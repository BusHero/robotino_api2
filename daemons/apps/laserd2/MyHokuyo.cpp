#include "MyHokuyo.h"
#include "rec/robotino/daemons/Log.h"

MyHokuyoImpl::MyHokuyoImpl( MyHokuyo* scanner )
	: _scanner( scanner )
{
}

void MyHokuyoImpl::logCallback( const std::string& message )
{
	_scanner->logCb( message.c_str() );
}

void MyHokuyoImpl::scanCallback( const rec::LaserScanMessage& scan )
{
	_scanner->scanCb( scan );
}

MyHokuyo::MyHokuyo()
	: _hokuyo( this )
{
}

MyHokuyo::~MyHokuyo()
{
	closeDevice();
}

void MyHokuyo::setFieldOfView( const double minimumAngle, const double maximumAngle )
{
	_hokuyo.setFieldOfView( minimumAngle, maximumAngle );
}
	

bool MyHokuyo::isDeviceOpen() const
{
	return _hokuyo.isDeviceOpen();
}

bool MyHokuyo::openDevice( const QString& port, unsigned int speed )
{
	return _hokuyo.openDevice( port.toStdString(), speed );
}

void MyHokuyo::closeDevice()
{
	_hokuyo.closeDevice();
}

void MyHokuyo::logCb( const QString& message )
{
	rec::robotino::daemons::Log::singleton()->log( message, 1 );
}

void MyHokuyo::scanCb( const rec::LaserScanMessage& data )
{
	Q_EMIT scan( data );
}