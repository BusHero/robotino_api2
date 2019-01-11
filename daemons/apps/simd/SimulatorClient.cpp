#include "SimulatorClient.h"

SimulatorClient* SimulatorClient::_impl = NULL;

void SimulatorClient::init()
{
	_impl = new SimulatorClient;
}

void SimulatorClient::done()
{
	delete _impl;
	_impl = NULL;
}

SimulatorClient::SimulatorClient()
{
	setAutoReconnectEnabled( true );
	set_rec_robotino_rpc_motor_readings_enabled(true);
	set_rec_robotino_rpc_distance_sensors_enabled(true);
	set_rec_robotino_rpc_analog_input_enabled(true);
	set_rec_robotino_rpc_digital_input_enabled(true);
	set_rec_robotino_rpc_bumper_enabled(true);
	set_rec_robotino_rpc_battery_enabled(true);
	set_rec_robotino_rpc_odometry_enabled(true);
	set_rec_robotino_rpc_scan0_enabled(true);
	set_rec_robotino_rpc_image0_enabled(true);
}
