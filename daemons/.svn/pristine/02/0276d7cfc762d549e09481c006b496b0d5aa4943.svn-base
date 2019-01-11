#include "Client.h"

Client::Client()
{
	bool connected = true;
	connected &= connect( this, SIGNAL( is_connected() ), SLOT( on_connected() ) );
	connected &= connect( this, SIGNAL( is_disconnected() ), SLOT( on_disconnected() ) );
	Q_ASSERT( connected );

	setAutoReconnectEnabled( true );
}

void Client::on_connected()
{
}

void Client::on_disconnected()
{
}

void Client::on_laserscan( const rec::LaserScanMessage& scan )
{
	if( isConnected() )
	{
		set_scan( scan.header.seq, 
			scan.header.stamp,
			scan.header.frame_id,
			scan.angle_min,
			scan.angle_max,
			scan.angle_increment,
			scan.time_increment,
			scan.scan_time,
			scan.range_min,
			scan.range_max,
			scan.ranges,
			scan.intensities 
			);
	}
}

