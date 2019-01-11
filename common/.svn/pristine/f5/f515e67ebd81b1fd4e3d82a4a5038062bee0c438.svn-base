#include "rec/robotino3/serialio/Parser.h"
#include "rec/robotino3/serialio/Decoder.h"
#include "rec/robotino3/serialio/utils.h"

#include "rec/serialport/SerialPort.h"

#include "rec/robotino3/serialio/Tag.h"

#define HEAD (const char)0xaa
#define DATA_ESCAPE (const char)0x55
#define DATA_MANGLE (const char)0x20

using namespace rec::robotino3::serialio;

Parser::Parser( rec::serialport::SerialPort* serial, Decoder* decoder, ParserDelegate* delegate )
	: _serial( serial )
	, _decoder( decoder )
	, _delegate( delegate )
{
}

TagList Parser::parse()
{
	try
	{
		char ch = 0;
		int equalCounter = 0;

		while( HEAD != ch )
		{
			char newch;
			if( 1 != _serial->read( (unsigned char*)&newch, 1 ) || equalCounter > 100 )
			{
				return TagList();
			}

			if( newch == ch )
			{
				++equalCounter;
			}
			ch = newch;
		}

		int length = readUInt16();
		if( _delegate && _delegate->parserDelegateVerbosity() > 1 )
		{
			_delegate->parserDelegateLog( QString( "Package length: %1" ).arg( length ) );
		}

		QByteArray body = read_unmangled( length );

		quint16 checksum = readUInt16();

		if( _delegate && _delegate->parserDelegateVerbosity() > 1 )
		{
			QByteArray baDebug( 3, 0 );
			baDebug[0] = HEAD;
			baDebug[1] = 0xFF & length;
			baDebug[2] = 0xFF & ( length >> 8 );
			baDebug += body;
			baDebug.append( 0xFF & checksum );
			baDebug.append( 0xFF & ( checksum >> 8 ) );

			QString str;
			int j=0;
			while( j<baDebug.size() )
			{
				str += "<<";
				for( int i=0; i<20 && j<baDebug.size(); ++i )
				{
					str += QString( " %1" ).arg( quint16(0xFF) & (quint16)baDebug[j], 2, 16, QChar( '0' ) );
					++j;
				}
				str += "\n";
			}
			_delegate->parserDelegateLog( str );
		}

		QByteArray lengthData( 2, 0 );
		lengthData[0] = 0xFF & length;
		lengthData[1] = length >> 8; 
		quint16 calculatedChecksum = rec::robotino3::serialio::calculate_checksum( lengthData + body );

		if( calculatedChecksum != checksum )
		{
			throw ParserException( QString( "Checksum mismatch - read:%1 calculated:%2" ).arg( checksum, 2, 16, QChar( '0' ) ).arg( calculatedChecksum, 2, 16, QChar( '0' ) ) );
		}

		TagList l  = _decoder->decode( body );

		if( _delegate && _delegate->parserDelegateVerbosity() > 1 )
		{
			Q_FOREACH( TagPointer p, l )
			{
				_delegate->parserDelegateLog( p->print() );
			}
		}

		return l;
	}
	catch( rec::serialport::SerialPortException& e )
	{
		if( _delegate )
		{
			_delegate->parserDelegateLog( QString( "Error: %1" ).arg( e.what() ) );
		}
		return TagList();
	}
	catch( ParserException& e )
	{
		if( _delegate )
		{
			_delegate->parserDelegateLog( QString( "Error: %1" ).arg( e.what() ) );
		}
		return TagList();
	}
}

QByteArray Parser::read_unmangled( int length ) const
{
	QByteArray ba( length, 0 );
	int i=0;
	while( i<length )
	{
		char ch = 0;
		if( 1 != _serial->read( (unsigned char*)&ch, 1 ) )
		{
			throw ParserException( "Error reading 1 byte" );
		}

		if( DATA_ESCAPE == ch )
		{
			if( 1 != _serial->read( (unsigned char*)&ch, 1 ) )
			{
				throw ParserException( "Error reading 1 byte" );
			}
			ba[i] = ( ch ^ DATA_MANGLE );
		}
		else if( HEAD == ch )
		{
			throw ParserException( "Found HEAD in message body" );
		}
		else
		{
			ba[i] = ch;
		}

		++i;
	}

	return ba;
}

quint16 Parser::readUInt16() const
{
	QByteArray ba = read_unmangled( 2 );
	return decodeUInt16( ba );
}
