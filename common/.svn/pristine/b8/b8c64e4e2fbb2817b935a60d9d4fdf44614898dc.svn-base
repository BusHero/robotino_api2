#include "rec/robotino/server/v1/messages/CameraControl.h"

using namespace rec::robotino::server::v1::messages;

CameraControl::CameraControl()
: width( 0 )
, height( 0 )
{
}

bool CameraControl::decode( const QByteArray& data )
{
	quint16 tmp_uint16 = 0;

	if( data.size() < 5)
	{
		return false;
	}

	const unsigned char* datap = reinterpret_cast<const unsigned char*>( data.constData() );

	switch( *datap )
	{
	case 0://QVGA
		width = 320;
		height = 240;
		break;

	case 1://VGA
		width = 640;
		height = 480;
		break;

	case 2://Unknown
		width = *(datap+1);
		width |= ( *(datap+2) << 8 );
		height = *(datap+3);
		height |= ( *(datap+4) << 8 );
		break;
	}

	return true;
}

