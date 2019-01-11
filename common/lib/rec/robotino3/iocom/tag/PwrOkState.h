#ifndef _REC_ROBOTINO3_IOCOM_TAG_PwrOkState_H_
#define _REC_ROBOTINO3_IOCOM_TAG_PwrOkState_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/PwrOkStateFwd.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class PwrOkState : public rec::robotino3::serialio::Tag
				{
				public:
					static PwrOkStatePointer create()
					{
						return PwrOkStatePointer( new PwrOkState );
					}

					QString print() const
					{
						return QString( "TAG_PWR_OK_STATE: %1" ).arg( (_high?"high":"low") );
					}

					static PwrOkStatePointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 1 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "PwrOkState length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "PwrOkState length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							PwrOkStatePointer PwrOkState = create();
							PwrOkState->_high = ( (unsigned char)ba[0] != 0 );
							return PwrOkState;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of PwrOkState tag" ) );
						}
					}

					bool high() const
					{
						return _high;
					}

				private:
					PwrOkState()
						: Tag( TAG_PWR_OK_STATE )
						, _high( false )
					{
					}

					bool _high;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_PwrOkState_H_
