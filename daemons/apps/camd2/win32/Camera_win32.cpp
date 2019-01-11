#include "Camera_win32.h"

#include <QtGui>

#include <stdio.h>

Camera_Win32::Camera_Win32()
	: _width( 320 )
	, _height( 320 )
{
}

Camera_Win32::~Camera_Win32()
{
	stop();
}

void Camera_Win32::setVideoDevice( const QString& name )
{
}

// possible values: mmap, read, userptr
void Camera_Win32::setIOMethod( const QString& name )
{
}

void Camera_Win32::setResolution( int width, int height )
{
	_width = width;
	_height= height;
}

void Camera_Win32::resolution( int* width, int* height )
{
}

// possible values: yuyv, uyvy, mjpeg
void Camera_Win32::setFormat( const QString& format )
{
}

void Camera_Win32::setBrightness( int value )
{
}

void Camera_Win32::setContrast( int value )
{
}

void Camera_Win32::setSaturation( int value )
{
}

void Camera_Win32::setAutoWhiteBalanceEnabled( bool enable )
{
}

void Camera_Win32::setGain( int value )
{
}

void Camera_Win32::setWhiteBalanceTemperature( int value )
{
}

void Camera_Win32::setBacklightCompensation( int value )
{
}

void Camera_Win32::setAutoExposureEnabled( bool enable )
{
}

void Camera_Win32::setExposure( int value )
{
}

void Camera_Win32::setAutoFocusEnabled( bool enable )
{
}

void Camera_Win32::setFocus( int focus )
{
}

void Camera_Win32::setSharpness( int value )
{
}

void Camera_Win32::start()
{
}

void Camera_Win32::stop()
{
}

QByteArray Camera_Win32::grab( unsigned int* width, unsigned int* height, unsigned int* bytes_per_pixel, QString* format )
{
	static int number = 0;

	*width = _width;
	*height = _height;
	*bytes_per_pixel = 3;
	*format = "rgb24";

	++number;

	QImage image( _width, _height, QImage::Format_RGB888 );

	QPainter p( &image );
	p.fillRect( image.rect(), Qt::white );

	p.setPen( QPen( QBrush( Qt::red ), 50 ) );

	int px = (2*number) % _width;
	if( px >= _width )
	{
		px = _width - px;
	}

	p.drawPoint( px, 50 );

	p.setPen( QPen( QBrush( Qt::green ), 50 ) );

	px = (number*3) % _width;
	if( px >= _width )
	{
		px = _width - px;
	}

	p.drawPoint( px, _height / 2 );

	p.setPen( QPen( QBrush( Qt::blue ), 50 ) );

	px = (number*4) % _width;
	if( px >= _width )
	{
		px = _width - px;
	}

	p.drawPoint( px, _height -50 );

	QByteArray ba( image.width()*image.height()*3, 0 );
	unsigned char* dest = (unsigned char*)ba.data();
	int step = image.width()*3;

	for( int line=0; line<image.height(); ++line )
	{
		const unsigned char* p = image.scanLine( line );
		memcpy( dest, p, step );
		dest += step;
	}

	return ba;
}
