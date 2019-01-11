#include "Motor.h"
#include "Client.h"

extern Client* client;

Motor::Motor( unsigned int id )
: _id( id )
, _mode( FreeMode )
, _setPoint( 0.0f )
, _speed( 0.0f )
, _position( 0 )
, _current( 10 )
, _maxcurrent( 0.0f )
, _exceedtime( 0.0f )
, _overCurrentTimer( new QTimer( this ) )
, _setPointAtOverCurrent( 0.0f )
, _isOverCurrentDetected( false )
{
	_overCurrentTimer->setSingleShot( true );

	bool connected = true;
	connected &= (bool)connect( client, SIGNAL( is_connected() ), SLOT( on_client_connected() ) );
	connected &= (bool)connect( _overCurrentTimer, SIGNAL( timeout() ), SLOT( on_overCurrentTimer_timeout() ) );
	Q_ASSERT( connected );
}

void Motor::set_motor_mode( const QString& mode )
{
	Mode_t newMode;

	if( "vel" == mode )
	{
		newMode = VelMode;
		_maxcurrent = _maxcurrentFromConfig;
	}
	else if( "pos" == mode )
	{
		newMode = PosMode;
		_maxcurrent = _maxcurrentFromConfig;
	}
	else if( "free" == mode )
	{
		newMode = FreeMode;
		_maxcurrent = _maxcurrentFromConfig;
	}
	else if( "gripper" == mode )
	{
		newMode = GripperMode;
		_maxcurrent = 0;
	}
	else
	{
		Q_EMIT log( QString( "Unknown motor mode %1" ).arg( mode ), 1 );
		return;
	}

	if( newMode != _mode )
	{
		Q_EMIT log( QString( "Changing motor %1 to mode %2" ).arg( _id+1 ).arg( mode ), 1 );
		_mode = newMode;
		Q_EMIT motor_mode_changed( mode );
	}
}

void Motor::set_motor_readings( const QVector< float >& speeds, const QVector< int >& positions, const QVector< float >& currents, float time_delta )
{
	Q_ASSERT( 4 == speeds.size() );
	Q_ASSERT( 4 == positions.size() );
	Q_ASSERT( 4 == currents.size() );

	_speed = speeds[ _id ];
	_position = positions[ _id ];

	_current.add( currents[ _id ] );

	Q_EMIT log( QString( "motor %1 mean current %2" ).arg( _id+1 ).arg( _current.mean() ), 3 );

	if( fabs( _current.mean() ) > _maxcurrent && _setPoint != 0.0f )
	{
		if( false == _overCurrentTimer->isActive() )
		{
			_overCurrentTimer->start();
		}
	}
	else
	{
		_overCurrentTimer->stop();
	}
}

void Motor::set_motor_setpoint( float speed, const rec::rpc::ClientInfo& )
{

	switch( _mode )
	{
	case VelMode:
		Q_EMIT piControllerEnabledChanged(_id,true);
	
		_setPoint = speed;
		break;

	case PosMode:
		Q_EMIT log( QString( "PosMode not implemented yet" ), 1 );
		_setPoint = 0;
		break;

	case GripperMode:
		Q_EMIT piControllerEnabledChanged(_id,false);
	
		if( speed > 0 )
		{
			speed = 255;
		}
		else if( speed < 0 )
		{
			speed = -255;
		}

		/*
		Gripper state:
		0 - Moving
		1 - Open
		2 - Closed
		*/
		{
			int state = 0;
			if( _isOverCurrentDetected )
			{
				state = ( _setPointAtOverCurrent > 0 ? 1 : 2 );
			}
			
			Q_EMIT gripper_state_changed( state );
		}

	case FreeMode:
		Q_EMIT log( QString( "motor %1 free mode" ).arg( _id+1 ), 2 );
		Q_EMIT piControllerEnabledChanged(_id,false);
		if( _isOverCurrentDetected )
		{
			Q_EMIT log( QString( "motor %1 over current" ).arg( _id+1 ), 2 );

			bool isNewSetpointSmaller = false;

			if( _setPointAtOverCurrent > 0 && speed < _setPointAtOverCurrent )
			{
				isNewSetpointSmaller = true;
			}
			else if( _setPointAtOverCurrent < 0 && speed > _setPointAtOverCurrent )
			{
				isNewSetpointSmaller = true;
			}

			if( isNewSetpointSmaller )
			{
				_isOverCurrentDetected = false;
				_setPoint = speed;
				Q_EMIT log( QString( "Over current cleared for motor %1" ).arg( _id+1 ), 2 );
			}
			else
			{
				_setPoint = 0;
				Q_EMIT log( QString( "Reduced set point of motor %1 to %2" ).arg( _id+1 ).arg( _setPoint ), 3 );
			}
		}
		else
		{
			_setPoint = speed;
		}
		break;
	}

	Q_EMIT motor_setpoint_changed( _setPoint );
}

void Motor::on_client_connected()
{
	QString str = client->getParameter( QString( "~/Motor%1/mode" ).arg( _id ), "free" ).toString();
	if( "vel" == str )
	{
		_mode = VelMode;
		Q_EMIT log( QString("Motor%1 VelMode").arg(_id+1) , 0 );
	}
	else if( "pos" == str )
	{
		_mode = PosMode;
		Q_EMIT log( QString("Motor%1 PosMode").arg(_id+1) , 0 );
	}
	else if( "free" == str )
	{
		_mode = FreeMode;
		Q_EMIT log( QString("Motor%1 FreeMode").arg(_id+1) , 0 );
	}
	else
	{
		Q_EMIT log( QString( "Unknown motor mode %1" ).arg( str ), 1 );
	}

	_maxcurrentFromConfig = client->getParameter( QString( "~/Motor%1/maxcurrent" ).arg( _id ), 1 ).toDouble();
	_maxcurrent = _maxcurrentFromConfig;

	_exceedtime = client->getParameter( QString( "~/Motor%1/exceedtime" ).arg( _id ), 400 ).toUInt();

	_overCurrentTimer->setInterval( _exceedtime );
}

void Motor::on_overCurrentTimer_timeout()
{
	_isOverCurrentDetected = true;
	_setPointAtOverCurrent = _setPoint;

	Q_EMIT log( QString( "Over current detected for motor %1" ).arg( _id+1 ), 1 );
	Q_EMIT motor_setpoint_changed( 0 );
}
