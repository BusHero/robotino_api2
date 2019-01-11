#include "rec/robotino3/grippercom/Com.h"
#include "rec/robotino3/grippercom/Decoder.h"

#include "rec/robotino3/grippercom/tag/tagenum.h"
#include "rec/robotino3/grippercom/tag/ResetLpc.h"
#include "rec/robotino3/grippercom/tag/SetPwm.h"
#include "rec/robotino3/grippercom/tag/SetLed.h"
#include "rec/robotino3/grippercom/tag/GetSensorReadings.h"
#include "rec/robotino3/grippercom/tag/SensorReadings.h"
#include "rec/robotino3/grippercom/tag/Error.h"

using namespace rec::robotino3::grippercom;

Com::Com()
	: rec::robotino3::serialio::Com( new rec::robotino3::grippercom::Decoder )
{
	#ifdef WIN32
	this->setUsbId( 0x1E29, 0x040F );
#else
	this->setUsbName( "REC_GmbH_Gripper" );
#endif
}

void Com::parse_i( rec::robotino3::serialio::TagPointer p )
{
	switch( p->value() )
	{
	case TAG_SENSORREADINGS:
		{
			rec::robotino3::grippercom::tag::SensorReadingsPointer p2 = p.staticCast<rec::robotino3::grippercom::tag::SensorReadings>();
			sensorReadingsCb( p2->i1(), p2->i2(), p2->ad3(), p2->ad4(), p2->ad6() );
		}
		break;

	case TAG_ERROR:
		{
			rec::robotino3::grippercom::tag::ErrorPointer p2 = p.staticCast<rec::robotino3::grippercom::tag::Error>();
			logCb( p2->errorStr().toLatin1().constData() );
		}
		break;

	default:
		QString str = QString( "No action for tag %1 implemented" ).arg( p->value() );
		logCb( str.toLatin1().constData() );
		break;
	}
}

void Com::resetLpc( bool enterUSBBootloader ){ produce( rec::robotino3::serialio::TagHelper( tag::ResetLpc::create( enterUSBBootloader ) ) ); }
void Com::getSensorReadings(){ produce( rec::robotino3::serialio::TagHelper( tag::GetSensorReadings::create() ) ); }
void Com::setPwm( int servo, int ratio ){ produce( rec::robotino3::serialio::TagHelper( tag::SetPwm::create( servo, ratio ) ) ); }
void Com::setLed( int led, bool on ){ produce( rec::robotino3::serialio::TagHelper( tag::SetLed::create( led, (on?1:0) ) ) ); }

void Com::sensorReadingsCb( float i1, float i2, float ad3, float ad4, float ad6 ){}
