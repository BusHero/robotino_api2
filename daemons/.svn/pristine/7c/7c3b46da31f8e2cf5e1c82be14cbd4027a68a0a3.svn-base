#include "Odometry.h"
#include "DriveSystem.h"
#include "Client.h"

extern Client* client;

Odometry::Odometry()
: _timer( new QTimer( this ) )
, _odoX( 0.0 )
, _odoY( 0.0 )
, _odoPhi( 0.0 )
, _gyroscope_phi_origin( 0.0 )
, _gyroscope_phi( 0.0 )
, _vX( 0.0f )
, _vY( 0.0f )
, _vOmega( 0.0f )
, _seq( 0 )
{
	bool connected = true;
	connected &= connect( client, SIGNAL( is_connected() ), SLOT( on_client_connected() ) );
	Q_ASSERT( connected );

	connected &= connect( client, SIGNAL( is_disconnected() ), SLOT( on_client_disconnected() ) );
	Q_ASSERT( connected );

	connected &= connect( client, SIGNAL( set_odometry_changed( double, double, double ) ), SLOT( on_set_odometry_changed( double, double, double ) ) );
	Q_ASSERT( connected );

	connected &= connect( _timer, SIGNAL( timeout() ), SLOT( on_timer_timeout() ) );
	Q_ASSERT( connected );

	_timer->setSingleShot( false );
}

void Odometry::on_set_odometry_changed( double x, double y, double phi )
{
	_odoX = x;
	_odoY = y;

	_gyroscope_phi_origin = _gyroscope_phi - phi;

	_odoPhi = phi;

	_seq = 0;
}

void Odometry::set_speeds( float m0, float m1, float m2, float time_delta )
{
	if( time_delta > 1.0f )
	{
		return;
	}

	//Get the actual velocity of the robot
	unprojectVelocity( &_vX, &_vY, &_vOmega, m0, m1, m2, _driveLayout );

	//std::cout << vX << " " << vY << " " << vOmega << std::endl;

	//Compute the estimated change of the position and rotation
	double elapsedSeconds = time_delta;
	double deltaXLocal = (double)_vX     * elapsedSeconds;
	double deltaYLocal = (double)_vY     * elapsedSeconds;
	double deltaOmega  = (double)_vOmega * elapsedSeconds;

	//Project the local coordinates to global coordinates
	_odoPhi += deltaOmega;
	_odoX += cos( _odoPhi ) * deltaXLocal - sin( _odoPhi ) * deltaYLocal;
	_odoY += sin( _odoPhi ) * deltaXLocal + cos( _odoPhi ) * deltaYLocal;
}

void Odometry::on_client_connected()
{
	_shm.attach();

	_driveLayout.rb = client->getParameter( "~/DriveLayout/rb", _driveLayout.rb ).toDouble();
	_driveLayout.rw = client->getParameter( "~/DriveLayout/rw", _driveLayout.rw ).toDouble();
	_driveLayout.fctrl = client->getParameter( "~/DriveLayout/fctrl", _driveLayout.fctrl ).toDouble();
	_driveLayout.gear = client->getParameter( "~/DriveLayout/gear", _driveLayout.gear ).toDouble();
	_driveLayout.mer = client->getParameter( "~/DriveLayout/mer", _driveLayout.mer ).toDouble();

	_timer->setInterval( client->getParameter( "~/Odometry/rate", 50 ).toInt() );
	_timer->start();
}

void Odometry::on_client_disconnected()
{
	_timer->stop();
}

void Odometry::on_timer_timeout()
{
	if( _shm.isNewDataAvailable() )
	{
		_shm.lock();
		_gyroscope_phi = _shm.angleZ();
		_odoPhi = _gyroscope_phi - _gyroscope_phi_origin;
		_shm.unlock();
	}

	while( _odoPhi >= M_PI )
	{
		_odoPhi -= 2.0 * M_PI;
	}
	while( _odoPhi < -M_PI )
	{
		_odoPhi += 2.0 * M_PI;
	}

	client->set_odometry( _odoX, _odoY, _odoPhi, _vX, _vY, _vOmega, _seq );
	++_seq;
}
