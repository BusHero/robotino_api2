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
	Q_EMIT log(QString("Connected to %1").arg(this->address()), 0);
}

void Client::on_disconnected()
{
	//Q_EMIT log(QString("Disconnected from %1").arg(this->address()), 0);
}

void Client::on_laserscan(unsigned int channel, QVector<float> ranges, QVector<float> intensities, QVariantMap parameters)
{
	if( isConnected() )
	{
		set_scan( channel,
			ranges,
			intensities,
			parameters
			);
	}
}

