#include "Client.h"

Client::Client()
{
	bool connected = true;
	connected &= connect( this, SIGNAL( set_emergency_bumper_changed( bool ) ), SLOT( on_set_emergency_bumper_changed( bool ) ) );
	connected &= connect( this, SIGNAL( set_ea09_ip4address_changed( const QHostAddress&, const QHostAddress& ) ), SLOT( on_set_ea09_ip4address_changed( const QHostAddress&, const QHostAddress& ) ) );
	Q_ASSERT( connected );

	setAutoReconnectEnabled( true );
	set_rec_robotino_rpc_omnidrive_enabled( true );
	//set_rec_robotino_rpc_gyroscope_enabled( true ); //using SHM now
	set_rec_robotino_rpc_set_shutdown_enabled( true );
	set_rec_robotino_rpc_motor0_setpoint_enabled( true );
	set_rec_robotino_rpc_motor1_setpoint_enabled( true );
	set_rec_robotino_rpc_motor2_setpoint_enabled( true );
	set_rec_robotino_rpc_motor3_setpoint_enabled( true );
	set_rec_robotino_rpc_motor_setpoints_enabled( true );
	set_rec_robotino_rpc_set_emergency_bumper_enabled( true );
	set_rec_robotino_rpc_set_ea09_ip4address_enabled( true );
	set_rec_robotino_rpc_set_motor0_mode_enabled( true );
	set_rec_robotino_rpc_set_motor1_mode_enabled( true );
	set_rec_robotino_rpc_set_motor2_mode_enabled( true );
	set_rec_robotino_rpc_set_motor3_mode_enabled( true );
	set_rec_robotino_rpc_set_pid_parameters_enabled( true );
	set_rec_robotino_rpc_set_log_level_enabled( true );
	set_rec_robotino_rpc_set_odometry_enabled( true );
	set_rec_robotino_rpc_set_digital_output_enabled( true );
	set_rec_robotino_rpc_set_digital_output_array_enabled( true );
	set_rec_robotino_rpc_set_relay_enabled( true );
	set_rec_robotino_rpc_set_relay_array_enabled( true );
}

void Client::on_set_emergency_bumper_changed( bool on )
{
	setParameter( "~/Emergency/bumper", on );
}

void Client::on_set_ea09_ip4address_changed( const QHostAddress& address, const QHostAddress& netmask )
{
	//Q_EMIT log( QString( "on_set_ea09_ip4address_changed %1 %2" ).arg( address.toString() ).arg( netmask.toString() ), 0 );
	setParameter( "~/EA09/address", address.toString() );
	setParameter( "~/EA09/netmask", netmask.toString() );

	Q_EMIT setEA09Address( address, netmask );
}
