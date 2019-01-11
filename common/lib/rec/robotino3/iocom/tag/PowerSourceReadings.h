#ifndef _REC_ROBOTINO3_IOCOM_TAG_PowerSourceReadings_H_
#define _REC_ROBOTINO3_IOCOM_TAG_PowerSourceReadings_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/PowerSourceReadingsFwd.h"
#include "rec/robotino3/iocom/utils.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class PowerSourceReadings : public rec::robotino3::serialio::Tag
				{
				public:
					static PowerSourceReadingsPointer create()
					{
						return PowerSourceReadingsPointer( new PowerSourceReadings );
					}

					QString print() const
					{
						QString str = QString( "TAG_POWER_SOURCE_READINGS:"
							"\nVoltage: %1"
							"\nCurrent: %2"
							"\nExternal power: %3"
							"\nNum chargers: %4"
							"\nBattery type: %5"
							"\nBattery low: %6"
							"\nBattery shutdown counter: %7" )
							.arg( _battery_voltage )
							.arg( _system_current )
							.arg( _ext_power ? "Yes" : "No" )
							.arg( _num_chargers )
							.arg( _batteryType )
							.arg( _batteryLow ? "Yes" : "No" )
							.arg( _batteryLowShutdownCounter )
							;

						return str;
					}

					static PowerSourceReadingsPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 16 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "POWER_SOURCE_READINGS length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "POWER_SOURCE_READINGS length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							PowerSourceReadingsPointer readings = create();
							const char* p = ba.constData();

							readings->_battery_voltage = decodeFloat( p );
							p += 4;
							readings->_system_current = decodeFloat( p );
							p += 4;
							readings->_ext_power = *(p++);
							readings->_num_chargers = *(p++);
							readings->_batteryType = ( *(p++)>0 ? "NiMH" : "Pb" );
							readings->_batteryLow = *(p++);
							readings->_batteryLowShutdownCounter = decodeUInt32( p );

							return readings;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of POWER_SOURCE_READINGS tag" ) );
						}
					}

					float battery_voltage() const
					{
						return _battery_voltage;
					}

					float system_current() const
					{
						return _system_current;
					}

					bool ext_power() const
					{
						return _ext_power;
					}

					int num_chargers() const
					{
						return _num_chargers;
					}

					QString batteryType() const
					{
						return _batteryType;
					}

					bool batteryLow() const
					{
						return _batteryLow;
					}

					int batteryLowShutdownCounter() const
					{
						return _batteryLowShutdownCounter;
					}


				private:
					PowerSourceReadings()
						: Tag( TAG_POWER_SOURCE_READINGS )
						, _battery_voltage( 0.0f )
						, _system_current( 0.0f )
						, _ext_power( false )
						, _num_chargers( 0 )
						, _batteryType( "Pb" )
						, _batteryLow( false )
						, _batteryLowShutdownCounter( 0 )
					{
					}

					float _battery_voltage;
					float _system_current;
					bool _ext_power;
					int _num_chargers;
					QString _batteryType;
					bool _batteryLow;
					int _batteryLowShutdownCounter;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_PowerSourceReadings_H_
