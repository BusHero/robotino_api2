#ifndef _REC_ROBOTINO3_IOCOM_TAG_SetOdometry_H_
#define _REC_ROBOTINO3_IOCOM_TAG_SetOdometry_H_

#include "rec/robotino3/iocom/tag/SetOdometryFwd.h"
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
				class SetOdometry : public rec::robotino3::serialio::Tag
				{
				public:
					static SetOdometryPointer create( float x, float y, float rot )
					{
						return SetOdometryPointer( new SetOdometry( x, y, rot ) );
					}

					QString print() const
					{
						return QString( "TAG_SET_ODOMETRY %1 %2 %3" ).arg( _x ).arg( _y ).arg( _rot );
					}

					QByteArray encode() const
					{
						QByteArray ba( 14, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 12;
						encodeFloat( ba.data()+2, _x );
						encodeFloat( ba.data()+6, _y );
						encodeFloat( ba.data()+10, _rot );
						return ba;
					}

				private:
					SetOdometry( float x, float y, float rot )
						: Tag( TAG_SET_ODOMETRY )
						, _x( x )
						, _y( y )
						, _rot( rot )
					{
					}

					float _x;
					float _y;
					float _rot;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_SetOdometry_H_
