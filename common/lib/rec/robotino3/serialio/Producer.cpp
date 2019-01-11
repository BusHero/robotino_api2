#include "rec/robotino3/serialio/Producer.h"
#include "rec/robotino3/serialio/utils.h"

#include "rec/serialport/SerialPort.h"

#define HEAD (const char)0xaa
#define DATA_ESCAPE (const char)0x55
#define DATA_MANGLE (const char)0x20

#define MAX_UNMANGLED_SIZE 200

using namespace rec::robotino3::serialio;

Producer::Producer( rec::serialport::SerialPort* serial, ProducerDelegate* delegate )
	: _serial( serial )
	, _delegate( delegate )
{
}

void Producer::transmit()
{
	if( false == _txBuffer.isEmpty() )
	{
		QByteArray ba = produce();
		int num_bytes_to_send = ba.size();

		if( _delegate && _delegate->producerDelegateVerbosity() > 0 )
		{
			QString str = QString("Sending %1 bytes").arg( num_bytes_to_send );
			_delegate->producerDelegateLog( str );
		}

		const int numBytesPerPackage = 128;
		while( num_bytes_to_send > 0 )
		{
			if( num_bytes_to_send > numBytesPerPackage )
			{
				_serial->write( (const unsigned char*)ba.constData() + ba.size() - num_bytes_to_send, numBytesPerPackage );
				num_bytes_to_send -= numBytesPerPackage;
				msleep( 100 );
			}
			else
			{
				_serial->write( (const unsigned char*)ba.constData() + ba.size() - num_bytes_to_send, num_bytes_to_send );
				num_bytes_to_send = 0;
			}
		}
		_txBuffer.clear();
		_sigVec.clear();
	}
}

void Producer::clearTXBuffer()
{
	_txBuffer.clear();
	_sigVec.clear();
}

TagMap& Producer::operator<<( const TagPointer& tag )
{
	QString signature = tag->signature();
	if( false == _txBuffer.contains( signature ) )
	{
		_sigVec.append( signature );
	}
	_txBuffer[ signature ] = tag;
	return _txBuffer;
}

QByteArray Producer::produce() const
{
	QByteArray unmangled;

	Q_ASSERT( _sigVec.count() == _txBuffer.count() );
	for( int i=0; i<_txBuffer.count(); ++i )
	{
		const QString& signature = _sigVec[i];
		const TagPointer& tag = _txBuffer[signature];

		if( unmangled.size() > MAX_UNMANGLED_SIZE )
		{
			if( _delegate && _delegate->producerDelegateVerbosity() > 0 )
			{
				QString str = QString("Skipping tag %1").arg( signature );
				_delegate->producerDelegateLog( str );
			}
		}
		else
		{
			unmangled += tag->encode();
		}
	}

	quint16 length = static_cast<quint16>( unmangled.size() );

	unmangled.prepend( length >> 8 );
	unmangled.prepend( 0xff & length );

	quint16 checksum = rec::robotino3::serialio::calculate_checksum( unmangled );

	unmangled.append( 0xff & checksum );
	unmangled.append( checksum >> 8 );

	QByteArray mangled;
	for( int i=0; i<unmangled.size(); ++i )
	{
		const QByteRef ch = unmangled[i];
		if( HEAD == ch || DATA_ESCAPE == ch )
		{
			mangled += DATA_ESCAPE;
			mangled += ( ch ^ DATA_MANGLE );
		}
		else
		{
			mangled += ch;
		}
	}

	mangled.prepend( HEAD );

	if(  _delegate && _delegate->producerDelegateVerbosity() > 2 )
	{
		QString str;
		int j=0;
		while( j<mangled.size() )
		{
			str += ">>";
			for( int i=0; i<20 && j<mangled.size(); ++i )
			{
				str += QString( " %1" ).arg( quint16(0xFF) & (quint16)mangled[j], 2, 16, QChar( '0' ) );
				++j;
			}
			str += "\n";
		}
		_delegate->producerDelegateLog( str );
	}

	return mangled;
}


