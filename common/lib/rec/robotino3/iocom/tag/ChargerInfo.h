#ifndef _REC_ROBOTINO3_IOCOM_TAG_ChargerInfo_H_
#define _REC_ROBOTINO3_IOCOM_TAG_ChargerInfo_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/utils.h"
#include "rec/robotino3/iocom/tag/ChargerInfoFwd.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class ChargerInfo : public rec::robotino3::serialio::Tag
				{
				public:
					static ChargerInfoPointer create()
					{
						return ChargerInfoPointer( new ChargerInfo );
					}

					QString print() const
					{
						QString str = QString( "TAG_CHARGER_INFO %1:\n"
							"time: %2s\n"
							"battery voltage: %3V\n"
							"charging current: %4A\n"
							"bat1 temp: %5�C\n"
							"bat2 temp: %6�C\n"
							"state #%7: %8" )
							.arg( _chargerID )
							.arg( _time )
							.arg( _batteryVoltage )
							.arg( _chargingCurrent )
							.arg( _bat1temp )
							.arg( _bat2temp )
							.arg( _state_number )
							.arg( _state );

						return str;
					}

					static ChargerInfoPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( ch < 21 )
							{
								throw rec::robotino3::serialio::TagException( QString( "ChargerInfo length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() < ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "ChargerInfo length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							ChargerInfoPointer ChargerInfo = create();

							const char* p = ba.constData();
							ChargerInfo->_chargerID = *(p++);
							ChargerInfo->_time = decodeUInt32( p );
							p+=4;
							ChargerInfo->_batteryVoltage = decodeFloat( p );
							p+=4;
							ChargerInfo->_chargingCurrent = decodeFloat( p );
							p+=4;
							ChargerInfo->_bat1temp = decodeFloat( p );
							p+=4;
							ChargerInfo->_bat2temp = decodeFloat( p );
							p+=4;
							ChargerInfo->_state_number = *(p++);

							ChargerInfo->_state = QString( ba.mid( 22 ) );

							return ChargerInfo;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of ChargerInfo tag" ) );
						}
					}

					int chargerID() const
					{
						return _chargerID;
					}

					unsigned int time() const
					{
						return _time;
					}

					float batteryVoltage() const
					{
						return _batteryVoltage;
					}

					float chargingCurrent() const
					{
						return _chargingCurrent;
					}

					float bat1temp() const
					{
						return _bat1temp;
					}

					float bat2temp() const
					{
						return _bat2temp;
					}

					int state_number() const
					{
						return _state_number;
					}

					QString state() const
					{
						return _state;
					}

				private:
					ChargerInfo()
						: Tag( TAG_CHARGER_INFO )
						, _chargerID( 0 )
						, _time( 0 )
						, _batteryVoltage( 0 )
						, _chargingCurrent( 0 )
						, _bat1temp( 0 )
						, _bat2temp( 0 )
						, _state_number( 0 )
					{
					}

					int _chargerID;
					unsigned int _time;
					float _batteryVoltage;
					float _chargingCurrent;
					float _bat1temp;
					float _bat2temp;
					int _state_number;
					QString _state;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_ChargerInfo_H_
