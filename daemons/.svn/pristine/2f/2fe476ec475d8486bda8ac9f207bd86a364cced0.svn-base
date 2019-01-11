#include "Client.h"

Client::Client()
{
	bool connected = true;
	connected &= (bool)connect( this, SIGNAL( is_connected() ), SLOT( on_connected() ) );
	connected &= (bool)connect( this, SIGNAL( is_disconnected() ), SLOT( on_disconnected() ) );
	Q_ASSERT( connected );

	setAutoReconnectEnabled( true );
	set_rec_robotino_rpc_set_log_level_enabled( true );
}

void Client::on_connected()
{
}

void Client::on_disconnected()
{
}

void Client::on_laserscan( unsigned int channel, const rec::LaserScanMessage& scan )
{
	if( isConnected() )
	{
		QVariantMap parameters;
		parameters["seq"] = scan.header.seq;
		parameters["stamp"] = scan.header.stamp;
		parameters["frame_id"] = scan.header.frame_id;
		parameters["angle_min"] = scan.angle_min;
		parameters["angle_min"] = scan.angle_min;
		parameters["angle_max"] = scan.angle_max;
		parameters["angle_increment"] = scan.angle_increment;
		parameters["time_increment"] = scan.time_increment;
		parameters["range_min"] = scan.range_min;
		parameters["range_max"] = scan.range_max;

		set_scan( channel,
			scan.ranges,
			scan.intensities,
			parameters
			);
	}
}

