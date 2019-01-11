#ifndef _REC_ROBOTINO3_IOCOM_TAG_SetBoot_H_
#define _REC_ROBOTINO3_IOCOM_TAG_SetBoot_H_

#include "rec/robotino3/iocom/tag/SetBootFwd.h"
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
				class SetBoot : public rec::robotino3::serialio::Tag
				{
				public:
					static SetBootPointer create( int val )
					{
						return SetBootPointer( new SetBoot( val ) );
					}

					QString print() const
					{
						return QString( "TAG_SET_BOOT %1" ).arg( _val );
					}

					QByteArray encode() const
					{
						QByteArray ba( 3, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 1;
						ba[2] = _val;
						return ba;
					}

				private:
					SetBoot( int val )
						: Tag( TAG_SET_BOOT )
						, _val( val )
					{
					}

					int _val;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_SetBoot_H_
