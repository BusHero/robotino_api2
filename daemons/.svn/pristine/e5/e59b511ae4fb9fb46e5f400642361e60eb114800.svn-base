#include "KinectImpl.h"
#include "Client.h"

#include "libfreenect/libfreenect.h"

extern Client* client;

freenect_context *f_ctx = 0;
freenect_device *f_dev = 0;
int freenect_angle = 0;
int freenect_led;

uint8_t* rgb_back = NULL;

unsigned int _current_video_format = 255;
unsigned int _current_depth_format = 255;

freenect_resolution _current_resolution = FREENECT_RESOLUTION_LOW;
int _current_width = 320;
int _current_height= 240;

KinectImpl* impl = NULL;

void depth_cb(freenect_device *dev, void *v_depth, uint32_t timestamp)
{
	//FILE* fp = fopen( "test.pgm", "w");
	//if (fp == NULL) {
	//	fprintf(stderr, "Error: Cannot open file [%s]\n", "test.pgm");
	//	exit(1);
	//}
	//printf("%s\n", "test.pgm");
	//fprintf(fp, "P5 %d %d 65535\n", _current_width, _current_height);
	//fwrite( v_depth, _current_width * _current_height * 2, 1, fp);

	QByteArray data( (const char*)v_depth, _current_width*_current_height*2 );
	impl->setDepth( data, timestamp );
}

void rgb_cb(freenect_device *dev, void *rgb, uint32_t timestamp)
{
	QByteArray data( (const char*)rgb, _current_width*_current_height*3 );
	impl->setVideo( data, timestamp );
}

KinectImpl::KinectImpl( unsigned int device, unsigned int channel )
: _device( device )
, _channel( channel )
, _timer( new QTimer( this ) )
, _depthMinTime( 1000 )
{
	impl = this;

	bool connected = true;

	connected &= connect( _timer, SIGNAL( timeout() ), SLOT( on_timer_timeout() ) );
	Q_ASSERT( connected );

	connected &= connect( client, SIGNAL( kinect0_set_tilt_changed( double ) ), SLOT( on_client_kinect0_set_tilt_changed( double ) ) );
	Q_ASSERT( connected );

	connected &= connect( client, SIGNAL( kinect0_set_led_changed( unsigned int ) ), SLOT( on_client_kinect0_set_led_changed( unsigned int ) ) );
	Q_ASSERT( connected );

	connected &= connect( client, SIGNAL( kinect0_set_video_format_changed( unsigned int ) ), SLOT( on_client_kinect0_set_video_format_changed( unsigned int ) ) );
	Q_ASSERT( connected );

	connected &= connect( client, SIGNAL( kinect0_set_depth_format_changed( unsigned int ) ), SLOT( on_client_kinect0_set_depth_format_changed( unsigned int ) ) );
	Q_ASSERT( connected );

	_timer->setSingleShot( false );
	_timer->setInterval( 0 );

	_depthTime.start();
}

KinectImpl::~KinectImpl()
{
	if( f_dev )
	{
		freenect_stop_depth(f_dev);
		freenect_stop_video(f_dev);
		freenect_close_device(f_dev);
	}

	if( f_ctx )
	{
		freenect_shutdown(f_ctx);
	}

	free(rgb_back);
}

bool KinectImpl::start()
{
	if (freenect_init(&f_ctx, NULL) < 0)
	{
		Q_EMIT log("freenect_init() failed", 0 );
		return false;
	}

	//freenect_set_log_level(f_ctx, FREENECT_LOG_DEBUG);
	freenect_select_subdevices(f_ctx, (freenect_device_flags)(FREENECT_DEVICE_MOTOR | FREENECT_DEVICE_CAMERA));
	//freenect_select_subdevices(f_ctx, (freenect_device_flags)(FREENECT_DEVICE_MOTOR));

	int nr_devices = freenect_num_devices (f_ctx);
	Q_EMIT log( QString( "Number of devices found: %1" ).arg( nr_devices ), 0 );

	if (nr_devices < 1)
	{
		return false;
	}

	if( _device+1 > nr_devices )
	{
		return false;
	}

	if (freenect_open_device(f_ctx, &f_dev, _device) < 0)
	{
		Q_EMIT log( "Could not open device\n", 0 );
		return false;
	}

	QString resStr = client->getParameter( "~/DEPTH/resolution", "medium" ).toString();
	if( "low" == resStr )
	{
		_current_resolution = FREENECT_RESOLUTION_LOW;
		_current_width = 320;
		_current_height= 240;
	}
	else if( "medium" == resStr )
	{
		_current_resolution = FREENECT_RESOLUTION_MEDIUM;
		_current_width = 640;
		_current_height= 480;
	}
	else if( "high" == resStr )
	{
		_current_resolution = FREENECT_RESOLUTION_HIGH;
		_current_width = 1280;
		_current_height= 1024;
	}

	rgb_back = (uint8_t*)malloc(_current_width*_current_height*3);

	freenect_set_tilt_degs(f_dev,0);
	freenect_set_led(f_dev,LED_RED);
	freenect_set_depth_callback(f_dev, depth_cb);
	freenect_set_video_callback(f_dev, rgb_cb);
	freenect_set_video_buffer(f_dev, rgb_back);

	on_client_kinect0_set_depth_format_changed( client->getParameter( "~/DEPTH/mode", 0 ).toUInt() );

	if( client->getParameter( "~/VIDEO/mode", -1 ).toInt() >= 0 )
	{
		on_client_kinect0_set_video_format_changed( client->getParameter( "~/VIDEO/mode", 0 ).toUInt() );
	}

	float depthHz = client->getParameter( "~/DEPTH/hz", 1.0f ).toDouble();
	if( depthHz > 0.0f )
	{
		_depthMinTime = static_cast<int>( 1000.0f / depthHz );
	}
	//freenect_start_depth(f_dev);
	//freenect_start_video(f_dev);

	_timer->start();

	return true;
}

void KinectImpl::setDepth( const QByteArray& data, unsigned int stamp )
{
	if( _depthTime.elapsed() < _depthMinTime )
	{
		return;
	}

	_depthTime.start();

	Q_EMIT log( QString( "depth %1" ).arg( data.size() ), 2 );
	client->set_kinect0_depth( data, QByteArray(), _current_width, _current_height, _current_depth_format, stamp );
}

void KinectImpl::setVideo( const QByteArray& data, unsigned int stamp )
{
	Q_EMIT log( QString( "video %1" ).arg( data.size() ), 2 );
	client->set_kinect0_video( data, _current_width, _current_height, _current_video_format, stamp );
}

void KinectImpl::on_client_kinect0_set_tilt_changed( double tilt )
{
	if( tilt > 30.0 )
	{
		tilt = 30.0;
	}
	else if( tilt < -30.0 )
	{
		tilt = -30.0;
	}
	freenect_set_tilt_degs(f_dev,tilt);
}

void KinectImpl::on_client_kinect0_set_led_changed( unsigned int state )
{
	switch( state )
	{
	case 1:
		freenect_set_led(f_dev,LED_GREEN);
		break;
	
	case 2:
		freenect_set_led(f_dev,LED_RED);
		break;
	
	case 3:
		freenect_set_led(f_dev,LED_YELLOW);
		break;
	
	case 4:
		freenect_set_led(f_dev,LED_BLINK_GREEN);
		break;
	
	case 5:
		freenect_set_led(f_dev,LED_BLINK_GREEN);
		break;
	
	case 6:
		freenect_set_led(f_dev,LED_BLINK_RED_YELLOW);
		break;
	
	default:
		freenect_set_led(f_dev,LED_OFF);
		break;
	}
}

void KinectImpl::on_client_kinect0_set_video_format_changed( unsigned int format )
{
	if( format == _current_video_format )
	{
		Q_EMIT log( "format already in use", 1 );
		return;
	}

	freenect_stop_video(f_dev);

	switch( format )
	{
	case 0:
		freenect_set_video_mode(f_dev, freenect_find_video_mode(_current_resolution, FREENECT_VIDEO_RGB));
		_current_video_format = format;
		Q_EMIT log( "new video format: RGB", 1 );
		break;

	case 1:
		freenect_set_video_mode(f_dev, freenect_find_video_mode(_current_resolution, FREENECT_VIDEO_BAYER));
		_current_video_format = format;
		Q_EMIT log( "new video format: BAYER", 1 );
		break;

	case 2:
		freenect_set_video_mode(f_dev, freenect_find_video_mode(_current_resolution, FREENECT_VIDEO_IR_8BIT));
		_current_video_format = format;
		Q_EMIT log( "new video format: IR_8BIT", 1 );
		break;

	case 3:
		freenect_set_video_mode(f_dev, freenect_find_video_mode(_current_resolution, FREENECT_VIDEO_IR_10BIT));
		_current_video_format = format;
		Q_EMIT log( "new video format: IR_10BIT", 1 );
		break;

	case 4:
		freenect_set_video_mode(f_dev, freenect_find_video_mode(_current_resolution, FREENECT_VIDEO_IR_10BIT_PACKED));
		_current_video_format = format;
		Q_EMIT log( "new video format: 10BIT_PACKED", 1 );
		break;

	case 5:
		freenect_set_video_mode(f_dev, freenect_find_video_mode(_current_resolution, FREENECT_VIDEO_YUV_RGB));
		_current_video_format = format;
		Q_EMIT log( "new video format: YUV_RGB", 1 );
		break;

	case 6:
		freenect_set_video_mode(f_dev, freenect_find_video_mode(_current_resolution, FREENECT_VIDEO_YUV_RAW));
		_current_video_format = format;
		Q_EMIT log( "new video format: YUV_RAW", 1 );
		break;

	default:
		Q_EMIT log( QString( "unknown video format: %1" ).arg( format ), 1 );
		break;
	}

	freenect_start_video(f_dev);
}

void KinectImpl::on_client_kinect0_set_depth_format_changed( unsigned int format )
{
	if( format == _current_depth_format )
	{
		Q_EMIT log( "format already in use", 1 );
		return;
	}

	freenect_stop_depth(f_dev);

	switch( format )
	{
	case 0:
		freenect_set_depth_mode(f_dev, freenect_find_depth_mode(_current_resolution, FREENECT_DEPTH_11BIT));
		_current_depth_format = format;
		Q_EMIT log( "new depth format: 11BIT", 1 );
		break;

	case 1:
		freenect_set_depth_mode(f_dev, freenect_find_depth_mode(_current_resolution, FREENECT_DEPTH_10BIT));
		_current_depth_format = format;
		Q_EMIT log( "new depth format: 10BIT", 1 );
		break;

	case 2:
		freenect_set_depth_mode(f_dev, freenect_find_depth_mode(_current_resolution, FREENECT_DEPTH_11BIT_PACKED));
		_current_depth_format = format;
		Q_EMIT log( "new depth format: 11BIT_PACKED", 1 );
		break;

	case 3:
		freenect_set_depth_mode(f_dev, freenect_find_depth_mode(_current_resolution, FREENECT_DEPTH_10BIT_PACKED));
		_current_depth_format = format;
		Q_EMIT log( "new depth format: 10BIT_PACKED", 1 );
		break;

	case 4:
		freenect_set_depth_mode(f_dev, freenect_find_depth_mode(_current_resolution, FREENECT_DEPTH_REGISTERED));
		_current_depth_format = format;
		Q_EMIT log( "new depth format: REGISTERED", 1 );
		break;

	case 5:
		freenect_set_depth_mode(f_dev, freenect_find_depth_mode(_current_resolution, FREENECT_DEPTH_MM));
		_current_depth_format = format;
		Q_EMIT log( "new depth format: MM", 1 );
		break;

	default:
		Q_EMIT log( QString( "unknown depth format: %1" ).arg( format ), 1 );
		break;
	}

	freenect_start_depth(f_dev);
}

void KinectImpl::on_timer_timeout()
{
	if( freenect_process_events(f_ctx) < 0 )
	{
		Q_EMIT error();
	}
	else
	{
		freenect_update_tilt_state(f_dev);
		freenect_raw_tilt_state* state = freenect_get_tilt_state(f_dev);
		
		double deg = freenect_get_tilt_degs( state );

		client->set_kinect0_tilt( deg );

		double dx,dy,dz;
		freenect_get_mks_accel(state, &dx, &dy, &dz);

		client->set_kinect0_accel( dx, dy, dz );

		Q_EMIT success();
	}
}
