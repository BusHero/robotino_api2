#include "RobotinoClient.h"
#include "Realsense.h"

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
	bool connected;
	
	connected = (bool)connect( this, SIGNAL( set_camera0_settings_changed( unsigned int, unsigned int, const QString& ) ),
		SLOT( on_set_camera_settings_changed( unsigned int, unsigned int, QString ) ) );
	Q_ASSERT( connected );

	connected = (bool)connect( this, SIGNAL( set_camera0_control_changed( const QString&, int ) ),
		SLOT( on_set_camera_control_changed( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected = (bool)connect( this, SIGNAL( image0_info_changed( const rec::rpc::ClientInfoSet& ) ),
		SLOT( on_image_info_changed( const rec::rpc::ClientInfoSet& ) ) );
	Q_ASSERT( connected );
	
	connected = (bool)connect( this, SIGNAL( is_connected() ), SLOT( on_is_connected() ) );
	Q_ASSERT( connected );
	
	connected = (bool)connect( this, SIGNAL( is_disconnected() ), SLOT( on_is_disconnected() ) );
	Q_ASSERT( connected );
	
	setAutoReconnectEnabled( true );
}

void RobotinoClient::on_is_connected()
{
	set_rec_robotino_rpc_image0_info_enabled(true);
	set_rec_robotino_rpc_set_camera0_settings_enabled(true);
	set_rec_robotino_rpc_set_camera0_control_enabled(true);
}

void RobotinoClient::on_is_disconnected()
{
	Realsense::singleton()->stop();
}

void RobotinoClient::on_set_camera_settings_changed( unsigned int width, unsigned int height, QString format )
{
	
}

void RobotinoClient::on_set_camera_control_changed( const QString& name, int value )
{
	
}

void RobotinoClient::on_image_info_changed( const rec::rpc::ClientInfoSet& info )
{
	qDebug() << "on_image_info_changed " << info.size();
	if( 0 == info.size() )
	{
		Realsense::singleton()->stop();
	}
	else
	{
		Realsense::singleton()->start();
	}
}