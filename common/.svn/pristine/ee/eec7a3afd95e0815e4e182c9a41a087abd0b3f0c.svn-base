#ifndef _REC_ROBOTINO3_IOCOM_TAG_SetPwrBtn_H_
#define _REC_ROBOTINO3_IOCOM_TAG_SetPwrBtn_H_

#include "rec/robotino3/iocom/tag/SetPwrBtnFwd.h"
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
				class SetPwrBtn : public rec::robotino3::serialio::Tag
				{
				public:
					static SetPwrBtnPointer create( bool high )
					{
						return SetPwrBtnPointer( new SetPwrBtn( high ) );
					}

					QString print() const
					{
						return QString( "TAG_SET_PWRBTN %1" ).arg( (_high?"High":"Low") );
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
					SetPwrBtn( bool high )
						: Tag( TAG_SET_PWRBTN )
						, _high( high )
					{
					}

					bool _high;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_SetPwrBtn_H_
