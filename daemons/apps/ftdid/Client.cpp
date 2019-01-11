#include "Client.h"

Client::Client()
	: _connected( false )
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
	_connected = true;
}

void Client::on_disconnected()
{
	_connected = false;
}

void Client::on_northstar_report( float pose_x,
				float pose_y,
				unsigned short r,
				float pose_theta,
				unsigned short numGoodSpots,
				unsigned short spot0_mag,
				unsigned short spot1_mag,
				unsigned int sequenceNumber,
				int roomReported )
{
	if( _connected )
	{
		set_northstar( sequenceNumber, roomReported, numGoodSpots, pose_x, pose_y, pose_theta, spot0_mag, spot1_mag );
	}
}

void Client::on_cruizcore_report( float angle, float rate )
{
	if( _connected )
	{
		set_gyroscope_ext( angle, rate );
	}
}

