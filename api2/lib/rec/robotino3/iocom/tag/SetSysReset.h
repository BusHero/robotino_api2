#ifndef _REC_ROBOTINO3_IOCOM_TAG_SetSysReset_H_
#define _REC_ROBOTINO3_IOCOM_TAG_SetSysReset_H_

#include "rec/robotino3/iocom/tag/SetSysResetFwd.h"
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
				class SetSysReset : public rec::robotino3::serialio::Tag
				{
				public:
					static SetSysResetPointer create(  bool high )
					{
						return SetSysResetPointer( new SetSysReset( high ) );
					}

					QString print() const
					{
						return QString( "TAG_SET_SYS_RESET %1" ).arg( (_high?"High":"Low") );
					}

					QByteArray encode() const
					{
						QByteArray ba( 3, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 1;
						ba[2] = (_high?1:0);
						return ba;
					}

				private:
					SetSysReset( bool high )
						: Tag( TAG_SET_SYS_RESET )
						, _high( high )
					{
					}

					bool _high;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_SetSysReset_H_
