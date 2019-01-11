#include "Odometry.h"
#include "DriveSystem.h"
#include "Client.h"
#include <iostream>

extern Client* client;

Odometry::Odometry()
: _timer( new QTimer( this ) )
, _odoX( 0.0 )
, _odoY( 0.0 )
, _odoPhi( 0.0 )
, _gyroscope_phi_origin( 0.0 )
, _vX( 0.0f )
, _vY( 0.0f )
, _vOmega( 0.0f )
, _seq( 0 )
, _gyroIdleTime( 1000 )
{
	bool connected = true;
	connected &= (bool)connect( client, SIGNAL( is_connected() ), SLOT( on_client_connected() ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( client, SIGNAL( is_disconnected() ), SLOT( on_client_disconnected() ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( client, SIGNAL( set_odometry_changed( double, double, double ) ), SLOT( on_set_odometry_changed( double, double, double ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( _timer, SIGNAL( timeout() ), SLOT( on_timer_timeout() ) );
	Q_ASSERT( connected );

	_timer->setSingleShot( false );

	_gyroIdleTimer.start();
}

void Odometry::setDriveLayout( const DriveLayout& layout )
{
	_driveLayout = layout;
}

void Odometry::on_set_odometry_changed( double x, double y, double phi )
{
	//std::cout << "on_set_odometry_changed " << x << " " << y << " " << 57.3*phi << std::endl;
	//std::cout << "current values " << _odoX << " " << _odoY << " " << 57.3*_odoPhi << std::endl;
	_odoX = x;
	_odoY = y;

	//std::cout << "_gyroscope_phi_origin " << 57.3*_gyroscope_phi_origin << std::endl;
	_gyroscope_phi_origin += (_odoPhi - phi);
	//std::cout << "_gyroscope_phi_origin " << 57.3*_gyroscope_phi_origin << std::endl;

	_odoPhi = phi;

	_seq = 0;
}

void Odometry::setGyroscopeExt( double phi, double rate, const rec::rpc::ClientInfo& info )
{
	if (_gyroIdleTimer.elapsed() < _gyroIdleTime)
	{
		_odoPhi = phi - _gyroscope_phi_origin;
	}
	else
	{
		_gyroscope_phi_origin = phi - _odoPhi;
	}
}

void Odometry::setGyroscopeInternal(double phi, double rate)
{
	_odoPhi = phi - _gyroscope_phi_origin;
}

void Odometry::set_motor_readings( const QVector< float >& speeds /*in rpm*/, const QVector< int >& /*positions*/, const QVector< float >& /*currents in A*/, float deltat )
{
	if( deltat > 1.0f )
	{
		return;
	}

	//Get the actual velocity of the robot
	unprojectVelocity( &_vX, &_vY, &_vOmega, speeds[0], speeds[1], speeds[2], _driveLayout );

	//std::cout << vX << " " << vY << " " << vOmega << std::endl;

	//Compute the estimated change of the position and rotation
	double elapsedSeconds = deltat;
	double deltaXLocal = (double)_vX     * elapsedSeconds;
	double deltaYLocal = (double)_vY     * elapsedSeconds;
	double deltaOmega  = (double)_vOmega * elapsedSeconds;

	//Project the local coordinates to global coordinates
	_odoPhi += deltaOmega;
	_odoX += cos( _odoPhi ) * deltaXLocal - sin( _odoPhi ) * deltaYLocal;
	_odoY += sin( _odoPhi ) * deltaXLocal + cos( _odoPhi ) * deltaYLocal;

	if( _vX != 0 || _vY != 0 || _vOmega != 0 )
	{
		_gyroIdleTimer.start();
	}
}

void Odometry::setMotorSetpointNotZero()
{
	_gyroIdleTimer.start();
}

void Odometry::on_client_connected()
{
	_gyroIdleTime =  client->getParameter( "~/Odometry/gyro_idle_time", 1000 ).toInt();

	_timer->setInterval( client->getParameter( "~/Odometry/publish_rate", 50 ).toInt() );
	_timer->start();
}

void Odometry::on_client_disconnected()
{
	_timer->stop();
}

void Odometry::on_timer_timeout()
{
	while( _odoPhi >= M_PI )
	{
		_odoPhi -= 2.0 * M_PI;
	}
	while( _odoPhi < -M_PI )
	{
		_odoPhi += 2.0 * M_PI;
	}

	Q_EMIT odometry( _odoX, _odoY, _odoPhi, _vX, _vY, _vOmega, _seq );
	++_seq;
}
