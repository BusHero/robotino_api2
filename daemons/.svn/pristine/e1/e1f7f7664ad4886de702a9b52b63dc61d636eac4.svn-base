#include "Client.h"
#include "action/Action.h"
#include "action/Execute.h"
#include "action/RequestShutdown.h"

Client::Client()
: _emergencyBumper( false )
, _numDisplayTextSubscribers( 0 )
, _numDisplayBacklightSubscribers( 0 )
, _numDisplayVBarSubscribers( 0 )
, _numDisplayHBarSubscribers( 0 )
, _numDisplayProgressSubscribers( 0 )
, _numDisplayClearSubscribers( 0 )
, _emergency_bumper( false )
, _motorTimer( new QTimer( this ) )
, _battery_voltage( 0.0f )
, _system_current( 0.0f )
{
	_motorTimer->setSingleShot( true );
	_motorTimer->setInterval( 1000 );

	bool connected = true;
	connected &= connect( this, SIGNAL( is_connected() ), SLOT( on_connected() ) );
	connected &= connect( this, SIGNAL( is_disconnected() ), SLOT( on_disconnected() ) );
	connected &= connect( this, SIGNAL( ea09_ip4address_changed( const QHostAddress&, const QHostAddress& ) ), SLOT( on_ea09_ip4address_changed( const QHostAddress&, const QHostAddress& ) ) );
	connected &= connect( this, SIGNAL( emergency_bumper_changed( bool ) ), SLOT( on_emergency_bumper_changed( bool ) ) );
	connected &= connect( this, SIGNAL( display_text_info_changed( const rec::rpc::ClientInfoSet& ) ), SLOT( on_display_text_info_changed( const rec::rpc::ClientInfoSet& ) ) );
	connected &= connect( this, SIGNAL( display_backlight_info_changed( const rec::rpc::ClientInfoSet& ) ), SLOT( on_display_backlight_info_changed( const rec::rpc::ClientInfoSet& ) ) );
	connected &= connect( this, SIGNAL( display_vbar_info_changed( const rec::rpc::ClientInfoSet& ) ), SLOT( on_display_vbar_info_changed( const rec::rpc::ClientInfoSet& ) ) );
	connected &= connect( this, SIGNAL( display_hbar_info_changed( const rec::rpc::ClientInfoSet& ) ), SLOT( on_display_hbar_info_changed( const rec::rpc::ClientInfoSet& ) ) );
	connected &= connect( this, SIGNAL( display_progress_info_changed( const rec::rpc::ClientInfoSet& ) ), SLOT( on_display_progress_info_changed( const rec::rpc::ClientInfoSet& ) ) );
	connected &= connect( this, SIGNAL( display_clear_info_changed( const rec::rpc::ClientInfoSet& ) ), SLOT( on_display_clear_info_changed( const rec::rpc::ClientInfoSet& ) ) );
	
	connected &= connect( this, SIGNAL( ea_version_changed( const QString&, quint16, quint16, quint16 ) ), SLOT( on_ea_version_changed( const QString&, quint16, quint16, quint16 ) ) );
	Q_ASSERT( connected );

	connected &= connect( _motorTimer, SIGNAL( timeout() ), SLOT( on_motorTimer_timeout() ) );
	Q_ASSERT( connected );

	connected &= connect( this, SIGNAL( battery_changed( float, float ) ), SLOT( on_battery_changed( float, float ) ) );
	Q_ASSERT( connected );

	setAutoReconnectEnabled( true );
	set_rec_robotino_rpc_display_buttons_enabled( true );
	
	set_rec_robotino_rpc_display_text_info_enabled( true );
	set_rec_robotino_rpc_display_backlight_info_enabled( true );
	set_rec_robotino_rpc_display_vbar_info_enabled( true );
	set_rec_robotino_rpc_display_hbar_info_enabled( true );
	set_rec_robotino_rpc_display_progress_info_enabled( true );
	set_rec_robotino_rpc_display_clear_info_enabled( true );

	set_rec_robotino_rpc_ea09_ip4address_enabled( true );
	set_rec_robotino_rpc_emergency_bumper_enabled( true );
	set_rec_robotino_rpc_ea_version_enabled( true );
	set_rec_robotino_rpc_power_button_enabled( true );
	set_rec_robotino_rpc_request_shutdown_enabled( true );

	set_rec_robotino_rpc_motor0_setpoint_enabled( true );
	set_rec_robotino_rpc_motor1_setpoint_enabled( true );
	set_rec_robotino_rpc_motor2_setpoint_enabled( true );
	set_rec_robotino_rpc_motor3_setpoint_enabled( true );
	set_rec_robotino_rpc_motor_setpoints_enabled( true );
	set_rec_robotino_rpc_omnidrive_enabled( true );
	set_rec_robotino_rpc_api1_setstate_received_enabled( true );

	set_rec_robotino_rpc_battery_enabled( true );
	set_rec_robotino_rpc_set_log_level_enabled( true );
}

void Client::disconnectMotorDriver()
{
	if( false == _motorDriverInfo.isNull() )
	{
		disconnectClient( _motorDriverInfo.address, _motorDriverInfo.port );
	}

	set_disconnect_api1_clients();
}

void Client::on_connected()
{
}

void Client::on_disconnected()
{
	_numDisplayTextSubscribers = 0;
	_numDisplayBacklightSubscribers = 0;
	_numDisplayVBarSubscribers = 0;
	_numDisplayHBarSubscribers = 0;
	_numDisplayProgressSubscribers = 0;
	_numDisplayClearSubscribers = 0;
}

void Client::on_ea09_ip4address_changed( const QHostAddress& address, const QHostAddress& netmask )
{
	_ea09_ip4address = address;
	_ea09_ip4netmask = netmask;

	Q_EMIT my_ea09_ip4address_changed( address, netmask );
}

void Client::on_emergency_bumper_changed( bool on )
{
	_emergency_bumper = on;
}

void Client::on_display_text_info_changed( const rec::rpc::ClientInfoSet& info )
{
	Q_EMIT log( QString( "on_display_text_info_changed %1" ).arg( info.size() ), 0 );
	bool before = checkDisplaySubscribers();
	_numDisplayTextSubscribers = info.size();
	activateDisplay( before );	
}

void Client::on_display_backlight_info_changed( const rec::rpc::ClientInfoSet& info )
{
	Q_EMIT log( QString( "on_display_backlight_info_changed %1" ).arg( info.size() ), 2 );
	bool before = checkDisplaySubscribers();
	_numDisplayBacklightSubscribers = info.size();
	activateDisplay( before );
}

void Client::on_display_vbar_info_changed( const rec::rpc::ClientInfoSet& info )
{
	Q_EMIT log( QString( "on_display_vbar_info_changed %1" ).arg( info.size() ), 2 );
	bool before = checkDisplaySubscribers();
	_numDisplayVBarSubscribers = info.size();
	activateDisplay( before );
}

void Client::on_display_hbar_info_changed( const rec::rpc::ClientInfoSet& info )
{
	Q_EMIT log( QString( "on_display_hbar_info_changed %1" ).arg( info.size() ), 2 );
	bool before = checkDisplaySubscribers();
	_numDisplayHBarSubscribers = info.size();
	activateDisplay( before );
}

void Client::on_display_progress_info_changed( const rec::rpc::ClientInfoSet& info )
{
	Q_EMIT log( QString( "on_display_progress_info_changed %1" ).arg( info.size() ), 2 );
	bool before = checkDisplaySubscribers();
	_numDisplayProgressSubscribers = info.size();
	activateDisplay( before );
}

void Client::on_display_clear_info_changed( const rec::rpc::ClientInfoSet& info )
{
	Q_EMIT log( QString( "on_display_clear_info_changed %1" ).arg( info.size() ), 2 );
	bool before = checkDisplaySubscribers();
	_numDisplayClearSubscribers = info.size();
	activateDisplay( before );
}

void Client::on_ea_version_changed( const QString& board, quint16 firmware_major, quint16 firmware_minor, quint16 firmware_patch )
{
	action::Action::setCondition( board, true );

	if( "ea05" == board || "ea09" == board )
	{
		action::Action::setCondition( "allnet", true );
	}

	if( "ea11" == board )
	{
		action::Action::setCondition( "ath9k", true );
	}
}

void Client::restartMotorTimer()
{
	action::Action::activate( action::Action::controlActivatedAction() );

	_motorTimer->start();
}

void Client::on_motor_setpoint_changed( float speed, const rec::rpc::ClientInfo& info )
{
	_motorDriverStr = info.address.toString() + " " + info.name;
	_motorDriverInfo = info;

	restartMotorTimer();
}

void Client::on_motor_setpoints_changed( float, float, float, const rec::rpc::ClientInfo& info )
{
	on_motor_setpoint_changed( 0.0f, info );
}

void Client::on_omnidrive_changed( float vx, float vy, float omega, const rec::rpc::ClientInfo& info )
{
	on_motor_setpoint_changed( 0.0f, info );
}

void Client::on_motorTimer_timeout()
{
	action::Action::activate( action::Action::rootAction() );
}

void Client::on_battery_changed( float battery_voltage, float system_current )
{
	_battery_voltage = battery_voltage;
	_system_current = system_current;
}

void Client::on_api1_setstate_received_changed()
{
	_motorDriverStr = "API1 connection";
	_motorDriverInfo.clear();

	restartMotorTimer();
}

bool Client::checkDisplaySubscribers() const
{
	if( _numDisplayTextSubscribers > 0
		&& _numDisplayBacklightSubscribers > 0
		&& _numDisplayVBarSubscribers > 0
		&& _numDisplayHBarSubscribers > 0
		&& _numDisplayProgressSubscribers > 0
		&& _numDisplayClearSubscribers > 0 )
	{
		return true;
	}
	return false;
}

void Client::activateDisplay( bool before )
{
	if( false == before && checkDisplaySubscribers() )
	{
		action::Action::activate( action::Action::rootAction(), true );

		//set_controller_active_enabled( true );
	}
}

void Client::set_controller_active_enabled( bool enable )
{
	if( enable )
	{
		bool success = true;
		success &= connect( this, SIGNAL( motor0_setpoint_changed( float, const rec::rpc::ClientInfo& ) ), SLOT( on_motor_setpoint_changed( float, const rec::rpc::ClientInfo& ) ) );
		Q_ASSERT( success );

		success &= connect( this, SIGNAL( motor1_setpoint_changed( float, const rec::rpc::ClientInfo& ) ), SLOT( on_motor_setpoint_changed( float, const rec::rpc::ClientInfo& ) ) );
		Q_ASSERT( success );

		success &= connect( this, SIGNAL( motor2_setpoint_changed( float, const rec::rpc::ClientInfo& ) ), SLOT( on_motor_setpoint_changed( float, const rec::rpc::ClientInfo& ) ) );
		Q_ASSERT( success );

		success &= connect( this, SIGNAL( motor3_setpoint_changed( float, const rec::rpc::ClientInfo& ) ), SLOT( on_motor_setpoint_changed( float, const rec::rpc::ClientInfo& ) ) );
		Q_ASSERT( success );

		success &= connect( this, SIGNAL( motor_setpoints_changed( float, float, float, const rec::rpc::ClientInfo& ) ), SLOT( on_motor_setpoints_changed( float, float, float, const rec::rpc::ClientInfo& ) ) );
		Q_ASSERT( success );

		success &= connect( this, SIGNAL( omnidrive_changed( float, float, float, const rec::rpc::ClientInfo& ) ), SLOT( on_omnidrive_changed( float, float, float, const rec::rpc::ClientInfo& ) ) );
		Q_ASSERT( success );

		success &= connect( this, SIGNAL( api1_setstate_received_changed() ), SLOT( on_api1_setstate_received_changed() ) );
		Q_ASSERT( success );
	}
	else
	{
		bool success = true;
		success &= disconnect( this, SIGNAL( motor0_setpoint_changed( float, const rec::rpc::ClientInfo& ) ), this, SLOT( on_motor_setpoint_changed( float, const rec::rpc::ClientInfo& ) ) );
		success &= disconnect( this, SIGNAL( motor1_setpoint_changed( float, const rec::rpc::ClientInfo& ) ), this, SLOT( on_motor_setpoint_changed( float, const rec::rpc::ClientInfo& ) ) );
		success &= disconnect( this, SIGNAL( motor2_setpoint_changed( float, const rec::rpc::ClientInfo& ) ), this, SLOT( on_motor_setpoint_changed( float, const rec::rpc::ClientInfo& ) ) );
		success &= disconnect( this, SIGNAL( motor3_setpoint_changed( float, const rec::rpc::ClientInfo& ) ), this, SLOT( on_motor_setpoint_changed( float, const rec::rpc::ClientInfo& ) ) );
		success &= disconnect( this, SIGNAL( motor_setpoints_changed( float, float, float, const rec::rpc::ClientInfo& ) ), this, SLOT( on_motor_setpoints_changed( float, float, float, const rec::rpc::ClientInfo& ) ) );
		success &= disconnect( this, SIGNAL( omnidrive_changed( float, float, float, const rec::rpc::ClientInfo& ) ), this, SLOT( on_omnidrive_changed( float, float, float, const rec::rpc::ClientInfo& ) ) );
		success &= disconnect( this, SIGNAL( api1_setstate_received_changed() ), this, SLOT( on_api1_setstate_received_changed() ) );
	}
}
