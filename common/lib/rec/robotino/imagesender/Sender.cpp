#include "rec/robotino/imagesender/Sender.h"
#include <QtDebug>

using namespace rec::robotino::imagesender;

const int Sender::_startSequenceSize = 10;
const qint8 Sender::_startSequence[10] = {'S','t','a','r','t','S','t','a','r','t'};

const int Sender::_stopSequenceSize = 10;
const qint8 Sender::_stopSequence[10] = {'S','t','o','p','p','S','t','o','p','p'};

const int Sender::_headerSize = 26;

Sender::Sender( QObject* parent )
: QUdpSocket( parent )
, _bytesWritten( 0 )
, _receiverPort( 0 )
, _partSize( 0xFFFF )
, _state( IdleState )
, _timer( new QTimer( this ) )
{
	_timer->setInterval( 20 );
	_timer->setSingleShot( true );
	connect( _timer, SIGNAL( timeout() ), SLOT( on_timer_timeout() ) );
}

void Sender::setRawImageData( const QByteArray& data,
																unsigned int width,
																unsigned int height,
																unsigned int numChannels,
																unsigned int bitsPerChannel,
																unsigned int step )
{
	setImageData( false, data, width, height, numChannels, bitsPerChannel, step );
}

void Sender::setJpgImageData( const QByteArray& data )
{
	setImageData( true, data, 0, 0, 0, 0, 0 );
}

void Sender::setImageData( bool isJpg, const QByteArray& data,
																unsigned int width,
																unsigned int height,
																unsigned int numChannels,
																unsigned int bitsPerChannel,
																unsigned int step )
{
	if( IdleState != _state )
	{
		qDebug() << "calling setImageData while sending image is not permitted";
		return;
	}

	_bytesWritten = 0;

	if( false == data.isEmpty() && false == _receiver.isNull() )
	{
		_imageData.resize( _headerSize + data.size() + _stopSequenceSize );

		memcpy( (void*)_imageData.data(), _startSequence, _startSequenceSize );
		memcpy( (void*)( _imageData.data() + _headerSize ), data.constData(), data.size() );
		memcpy( (void*)(_imageData.data() + _headerSize + data.size() ), _stopSequence, _stopSequenceSize );

		_state = SendingState;

		quint32 dataSize = static_cast<unsigned int>( data.size() );

		_imageData[10] = ( dataSize & 0xFF );
		_imageData[11] = ( ( dataSize >> 8 ) & 0xFF );
		_imageData[12] = ( ( dataSize >> 16 ) & 0xFF );
		_imageData[13] = ( ( dataSize >> 24 ) & 0xFF );

		if( isJpg )
		{
			_imageData[14] = 0; //JPG
		}
		else
		{
			_imageData[14] = 2; //RAW
		}

		if( 320 == width && 240 == height )
		{
			_imageData[15] = 0; //QVGA
		}
		else if( 640 == width && 480 == height )
		{
			_imageData[15] = 1; //VGA
		}
		else
		{
			_imageData[15] = 2; //Custom
		}

		_imageData[16] = ( width & 0xFF );
		_imageData[17] = ( ( width >> 8 ) & 0xFF );
		_imageData[18] = ( ( width >> 16 ) & 0xFF );
		_imageData[19] = ( ( width >> 24 ) & 0xFF );

		_imageData[20] = ( height & 0xFF );
		_imageData[21] = ( ( height >> 8 ) & 0xFF );
		_imageData[22] = ( ( height >> 16 ) & 0xFF );
		_imageData[23] = ( ( height >> 24 ) & 0xFF );

		_imageData[24] = numChannels;
		_imageData[25] = bitsPerChannel;

		on_timer_timeout();
	}
}

void Sender::setReceiver( quint32 address, quint16 port )
{
	_receiver = QHostAddress( address );
	_receiverPort = port;

	connectToHost( _receiver, port );
	waitForConnected();
}

void Sender::stop()
{
	_timer->stop();
	_state = IdleState;
	_imageData.clear();
	//disconnect( this, SIGNAL( bytesWritten( qint64 ) ), this, SLOT( on_bytesWritten( qint64 ) ) );
}

void Sender::on_timer_timeout()
{
	if( _bytesWritten < _imageData.size() )
	{
		qint32 numBytesToWrite = _partSize;
		if( _bytesWritten + numBytesToWrite >= _imageData.size() )
		{
			numBytesToWrite = _imageData.size() - _bytesWritten;
		}

		write( _imageData.constData() + _bytesWritten, numBytesToWrite );

		_bytesWritten += numBytesToWrite;

		//qDebug() << "\nWriting " << _bytesWritten << "of " << _imageData.size();

		//if( -1 == writeDatagram( _imageData.constData() + _bytesWritten, numBytesToWrite, _receiver, _receiverPort ) )
		//{
		//	qDebug() << "Error sending image data " << error();
		//	stop();
		//	return;
		//}
	}

	if( _bytesWritten < _imageData.size() )
	{
		_timer->start();
	}
	else
	{
		_state = IdleState;
		/*emit*/ imageSendingCompleted();
	}
}
