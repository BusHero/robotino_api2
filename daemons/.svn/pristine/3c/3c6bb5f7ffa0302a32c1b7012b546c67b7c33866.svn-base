#include "Client.h"

Client::Client()
{
	bool connected = true;
	connected &= (bool)QObject::connect( this, SIGNAL( set_shutdown_changed( bool ) ), SLOT( on_set_shutdown_changed( bool ) ) );
	
	Q_ASSERT( connected );

	setAutoReconnectEnabled( true );
	set_rec_robotino_rpc_omnidrive_enabled( true );
	set_rec_robotino_rpc_set_shutdown_enabled( true );
	set_rec_robotino_rpc_motor0_setpoint_enabled( true );
	set_rec_robotino_rpc_motor0_reset_position_enabled( true );
	set_rec_robotino_rpc_motor1_setpoint_enabled( true );
	set_rec_robotino_rpc_motor1_reset_position_enabled( true );
	set_rec_robotino_rpc_motor2_setpoint_enabled( true );
	set_rec_robotino_rpc_motor2_reset_position_enabled( true );
	set_rec_robotino_rpc_motor3_setpoint_enabled( true );
	set_rec_robotino_rpc_motor3_reset_position_enabled( true );
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
	set_rec_robotino_rpc_charger0_clear_error_enabled( true );
	set_rec_robotino_rpc_charger1_clear_error_enabled( true );
	set_rec_robotino_rpc_charger2_clear_error_enabled( true );
	set_rec_robotino_rpc_set_shutdown_enabled( true );
	set_rec_robotino_rpc_start_hotswap_enabled( true );
	set_rec_robotino_rpc_start_motor_debug_enabled( true );
}

void Client::on_set_shutdown_changed( bool shutdown )
{
	if( shutdown )
	{
#ifdef WIN32
		QProcess::startDetached("shutdown", QStringList() << "/s" << "/t" << "1" );
#else
		QProcess::startDetached("sudo", QStringList() << "/usr/local/bin/robotinopoweroff.sh" );
#endif
	}
}
