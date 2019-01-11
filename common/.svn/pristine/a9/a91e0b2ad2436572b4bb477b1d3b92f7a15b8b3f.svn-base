#ifndef _REC_ROBOTINO3_IOCOM_TAG_SetOdometryRotation_H_
#define _REC_ROBOTINO3_IOCOM_TAG_SetOdometryRotation_H_

#include "rec/robotino3/iocom/tag/SetOdometryRotationFwd.h"
#include "rec/robotino3/serialio/Tag.h"
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
				class SetOdometryRotation : public rec::robotino3::serialio::Tag
				{
				public:
					static SetOdometryRotationPointer create( float rot )
					{
						return SetOdometryRotationPointer( new SetOdometryRotation( rot ) );
					}

					QString print() const
					{
						return QString( "TAG_SET_ODOMETRY_ROTATION %1" ).arg( _rot );
					}

					QByteArray encode() const
					{
						QByteArray ba( 6, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 4;
						encodeFloat( ba.data()+2, _rot );
						return ba;
					}

				private:
					SetOdometryRotation( float rot )
						: Tag( TAG_SET_ODOMETRY_ROTATION )
						, _rot( rot )
					{
					}

					float _rot;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_SetOdometryRotation_H_
