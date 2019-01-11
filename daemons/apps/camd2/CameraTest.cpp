#include "CameraTest.h"

#include <QtGui>

#include <stdio.h>

CameraTest::CameraTest()
	: _width( 320 )
	, _height( 320 )
{
}

CameraTest::~CameraTest()
{
	stop();
}

void CameraTest::setVideoDevice( const QString& name )
{
}

// possible values: mmap, read, userptr
void CameraTest::setIOMethod( const QString& name )
{
}

void CameraTest::setResolution( int width, int height )
{
	_width = width;
	_height= height;
}

void CameraTest::resolution( int* width, int* height )
{
}

// possible values: yuyv, uyvy, mjpeg
void CameraTest::setFormat( const QString& format )
{
}

void CameraTest::setBrightness( int value )
{
}

void CameraTest::setContrast( int value )
{
}

void CameraTest::setSaturation( int value )
{
}

void CameraTest::setAutoWhiteBalanceEnabled( bool enable )
{
}

void CameraTest::setGain( int value )
{
}

void CameraTest::setWhiteBalanceTemperature( int value )
{
}

void CameraTest::setBacklightCompensation( int value )
{
}

void CameraTest::setAutoExposureEnabled( bool enable )
{
}

void CameraTest::setExposure( int value )
{
}

void CameraTest::setAutoFocusEnabled( bool enable )
{
}

void CameraTest::setFocus( int focus )
{
}

void CameraTest::setSharpness( int value )
{
}

void CameraTest::start()
{
}

void CameraTest::stop()
{
}

QByteArray CameraTest::grab( unsigned int* width, unsigned int* height, unsigned int* bytes_per_pixel, QString* format )
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
