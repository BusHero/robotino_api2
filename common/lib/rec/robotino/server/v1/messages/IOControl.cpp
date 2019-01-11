#include "rec/robotino/server/v1/messages/IOControl.h"

using namespace rec::robotino::server::v1::messages;

bool IOControl::decode( rec::iocontrol::remotestate::SetState* setState, const QByteArray& data )
{
	if( data.size() < 76 )
	{
		return false;
	}

	const unsigned char* datap = reinterpret_cast<const unsigned char*>( data.constData() );

	setState->isImageRequest = ( *datap > 0 ? true : false );
	setState->shutdown = ( *(datap+1) > 0 ? true : false );
	setState->imageServerPort = *(datap+2);
	setState->imageServerPort |= ( *(datap+3) << 8 );

	setState->nstar_roomId = *(datap+4);
	
	if( false == setState->fromQDSAProtocol( datap+9 ) )
	{
		return false;
	}

	setState->setOdometry = ( *(datap+56) > 0 ? true : false );

	setState->odometryX = *( reinterpret_cast<const float*>( datap+57 ) );
	setState->odometryY = *( reinterpret_cast<const float*>( datap+61 ) );
	setState->odometryPhi = *( reinterpret_cast<const float*>( datap+65 ) );

	setState->nstar_ceilingCal = *( reinterpret_cast<const float*>( datap+69 ) );
	if( setState->nstar_ceilingCal <= 0.0f )
	{
		setState->nstar_ceilingCal = 1.0f;
	}

	return true;
}

