#ifndef _REC_ROBOTINO3_IOCOM_TAG_SetPwrOkState_H_
#define _REC_ROBOTINO3_IOCOM_TAG_SetPwrOkState_H_

#include "rec/robotino3/iocom/tag/SetPwrOkStateFwd.h"
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
				class SetPwrOkState : public rec::robotino3::serialio::Tag
				{
				public:
					static SetPwrOkStatePointer create( bool high )
					{
						return SetPwrOkStatePointer( new SetPwrOkState( high ) );
					}

					QString print() const
					{
						return QString( "TAG_SET_PWR_OK_STATE %1" ).arg( (_high?"high":"low") );
					}

					QByteArray encode() const
					{
						QByteArray ba( 3, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 1;
						ba[2] = ( _high?1:0 );
						return ba;
					}

				private:
					SetPwrOkState( bool high )
						: Tag( TAG_SET_PWR_OK_STATE )
						, _high( high )
					{
					}

					bool _high;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_SetPwrOkState_H_
