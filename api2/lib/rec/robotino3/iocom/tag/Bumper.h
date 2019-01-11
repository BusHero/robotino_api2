#ifndef _REC_ROBOTINO3_IOCOM_TAG_Bumper_H_
#define _REC_ROBOTINO3_IOCOM_TAG_Bumper_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/BumperFwd.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class Bumper : public rec::robotino3::serialio::Tag
				{
				public:
					static BumperPointer create()
					{
						return BumperPointer( new Bumper );
					}

					QString print() const
					{
						return QString( "TAG_BUMPER: %1" ).arg( (_hit?"hit":"not hit") );
					}

					static BumperPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 1 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "Bumper length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "Bumper length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							BumperPointer Bumper = create();
							Bumper->_hit = ( (unsigned char)ba[0] != 0 );
							return Bumper;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of Bumper tag" ) );
						}
					}

					bool hit() const
					{
						return _hit;
					}

				private:
					Bumper()
						: Tag( TAG_BUMPER )
						, _hit( false )
					{
					}

					bool _hit;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_Bumper_H_
