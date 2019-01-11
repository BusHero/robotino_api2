#include "usb_cam/Camera_linux.h"

#include <stdio.h>

Camera_Linux::Camera_Linux()
: _video_device_name( "/dev/video0" )
, _io_method_name( "mmap" )
, _io_method( IO_METHOD_MMAP )
, _pixel_format_name( "mjpg" )
, _pixel_format( PIXEL_FORMAT_MJPEG )
, _image_width( 320 )
, _image_height( 240 )
, _camera_image( NULL )
{
}

Camera_Linux::~Camera_Linux()
{
	stop();
}

void Camera_Linux::setVideoDevice( const QString& name )
{
	_video_device_name = name;
}

// possible values: mmap, read, userptr
void Camera_Linux::setIOMethod( const QString& name )
{
	QString n = name.toLower();
	if(n == "mmap")
	{
		_io_method = IO_METHOD_MMAP;
	}
	else if(n == "read")
	{
		_io_method = IO_METHOD_READ;
	}
	else if(n == "userptr")
	{
		_io_method = IO_METHOD_USERPTR;
	}
	else
	{
		_io_method_name = "invalid";
		throw CameraException( "Unknown io method." );
	}

	_io_method_name = n;
}

void Camera_Linux::setResolution( int width, int height )
{
	_image_width = width;
	_image_height = height;
}

void Camera_Linux::resolution( int* width, int* height )
{
	*width = _image_width;
	*height = _image_height;
}

// possible values: yuyv, uyvy, mjpeg
void Camera_Linux::setFormat( const QString& format )
{
	QString f = format.toLower();

	if(f == "raw")
	{
		_pixel_format = PIXEL_FORMAT_RAW;
	}
	else if( f == "mjpg" || f == "mjpeg" )
	{
		f = "mjpg";
		_pixel_format = PIXEL_FORMAT_MJPEG;
	}
	else {
		_pixel_format_name = "invalid";
		throw CameraException( "Unknown pixel format." );
	}

	_pixel_format_name = f;
}

void Camera_Linux::setBrightness( int value )
{
	usb_cam_camera_set_brightness( value );
}

void Camera_Linux::setContrast( int value )
{
	usb_cam_camera_set_contrast( value );
}

void Camera_Linux::setSaturation( int value )
{
	usb_cam_camera_set_saturation( value );
}

void Camera_Linux::setAutoWhiteBalanceEnabled( bool enable )
{
	usb_camera_set_auto_white_balance( enable );
}

void Camera_Linux::setGain( int value )
{
	usb_camera_set_gain( value );
}

void Camera_Linux::setWhiteBalanceTemperature( int value )
{
	usb_camera_set_white_balance_temperature( value );
}

void Camera_Linux::setBacklightCompensation( int value )
{
	usb_camera_set_backlight_compensation( value );
}

void Camera_Linux::setAutoExposureEnabled( bool enable )
{
	if( enable )
	{
		usb_camera_set_auto_exposure_mode();
	}
	else
	{
		usb_camera_set_manual_exposure_mode();
	}
}

void Camera_Linux::setExposure( int value )
{
	usb_camera_set_exposure( value );
}

void Camera_Linux::setAutoFocusEnabled( bool enable )
{
	usb_camera_set_auto_focus_enabled( enable );
}

void Camera_Linux::setFocus( int focus )
{
	usb_camera_set_focus( focus );
}

void Camera_Linux::setSharpness( int value )
{
	usb_camera_set_sharpness( value );
}

void Camera_Linux::start()
{
	QString str;
	str += QString( "video_device set to [%1]\n" ).arg( _video_device_name );
	str += QString( "io_method set to [%1]\n" ).arg( _io_method_name );
	str += QString( "image_width set to [%1]\n" ).arg( _image_width );
	str += QString( "image_height set to [%1]\n" ).arg( _image_height );
	str += QString( "pixel_format set to [%1]" ).arg( _pixel_format_name );

	Q_EMIT log( str, 1 );

	_camera_image = usb_cam_camera_start( _video_device_name.toStdString().c_str(),
		_io_method,
		_pixel_format,
		_image_width,
		_image_height
		);

	if( NULL == _camera_image )
	{
		throw CameraException( "usb_cam_camera_start failed." );
	}
}

void Camera_Linux::stop()
{
	usb_cam_camera_shutdown();
	_camera_image = NULL;
}

QByteArray Camera_Linux::grab( unsigned int* width, unsigned int* height, unsigned int* bytes_per_pixel, QString* format )
{
	if( NULL == _camera_image )
	{
		throw CameraException( "not started yet" );
	}

	if( false == usb_cam_camera_grab_image( _camera_image ) )
	{
		throw CameraException( "error grabbing image" );
	}

	QByteArray ba( _camera_image->image, _camera_image->image_size );
	*width = _camera_image->width;
	*height = _camera_image->height;
	*bytes_per_pixel = _camera_image->bytes_per_pixel;
	*format = _camera_image->pixel_format;

	if( "mjpg" != *format )
	{
		if( ba.size() != *width * *bytes_per_pixel * *height )
		{
			Q_EMIT log( QString( "width, height, data size mismatch: w=%1 h=%2 bytesperpixel=%3 size=%4 format=%5" )
				.arg( *width )
				.arg( *height )
				.arg( *bytes_per_pixel )
				.arg( ba.size() )
				.arg( *format ), 1
				);

			ba.clear();
		}
	}

	return ba;
}
