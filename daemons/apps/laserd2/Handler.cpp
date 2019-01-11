#include "Handler.h"
#include <iostream>
#include "rec/laserrangefinder/ScannerException.h"
#include "Client.h"
#include "MyScanner.h"
#include "rec/robotino/daemons/Log.h"

#define LASERD_PORT 8280
#define LASER_DEV "/dev/scanner_front"
#define DEV_WAIT_TIME 10

#ifdef WIN32
#define LOGFILE "laserd_log.txt"
#define SETTINGSFILE "laserd.conf"
#else
#define LOGFILE "/var/log/laserd"
#define SETTINGSFILE "/etc/robotino/laserd.conf"
#endif

extern Client* client;

Handler::Handler(const QString& port, unsigned int channel, MyScanner* scanner)
	: _scanner(scanner)
, _port( port )
, _channel( channel )
, _angle_min_deg( -90.0f )
, _angle_max_deg( 90.0f )
, _laser_status_timer( new QTimer( this ) )
{
	qRegisterMetaType< rec::LaserScanMessage >();

	bool connected = true;

	connected &= (bool)connect(_scanner, SIGNAL(scan(const rec::LaserScanMessage&)), SLOT(on_scan(const rec::LaserScanMessage&)), Qt::QueuedConnection);
	Q_ASSERT( connected );

	if( _port.isEmpty() )
	{
		_port = LASER_DEV;
	}
}

Handler::~Handler()
{
	delete _scanner;
	_scanner = NULL;
	rec::robotino::daemons::Log::singleton()->log( "Laserd stopped", 1 );
}

void Handler::init()
{
	rec::robotino::daemons::Log::singleton()->log( "Laserd2 started", 1 );

	_laser_status_timer->setSingleShot( false );
	_laser_status_timer->setInterval( 1000 );

	_angle_min_deg = client->getParameter("~/angle_min", -90).toInt();
	if (_angle_min_deg < -120)
	{
		rec::robotino::daemons::Log::singleton()->log("angle_min to small", 0);
		_angle_min_deg = -120;
	}

	_angle_max_deg = client->getParameter("~/angle_max", 90).toInt();
	if (_angle_max_deg > 120)
	{
		rec::robotino::daemons::Log::singleton()->log("angle_max to large", 0);
		_angle_max_deg = 120;
	}

	if (_angle_min_deg >= _angle_max_deg)
	{
		rec::robotino::daemons::Log::singleton()->log("angle_min greater or equal angle_max", 0);
		_angle_min_deg = -90;
		_angle_max_deg = 90;
	}

	bool connected = true;
	connected &= (bool)connect( _laser_status_timer, SIGNAL( timeout() ), SLOT( on_laser_status_timer_timeout() ) );
	Q_ASSERT( connected );

	qApp->postEvent( this, new StartEvent );
}

void Handler::on_scan( const rec::LaserScanMessage& scan )
{
	rec::robotino::daemons::Log::singleton()->log( "Scan received", 2 );
	client->on_laserscan( _channel, scan );
}

void Handler::on_laser_status_timer_timeout()
{
	if (false == _scanner->isDeviceOpen())
	{
		rec::robotino::daemons::Log::singleton()->log( "Device removed", 1 );
		qApp->quit();
	}
}


void Handler::customEvent( QEvent* e )
{
	switch( e->type() )
	{
	case StartEventId:
		{
			try
			{
				_scanner->setFieldOfView(_angle_min_deg / 180.0 * M_PI, _angle_max_deg / 180.0 * M_PI);
				rec::robotino::daemons::Log::singleton()->log(QString("Field of view %1° %2°").arg(_angle_min_deg).arg(_angle_max_deg), 1);

				if (_scanner->openDevice(_port, 0))
				{
					rec::robotino::daemons::Log::singleton()->log( "Rangefinder found at " + _port, 1 );
					_laser_status_timer->start();
				}
				else
				{
					rec::robotino::daemons::Log::singleton()->log( "No rangefinder at " + _port + " found", 1 );
					qApp->exit(1);
				}
			}
			catch( rec::laserrangefinder::ScannerException& e )
			{
				_scanner->closeDevice();

				rec::robotino::daemons::Log::singleton()->log( "No rangefinder found", 1 );
				qApp->exit(1);
			}
		}
		break;
	}
}


