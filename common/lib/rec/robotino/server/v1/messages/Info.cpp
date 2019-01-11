#include "rec/robotino/server/v1/messages/Info.h"

using namespace rec::robotino::server::v1::messages;

Info::Info()
: _isPassiveMode( false )
{
}

void Info::setMessage( const QString& message )
{
	_message = message;
}

void Info::setPassiveMode( bool isPassiveMode )
{
	_isPassiveMode = isPassiveMode;
}

QByteArray Info::encode() const
{
	QByteArray ba( 2004, 0 );

	unsigned char* p = reinterpret_cast<unsigned char*>( ba.data() );

	*p = 6;
	*(p+1) = 0xD1;
	*(p+2) = 0x7; //0x7D1 = 2001

	strncpy( (char*)( p+3 ), _message.toLatin1().constData(), 2000 );

	*(p+2003) = ( _isPassiveMode ? 1 : 0 );

	return ba;
}

