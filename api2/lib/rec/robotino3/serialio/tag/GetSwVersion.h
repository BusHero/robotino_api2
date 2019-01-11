#ifndef _REC_ROBOTINO3_SERIALIO_TAG_GETSWVERSION_H_
#define _REC_ROBOTINO3_SERIALIO_TAG_GETSWVERSION_H_

#include "rec/robotino3/serialio/tag/GetSwVersionFwd.h"
#include "rec/robotino3/serialio/Tag.h"

namespace rec
{
	namespace robotino3
	{
		namespace serialio
		{
			namespace tag
			{
				class GetSwVersion : public rec::robotino3::serialio::Tag
				{
				public:
					static GetSwVersionPointer create()
					{
						return GetSwVersionPointer( new GetSwVersion );
					}

					QString print() const
					{
						return "TAG_GET_SW_VERSION";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetSwVersion()
						: Tag( TAG_GET_SW_VERSION )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_SERIALIO_TAG_GETSWVERSION_H_
