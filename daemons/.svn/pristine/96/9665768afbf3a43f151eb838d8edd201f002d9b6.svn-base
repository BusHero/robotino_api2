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
	set_rec_robotino_rpc_set_log_level_enabled( true );
}

void Client::setTopicsEnabled( bool on )
{
	set_rec_robotino_rpc_image0_enabled( on );
	if( on )
	{
		set_set_camera0_settings( 320, 240, "raw" );
	}
	set_rec_robotino_rpc_odometry_enabled( on );
	set_rec_robotino_rpc_distance_sensors_enabled( on );
	set_rec_robotino_rpc_bumper_enabled( on );
	set_rec_robotino_rpc_battery_enabled( on );
}