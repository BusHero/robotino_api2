#include "ImageHandler.h"
#include "ImageDecoder.h"
#include "Client.h"

ImageHandler::ImageHandler( QObject* parent )
	: _imageDecoder( NULL )
{
	bool success = true;

	success &= connect( Client::singleton(),
		SIGNAL( image0_changed( const QByteArray&,
									unsigned int,
									unsigned int,
									unsigned int,
									const QString& ) ),
		SLOT( on_client_image0_changed( const QByteArray&,
									unsigned int,
									unsigned int,
									unsigned int,
									const QString& ) ) );
	Q_ASSERT( success );

	start();
}

ImageHandler::~ImageHandler()
{
	exit();
	wait();
}

QImage ImageHandler::getImage() const
{
	QMutexLocker lk( &_imageDecoderMutex );
	if( NULL == _imageDecoder )
	{
		return QImage();
	}

	return _imageDecoder->getImage();
}

void ImageHandler::on_client_image0_changed( const QByteArray& data,
									unsigned int width,
									unsigned int height,
									unsigned int step,
									const QString& format )
{
	QMutexLocker lk( &_imageDecoderMutex );
	if( NULL == _imageDecoder )
	{
		return;
	}

	_imageDecoder->setImage( data, width, height, step, format );
}

void ImageHandler::run()
{
	{
		QMutexLocker lk( &_imageDecoderMutex );
		_imageDecoder = new ImageDecoder;
	}

	exec();

	{
		QMutexLocker lk( &_imageDecoderMutex );
		delete _imageDecoder;
		_imageDecoder = NULL;
	}
}
