#include "Com.h"

Com::Com()
{
	setVerbosity( 2 );
}

void Com::hardwareVersionCb( const char* version )
{
	Q_EMIT hardwareVersion( version );
}

void Com::softwareVersionCb( const char* version )
{
	Q_EMIT softwareVersion( version );
}

void Com::logCb( const char* message )
{
	Q_EMIT log( message );
}

void Com::sensorReadingsCb( float i1, float i2, float ad3, float ad4, float ad6 )
{
	Q_EMIT sensorReadings( i1, i2, ad3, ad4, ad6 );
}

