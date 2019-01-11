#include "Client.h"

Client::Client()
{
	bool connected = true;
	connected &= connect( this, SIGNAL( is_connected() ), SLOT( on_connected() ) );
	connected &= connect( this, SIGNAL( is_disconnected() ), SLOT( on_disconnected() ) );
	Q_ASSERT( connected );

	setAutoReconnectEnabled( true );
	set_rec_robotino_rpc_set_log_level_enabled( true );
	set_rec_robotino_rpc_kinect0_set_tilt_enabled( true );
	set_rec_robotino_rpc_kinect0_set_led_enabled( true );
	set_rec_robotino_rpc_kinect0_set_video_format_enabled( true );
	set_rec_robotino_rpc_kinect0_set_depth_format_enabled( true );
}

void Client::on_connected()
{
}

void Client::on_disconnected()
{
}


