#include "rec/robotino/server/v1/messages/IOStatus.h"

using namespace rec::robotino::server::v1::messages;

QByteArray IOStatus::encode( const rec::iocontrol::remotestate::SensorState& sensorState )
{
	quint16 distance[9] = {0,0,0,0,0,0,0,0,0};
	qint16 tmp_int16 = 0;
	const quint8* tmp_uint8p = NULL;

	for( unsigned int i=0; i<9; ++i )
	{
		distance[i] = static_cast<quint16>( 1024.0f / 2.55f * sensorState.distanceSensor[i] );
	}

	QByteArray ba( 144, 0 );

	unsigned char* p = reinterpret_cast<unsigned char*>( ba.data() );

	*p = 1;
	*(p+1) = 141;
	*(p+2) = 0;

	*(p+3) = ( sensorState.serialLineUpdateFreqeuncy & 0xFF );
	*(p+4) = ( ( sensorState.serialLineUpdateFreqeuncy >> 8 ) & 0xFF );

	sensorState.toQDSAProtocol( p+5 );

	*(p+106) = ( sensorState.nstar_sequenceNumber & 0xFF );
	*(p+107) = ( ( sensorState.nstar_sequenceNumber >> 8 ) & 0xFF );
	*(p+108) = ( ( sensorState.nstar_sequenceNumber >> 16 ) & 0xFF );
	*(p+109) = ( ( sensorState.nstar_sequenceNumber >> 24 ) & 0xFF );

	*(p+110) = sensorState.nstar_roomId;
	*(p+111) = sensorState.nstar_numSpotsVisible;

	tmp_int16 = sensorState.nstar_posX;
	*(p+112) = ( tmp_int16 & 0xFF );
	*(p+113) = ( ( tmp_int16 >> 8 ) & 0xFF );

	tmp_int16 = sensorState.nstar_posY;
	*(p+114) = ( tmp_int16 & 0xFF );
	*(p+115) = ( ( tmp_int16 >> 8 ) & 0xFF );

	tmp_uint8p = reinterpret_cast<const quint8*>( &sensorState.nstar_posTheta );
	*(p+116) = *tmp_uint8p;
	*(p+117) = *(tmp_uint8p+1);
	*(p+118) = *(tmp_uint8p+2);
	*(p+119) = *(tmp_uint8p+3);

	*(p+120) = ( sensorState.nstar_magSpot0 & 0xFF );
	*(p+121) = ( ( sensorState.nstar_magSpot0 >> 8 ) & 0xFF );

	*(p+122) = ( sensorState.nstar_magSpot1 & 0xFF );
	*(p+123) = ( ( sensorState.nstar_magSpot1 >> 8 ) & 0xFF );

	*(p+124) = ( sensorState.isPassiveMode ? 1 : 0 );

	tmp_uint8p = reinterpret_cast<const quint8*>( &sensorState.odometryX );
	*(p+125) = *tmp_uint8p;
	*(p+126) = *(tmp_uint8p+1);
	*(p+127) = *(tmp_uint8p+2);
	*(p+128) = *(tmp_uint8p+3);

	tmp_uint8p = reinterpret_cast<const quint8*>( &sensorState.odometryY );
	*(p+129) = *tmp_uint8p;
	*(p+130) = *(tmp_uint8p+1);
	*(p+131) = *(tmp_uint8p+2);
	*(p+132) = *(tmp_uint8p+3);

	tmp_uint8p = reinterpret_cast<const quint8*>( &sensorState.odometryPhi );
	*(p+133) = *tmp_uint8p;
	*(p+134) = *(tmp_uint8p+1);
	*(p+135) = *(tmp_uint8p+2);
	*(p+136) = *(tmp_uint8p+3);

	for( unsigned int i=0; i<7; ++i )
	{
		*(p+137+i) = 0;
	}


	return ba;
}

