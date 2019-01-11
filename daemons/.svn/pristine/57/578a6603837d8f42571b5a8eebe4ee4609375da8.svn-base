#ifndef _KINECTIMPL_H_
#define _KINECTIMPL_H_

#include <QtCore>

class KinectImpl : public QObject
{
	Q_OBJECT
public:
	KinectImpl( unsigned int device, unsigned int channel );
	~KinectImpl();

	bool start();

	void setDepth( const QByteArray& data, unsigned int stamp );
	void setVideo( const QByteArray& data, unsigned int stamp );

Q_SIGNALS:
	void log( const QString& message, int verbosity );
	void error();
	void success();

private Q_SLOTS:
	void on_timer_timeout();

	void on_client_kinect0_set_tilt_changed( double tilt );
	void on_client_kinect0_set_led_changed( unsigned int state );
	void on_client_kinect0_set_video_format_changed( unsigned int format );
	void on_client_kinect0_set_depth_format_changed( unsigned int format );

private:
	unsigned int _device;
	unsigned int _channel;
	QTimer* _timer;

	int _depthMinTime;
	QTime _depthTime;
};

#endif //_KINECTIMPL_H_
