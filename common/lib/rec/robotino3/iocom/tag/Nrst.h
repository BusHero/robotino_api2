#ifndef _REC_ROBOTINO3_IOCOM_TAG_Nrst_H_
#define _REC_ROBOTINO3_IOCOM_TAG_Nrst_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/NrstFwd.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class Nrst : public rec::robotino3::serialio::Tag
				{
				public:
					static NrstPointer create()
					{
						return NrstPointer( new Nrst );
					}

					QString print() const
					{
						return QString( "TAG_NRST: %1" ).arg( (_on?"on":"off") );
					}

					static NrstPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 1 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "Nrst length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "Nrst length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							NrstPointer Nrst = create();
							Nrst->_on = ( (unsigned char)ba[0] != 0 );
							return Nrst;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of Nrst tag" ) );
						}
					}

					bool on() const
					{
						return _on;
					}

				private:
					Nrst()
						: Tag( TAG_NRST )
						, _on( false )
					{
					}

					bool _on;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_Nrst_H_
