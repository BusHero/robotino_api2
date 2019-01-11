#ifndef _REC_ROBOTINO3_IOCOM_TAG_AllMotorPidParameters_H_
#define _REC_ROBOTINO3_IOCOM_TAG_AllMotorPidParameters_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/AllMotorPidParametersFwd.h"
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
				class AllMotorPidParameters : public rec::robotino3::serialio::Tag
				{
				public:
					static AllMotorPidParametersPointer create()
					{
						return AllMotorPidParametersPointer( new AllMotorPidParameters );
					}

					QString print() const
					{
						QString str( "TAG_ALL_MOTOR_PID_PARAMETERS:\n" );
						for( int i=0; i<_kp.size(); ++i )
						{
							str += QString( "%1 %2 %3\n" ).arg( _kp[i] ).arg( _ki[i] ).arg( _kd[i] );
						}

						return str;
					}

					static AllMotorPidParametersPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 48 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "AllMotorPidParameters length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "AllMotorPidParameters length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							AllMotorPidParametersPointer AllMotorPidParameters = create();
							
							const char* p = ba.constData();
							for( int i=0; i<4; ++i )
							{
								AllMotorPidParameters->_kp << decodeFloat( p );
								p += 4;
								AllMotorPidParameters->_ki << decodeFloat( p );
								p += 4;
								AllMotorPidParameters->_kd << decodeFloat( p );
								p += 4;
							}

							return AllMotorPidParameters;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of AllMotorPidParameters tag" ) );
						}
					}

					QVector< float > kp() const
					{
						return _kp;
					}

					QVector< float > ki() const
					{
						return _ki;
					}

					QVector< float > kd() const
					{
						return _kd;
					}

				private:
					AllMotorPidParameters()
						: Tag( TAG_ALL_MOTOR_PID_PARAMETERS )
					{
					}

					QVector< float > _kp;
					QVector< float > _ki;
					QVector< float > _kd;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_AllMotorPidParameters_H_
