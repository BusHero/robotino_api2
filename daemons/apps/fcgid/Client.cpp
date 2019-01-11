#include "Client.h"

Client* Client::_impl = NULL;

void Client::init()
{
	_impl = new Client;
}

void Client::done()
{
	delete _impl;
	_impl = NULL;
}

Client::Client()
{
	this->setAutoReconnectEnabled( true, 1000 );
	this->setLocalIPCEnabled( false );
}

void Client::setTopicsEnabled( bool on )
{
	set_rec_robotino_rpc_log_enabled(on);
	set_rec_robotino_rpc_set_log_level_enabled( true );

	set_rec_robotino_rpc_omnidrive_enabled( on );
	set_rec_robotino_rpc_odometry_enabled( on );
	set_rec_robotino_rpc_distance_sensors_enabled( on );
	set_rec_robotino_rpc_bumper_enabled( on );
	set_rec_robotino_rpc_battery_enabled( on );
	set_rec_robotino_rpc_map_enabled( on );
	set_rec_robotino_rpc_poseOnMap_enabled( on );
	set_rec_robotino_rpc_initialPose_enabled( on );
	set_rec_robotino_rpc_navGoal_enabled( on );
	set_rec_robotino_rpc_charger0_info_enabled( on );
	set_rec_robotino_rpc_charger1_info_enabled( on );
	set_rec_robotino_rpc_charger2_info_enabled( on );
    set_rec_robotino_rpc_charger0_version_enabled( on );
    set_rec_robotino_rpc_charger1_version_enabled( on );
    set_rec_robotino_rpc_charger2_version_enabled( on );
    set_rec_robotino_rpc_scan0_enabled( on );
	set_rec_robotino_rpc_fleetcom_response_enabled( on );
    set_rec_robotino_rpc_mclayout_enabled( on );
    set_rec_robotino_rpc_mcstatus_enabled( on );

    set_rec_robotino_rpc_digital_input_enabled( on );
    set_rec_robotino_rpc_analog_input_enabled( on );
    set_rec_robotino_rpc_set_digital_output_enabled( on );
    set_rec_robotino_rpc_set_digital_output_array_enabled( on );
    set_rec_robotino_rpc_set_relay_enabled( on );
    set_rec_robotino_rpc_set_relay_array_enabled( on );
}
