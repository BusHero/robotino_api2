#include "RobotinoClient.h"

RobotinoClient* RobotinoClient::_impl = NULL;

void RobotinoClient::init()
{
	_impl = new RobotinoClient;
}

void RobotinoClient::done()
{
	delete _impl;
	_impl = NULL;
}

RobotinoClient::RobotinoClient()
{
	setAutoReconnectEnabled( true );
	set_rec_robotino_rpc_motor0_setpoint_enabled(true);
	set_rec_robotino_rpc_motor1_setpoint_enabled(true);
	set_rec_robotino_rpc_motor2_setpoint_enabled(true);
	set_rec_robotino_rpc_motor3_setpoint_enabled(true);
	set_rec_robotino_rpc_omnidrive_enabled(true);
	set_rec_robotino_rpc_set_digital_output_enabled(true);
	set_rec_robotino_rpc_set_digital_output_array_enabled(true);
}
