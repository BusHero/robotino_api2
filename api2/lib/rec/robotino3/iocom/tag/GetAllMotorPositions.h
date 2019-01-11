#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetAllMotorPositions_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetAllMotorPositions_H_

#include "rec/robotino3/iocom/tag/GetAllMotorPositionsFwd.h"
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
				class GetAllMotorPositions : public rec::robotino3::serialio::Tag
				{
				public:
					static GetAllMotorPositionsPointer create()
					{
						return GetAllMotorPositionsPointer( new GetAllMotorPositions );
					}

					QString print() const
					{
						return "TAG_GET_ALL_MOTOR_POSITIONS";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetAllMotorPositions()
						: Tag( TAG_GET_ALL_MOTOR_POSITIONS )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetAllMotorPositions_H_
