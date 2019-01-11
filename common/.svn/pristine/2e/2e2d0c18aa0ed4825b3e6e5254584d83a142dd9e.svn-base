#include "rec/laserrangefinder/Hokuyo.h"
#include "rec/laserrangefinder/ScannerException.h"

using namespace rec::laserrangefinder;

Hokuyo::Hokuyo()
{
}

Hokuyo::~Hokuyo()
{
	closeDevice();
}

void Hokuyo::setFieldOfView( const double minimumAngle, const double maximumAngle )
{
	HokuyoDriverConfig cfg = config();
	cfg.min_angle = minimumAngle;
	cfg.max_angle = maximumAngle;
	config_update( cfg );
}

bool Hokuyo::isDeviceOpen() const
{
	switch( getState() )
	{
	case Driver::RUNNING:
		return true;

	default:
		return false;
	}
}

bool Hokuyo::openDevice( const std::string& port, unsigned int speed )
{
	HokuyoDriverConfig cfg = config();
	cfg.port = port;
	config_update( cfg );
	
	doOpen();

	if( Driver::OPENED == getState() )
	{
		doStart();
		_scan_timer.start();
	}

	return ( Driver::RUNNING == getState() );
}

void Hokuyo::closeDevice()
{
	doClose();
}

void Hokuyo::logCallback( const std::string& message )
{
}

void Hokuyo::scanCallback( const rec::LaserScanMessage& scan )
{
}

void Hokuyo::infoMessage( const std::string& message )
{
	logCallback( "INFO: " + message );
}

void Hokuyo::warnMessage( const std::string& message )
{
	logCallback( "WARN: " + message );
}

void Hokuyo::debugMessage( const std::string& message )
{
	logCallback( "DEBUG: " + message );
}

void Hokuyo::errorMessage( const std::string& message )
{
	logCallback( "ERROR: " + message );
}

void Hokuyo::hokuyoScanCallback( const hokuyo::LaserScan& scan )
{
	static unsigned int sequence = 0;

	rec::LaserScanMessage scan_msg;

	scan_msg.angle_min = scan.config.min_angle;
    scan_msg.angle_max = scan.config.max_angle;
    scan_msg.angle_increment = scan.config.ang_increment;
    scan_msg.time_increment = scan.config.time_increment;
    scan_msg.scan_time = scan.config.scan_time;
    scan_msg.range_min = scan.config.min_range;
    scan_msg.range_max = scan.config.max_range;
	scan_msg.ranges = QVector<float>::fromStdVector( scan.ranges );
    scan_msg.intensities = QVector<float>::fromStdVector( scan.intensities );
	scan_msg.header.stamp = (quint32)_scan_timer.restart();//( scan.system_time_stamp / 1000000 );
    scan_msg.header.seq = sequence++;
    scan_msg.header.frame_id = "laser_link";

	scanCallback( scan_msg );
}

