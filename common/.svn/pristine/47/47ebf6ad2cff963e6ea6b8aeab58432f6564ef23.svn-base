#include "rec/robotino3/serialio/Decoder.h"
#include "rec/robotino3/serialio/tag/HwVersion.h"
#include "rec/robotino3/serialio/tag/SwVersion.h"

rec::robotino3::serialio::TagList rec::robotino3::serialio::Decoder::decode( const QByteArray& data )
{
	QBuffer buffer;
	buffer.setData( data );
	buffer.open( QIODevice::ReadOnly );

	rec::robotino3::serialio::TagList l;
	while( false == buffer.atEnd() )
	{
		l << decode( &buffer );
	}

	return l;
}

rec::robotino3::serialio::TagList rec::robotino3::serialio::Decoder::decode( QIODevice* buffer )
{
	TagList l;

	unsigned char ch;
	if( buffer->getChar( (char*)&ch ) )
	{
		TagPointer p;
		switch( ch )
		{
		case TAG_HW_VERSION:
			p = rec::robotino3::serialio::tag::HwVersion::decode( buffer );
			break;

		case TAG_SW_VERSION:
			p = rec::robotino3::serialio::tag::SwVersion::decode( buffer );
			break;

		default:
			p = decode( ch, buffer );
			break;
		}

		if( p )
		{
			l << p;
		}
	}

	return l;
}
