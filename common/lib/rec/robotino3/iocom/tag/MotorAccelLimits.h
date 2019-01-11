#ifndef _REC_ROBOTINO3_IOCOM_TAG_MotorAccelLimits_H_
#define _REC_ROBOTINO3_IOCOM_TAG_MotorAccelLimits_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/MotorAccelLimitsFwd.h"
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
				class MotorAccelLimits : public rec::robotino3::serialio::Tag
				{
				public:
					static MotorAccelLimitsPointer create()
					{
						return MotorAccelLimitsPointer( new MotorAccelLimits );
					}

					QString print() const
					{
						QString str = QString( "TAG_MOTOR_ACCEL_LIMITS motor %1\nmin: %2  max: %3" ).arg( _motor ).arg( _minaccel, 0, 'g', 2 ).arg( _maxaccel, 0, 'g', 2 );
						return str;
					}

					static MotorAccelLimitsPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 9 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "MotorAccelLimits length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "MotorAccelLimits length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							MotorAccelLimitsPointer MotorAccelLimits = create();
							
							const char* p = ba.constData();
							MotorAccelLimits->_motor = *p++;
							MotorAccelLimits->_minaccel = decodeFloat( p );
							p += 4;
							MotorAccelLimits->_maxaccel = decodeFloat( p );

							return MotorAccelLimits;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of MotorAccelLimits tag" ) );
						}
					}

					int motor() const
					{
						return _motor;
					}

					float minaccel() const
					{
						return _minaccel;
					}

					float maxaccel() const
					{
						return _maxaccel;
					}

				private:
					MotorAccelLimits()
						: Tag( TAG_MOTOR_ACCEL_LIMITS )
						, _motor( -1 )
						, _minaccel( 0.0f )
						, _maxaccel( 0.0f )
					{
					}

					int _motor;
					float _minaccel;
					float _maxaccel;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_MotorAccelLimits_H_
