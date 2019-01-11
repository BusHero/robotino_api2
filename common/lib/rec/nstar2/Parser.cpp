#include "rec/nstar2/Parser.h"
#include "rec/nstar2/Com.h"
#include "rec/nstar2/utils.h"

#include "rec/serialport/SerialPort.h"

#include "rec/nstar2/tag/Root.h"

#define HEAD (const char)0xaa
#define DATA_ESCAPE (const char)0x55
#define DATA_MANGLE (const char)0x20

using namespace rec::nstar2;
using namespace rec::nstar2::tag;

Parser::Parser( rec::serialport::SerialPort* serial, Com* com )
	: _serial( serial )
	, _com( com )
{
}

quint16 Parser::calculate_checksum( const QByteArray& body ) const
{
	quint16 length = (quint16)body.size();
    quint16 out = length & 0xff;
    out += length >> 8;
	const char* p = body.constData();
    for( int i=0; i<body.size(); ++i )
	{
		out += (quint8)*p;
		++p;
	}
	return 0xffff & ( (1<<16) - out );
}

tag::TagList Parser::parse()
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
				throw ParserException( "Error reading 1 byte" );
			}

			if( newch == ch )
			{
				++equalCounter;
			}
			ch = newch;
		}

		int length = readUInt16();
		if( _com->verbosity() > 1 )
		{
			_com->log( QString( "Package length: %1" ).arg( length ).toLatin1().constData() );
		}

		QByteArray body = read_unmangled( length );

		quint16 checksum = readUInt16();

		if( _com->verbosity() > 0 )
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
			_com->log( str.toLatin1().constData() );
		}

		QByteArray lengthData( 2, 0 );
		lengthData[0] = 0xFF & length;
		lengthData[1] = length >> 8; 
		quint16 calculatedChecksum = rec::nstar2::calculate_checksum( lengthData + body );

		if( calculatedChecksum != checksum )
		{
			throw ParserException( QString( "Checksum mismatch - read:%1 calculated:%2" ).arg( checksum, 2, 16, QChar( '0' ) ).arg( calculatedChecksum, 2, 16, QChar( '0' ) ) );
		}

		tag::TagList l  = Tag::decode( body );

		if( _com->verbosity() > 0 )
		{
			Q_FOREACH( tag::TagPointer p, l )
			{
				_com->log( p->print().toLatin1().constData() );
			}
		}

		return l;
	}
	catch( rec::serialport::SerialPortException& e )
	{
		_com->log( QString( "Error: %1" ).arg( e.what() ).toLatin1().constData() );
		return tag::TagList();
	}
	catch( ParserException& e )
	{
		_com->log( QString( "Error: %1" ).arg( e.what() ).toLatin1().constData() );
		return tag::TagList();
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
