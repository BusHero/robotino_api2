#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetOdometry_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetOdometry_H_

#include "rec/robotino3/iocom/tag/GetOdometryFwd.h"
#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class GetOdometry : public rec::robotino3::serialio::Tag
				{
				public:
					static GetOdometryPointer create()
					{
						return GetOdometryPointer( new GetOdometry );
					}

					QString print() const
					{
						return "TAG_GET_ODOMETRY";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetOdometry()
						: Tag( TAG_GET_ODOMETRY )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetOdometry_H_
