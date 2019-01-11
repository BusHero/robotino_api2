#include "ImageDecoder.h"

#include "rec/cv_lt/jpeg.h"
#include "rec/cv_lt/yuyv2rgb.h"
#include "rec/cv_lt/rgb2argb32.h"

ImageDecoder::ImageDecoder()
	: _decodeTimer( new QTimer( this ) )
	, _sequenceNumber( 0 )
	, _lastDecodedImage( 0 )
{

	_decodeTimer->setSingleShot( false );
	_decodeTimer->setInterval( 200 );
	_decodeTimer->start();

	bool success = true;

	success &= connect( _decodeTimer, SIGNAL( timeout() ), SLOT( decode() ) );
	Q_ASSERT( success );
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

QImage ImageDecoder::getImage() const
{
	QMutexLocker lk( &_imageMutex );
	return _image;
}

void ImageDecoder::decode()
{
	static QByteArray rgbImage( 1024*768*3, 0 );
	static QImage liveImage;

	QByteArray data;
	unsigned int width;
	unsigned int height;
	unsigned int step;
	QString format;

	{
		QMutexLocker lk( &_dataMutex );
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
	}

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

	QMutexLocker lk( &_imageMutex );
	_image = liveImage;
}
