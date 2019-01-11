#ifndef _ROBOTINOCLIENT_H_
#define _ROBOTINOCLIENT_H_

#include "rec/robotino/rpc/Client.h"

class RobotinoClient : public rec::robotino::rpc::Client
{
	Q_OBJECT
public:
	static RobotinoClient* singleton()
	{
		Q_ASSERT( _impl );
		return _impl;
	}

	static void init();

	static void done();
	
private slots:
	void on_is_connected();
	void on_is_disconnected();
	
	void on_set_camera_settings_changed( unsigned int width, unsigned int height, QString format );
	void on_set_camera_control_changed( const QString& name, int value );
	void on_image_info_changed( const rec::rpc::ClientInfoSet& );

private:
	RobotinoClient();

	static RobotinoClient* _impl;
};

#endif //_ROBOTINOCLIENT_H_
