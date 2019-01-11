#include "ImageDecoder.h"

#include "rec/cv_lt/jpeg.h"
#include "rec/cv_lt/yuyv2rgb.h"
#include "rec/cv_lt/rgb2argb32.h"

#include "rec/robotino/daemons/Log.h"
/*
* This ImageDecoder has been altered by Shao. 
* Unlike the original ImageDecoder used in webd, this ImageDecoder do NOT has decodeTimer and
* only decode when getImage();
*/

ImageDecoder::ImageDecoder( QObject* parent )
	: QObject( parent )
	, _sequenceNumber( 0 )
	, _lastDecodedImage( 0 )
	, _width(0)
	, _height(0)
	, _step(0)
{
}

void ImageDecoder::setImage( const QByteArray& data,
	unsigned int width,
	unsigned int height,
	unsigned int step,
	const QString& format )
{
	QMutexLocker lk( &_dataMutex );
	_data = data;
	_width = width;
	_height = height;
	_step = step;
	_format = format;
	++_sequenceNumber;
}

QImage ImageDecoder::getImage()
{
	QMutexLocker lk( &_imageMutex );
	decode();
	return _image;
}

QByteArray ImageDecoder::getJpeg()
{
	QMutexLocker lk( &_dataMutex );

	if( _format.contains( "jpg" ) )
	{
		return _data;
	}
	else if( _format.contains("rgb24") )
	{
		if(_width*_height*3 == _data.size() )
		{
			QImage image( _width, _height, QImage::Format_RGB888 );

			for( unsigned int line=0; line<_height; ++line )
			{
				const unsigned char* psrc = (const unsigned char*)_data.constData() + 3*_width * line;
				unsigned char* pdst = image.scanLine( line );
				memcpy( pdst, psrc, _width*3 );
			}
			
			QByteArray ba;
			QBuffer buffer(&ba);
			image.save(&buffer,"jpg");
			return ba;
		}
	}
	
	return QByteArray();
}
/*
* WARNING: This function is NOT thread safe, therefore it MUST should be called ONLY when having a lock on _imageMutex
*/
void ImageDecoder::decode()
{
	//QTime t;
	//t.start();

	static QByteArray rgbImage( 1024*768*3, 0 );
	static QImage liveImage;

	QByteArray data;
	unsigned int width;
	unsigned int height;
	unsigned int step;
	QString format;

	if( _lastDecodedImage >= _sequenceNumber )
	{
		return;
	}

	_lastDecodedImage = _sequenceNumber;

	data = _data;
	width = _width;
	height = _height;
	step = _step;
	format = _format;
	if( "mjpg" == format )
	{
		unsigned int numChannels;
		if( rec::cv_lt::jpg_decompress( data.constData(), data.size(), rgbImage.data(), rgbImage.size(), &width, &height, &numChannels ) )
		{
			if( liveImage.width() != width || liveImage.height() != height )
			{
				liveImage = QImage( width, height, QImage::Format_ARGB32 );
			}

			rec::cv_lt::rgb2argb32( rgbImage.constData(),
				width,
				height,
				width * numChannels,
				(char*)liveImage.scanLine( 0 ),
				liveImage.bytesPerLine()
				);
		}
	}
	else if( "yuyv" == format )
	{
		if( liveImage.width() != width || liveImage.height() != height )
		{
			liveImage = QImage( width, height, QImage::Format_ARGB32 );
		}

		rec::cv_lt::yuyv2argb32( data.constData(),
			width,
			height,
			step,
			(char*)liveImage.scanLine( 0 ),
			liveImage.bytesPerLine()
			);
	}
	else if( "rgb24" == format )
	{
		if( liveImage.width() != width || liveImage.height() != height )
		{
			liveImage = QImage( width, height, QImage::Format_ARGB32 );
		}

		rec::cv_lt::rgb2argb32( data.constData(),
			width,
			height,
			step,
			(char*)liveImage.scanLine( 0 ),
			liveImage.bytesPerLine()
			);
	}

	_image = liveImage;
}
