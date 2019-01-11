//  Copyright (C) 2004-2009, Robotics Equipment Corporation GmbH

#include "CameraView.h"

#include "rec/cv_lt/jpeg.h"
#include "rec/cv_lt/yuyv2rgb.h"

#include <iostream>

CameraView::CameraView()
: _timerBuffer( 50, 0 )
, _timerBufferIndex( 0 )
, _frequencyLabel( new QLabel( this ) )
, _numBytesLabel( new QLabel( this ) )
{
	setMinimumSize( 320, 240 );

	_frequencyLabel->move( 5, 5 );
	_frequencyLabel->setFixedWidth( 50 );
	_numBytesLabel->move( 5, 25 );
	_numBytesLabel->setFixedWidth( 50 );
}

void CameraView::setMJPGImage( const QByteArray& data )
{
	QByteArray buffer( 1024*768*3, 0 );

	unsigned int width;
	unsigned int height;
	unsigned int numChannels;

	if( false == rec::cv_lt::jpg_decompress( data.constData(),
			data.size(),
			buffer.data(),
			buffer.size(),
			&width,
			&height,
			&numChannels
			) )
	{
		qDebug() << "Error decoding mjpeg";
		return;
	}

	if( width != _image.width()
		|| height != _image.height() )
	{
		_image = QImage( width, height, QImage::Format_RGB32 );
	}

	for( int line = 0; line < height; ++line )
	{
		unsigned char* dst = _image.scanLine( line );
		const char* src = buffer.constData() + width * numChannels * line;
		for( int x = 0; x < width; ++x )
		{
			dst[0] = src[2];
			dst[1] = src[1];
			dst[2] = src[0];
			dst += 4;
			src += 3;
		}
	}

	_timerBuffer[_timerBufferIndex++] = _timer.elapsed();
	if( _timerBufferIndex >= _timerBuffer.size() )
	{
		_timerBufferIndex = 0;
	}

	float ms = 0.0f;
	for( int i=0; i<_timerBuffer.size(); ++i )
	{
		ms += _timerBuffer[i];
	}
	ms /= _timerBuffer.size();
	if( 0.0f == ms )
	{
		ms = 10000.0f;
	}

	_frequencyLabel->setText( QString( "%1 hz" ).arg( 1000.0f / ms, 0, 'f', 0 ) );
	_numBytesLabel->setText( QString( "%1 kb" ).arg( data.size() / 1024 ) );

	_timer.start();

	update();
}

void CameraView::setImage( const QByteArray& data,
									unsigned int width,
									unsigned int height,
									unsigned int step,
									const QString& format )
{
	if( "mjpg" == format )
	{
		setMJPGImage( data );
		return;
	}

	if( width != _image.width()
		|| height != _image.height() )
	{
		_image = QImage( width, height, QImage::Format_RGB32 );
	}

	if( "yuyv" == format )
	{
		rec::cv_lt::yuyv2argb32( data.constData(),
			width,
			height,
			step,
			(char*)_image.scanLine( 0 ),
			_image.bytesPerLine()
			);
	}
	else
	{
		qDebug() << "Unsupported format";
		return;
	}

	_timerBuffer[_timerBufferIndex++] = _timer.elapsed();
	if( _timerBufferIndex >= _timerBuffer.size() )
	{
		_timerBufferIndex = 0;
	}

	float ms = 0.0f;
	for( int i=0; i<_timerBuffer.size(); ++i )
	{
		ms += _timerBuffer[i];
	}
	ms /= _timerBuffer.size();
	if( 0.0f == ms )
	{
		ms = 10000.0f;
	}

	_frequencyLabel->setText( QString( "%1 hz" ).arg( 1000.0f / ms, 0, 'f', 0 ) );
	_numBytesLabel->setText( QString( "%1 kb" ).arg( data.size() / 1024 ) );

	_timer.start();

	update();
}

void CameraView::paintEvent( QPaintEvent* e )
{
	QPainter painter( this );

	painter.drawImage( rect(), _image );
}
