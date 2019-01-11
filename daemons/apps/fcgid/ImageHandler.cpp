#include "ImageHandler.h"
#include "ImageDecoder.h"
#include "Client.h"
#include "rec/robotino/daemons/Log.h"

ImageHandler::ImageHandler( QObject* parent )
: _imageDecoder( new ImageDecoder( this ) )
, _imageRequestTimeoutTimer( new QTimer( this ) )
{
	bool success = true;
	success = (bool)connect( Client::singleton(),
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

	success = connect( _imageRequestTimeoutTimer, SIGNAL( timeout() ), SLOT( on_imageRequestTimeoutTimer_timeout() ) );
	Q_ASSERT( success );

	_imageRequestTimeoutTimer->setSingleShot( true );
	_imageRequestTimeoutTimer->setInterval( Client::singleton()->getParameter( "~/image_request_timeout", 2000 ).toUInt() );

	lastImageTime.start();
}

ImageHandler::~ImageHandler()
{
}

int ImageHandler::timeSinceLastImg()
{
	return lastImageTime.elapsed();
}

QImage ImageHandler::getImage()
{
	qApp->postEvent( this, new SetImageTopicEnableEvent );

	QMutexLocker lk( &_imageDecoderMutex );
	if( NULL == _imageDecoder )
	{
		return QImage();
	}
	return _imageDecoder->getImage();
}

QByteArray ImageHandler::getJpeg()
{
	qApp->postEvent( this, new SetImageTopicEnableEvent );

	QMutexLocker lk( &_imageDecoderMutex );
	rec::robotino::daemons::Log::singleton()->log( QString("ImageHandler::getImageData() Thread ID = %1").arg((unsigned long)QThread::currentThreadId()), 0 );

	if( NULL == _imageDecoder )
	{
		return QByteArray();
	}
	return _imageDecoder->getJpeg();
}

void ImageHandler::on_client_image0_changed( const QByteArray& data,
		unsigned int width,
		unsigned int height,
		unsigned int step,
		const QString& format )
{
	QMutexLocker lk( &_imageDecoderMutex );
	//rec::robotino::daemons::Log::singleton()->log( QString("ImageHandler::on_client_image0_changed Thread ID = %1").arg(QThread::currentThreadId()), 0 );

	if( NULL == _imageDecoder )
	{
		return;
	}
	_imageDecoder->setImage( data, width, height, step, format );
	lastImageTime.restart();
}

void ImageHandler::on_imageRequestTimeoutTimer_timeout()
{
	Client::singleton()->set_rec_robotino_rpc_image0_enabled( false );
}

void ImageHandler::customEvent( QEvent* e )
{
	switch( e->type() )
	{
	case SetImageTopicEnableEventId:
		Client::singleton()->set_rec_robotino_rpc_image0_enabled( true );
		_imageRequestTimeoutTimer->start();
		break;

	default:
		break;
	}
}
