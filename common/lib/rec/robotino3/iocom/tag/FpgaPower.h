#ifndef _REC_ROBOTINO3_IOCOM_TAG_FpgaPower_H_
#define _REC_ROBOTINO3_IOCOM_TAG_FpgaPower_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/FpgaPowerFwd.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class FpgaPower : public rec::robotino3::serialio::Tag
				{
				public:
					static FpgaPowerPointer create()
					{
						return FpgaPowerPointer( new FpgaPower );
					}

					QString print() const
					{
						return QString( "TAG_FPGA_POWER: %1" ).arg( (_on?"on":"off") );
					}

					static FpgaPowerPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 1 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "FpgaPower length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "FpgaPower length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							FpgaPowerPointer FpgaPower = create();
							FpgaPower->_on = ( (unsigned char)ba[0] != 0 );
							return FpgaPower;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of FpgaPower tag" ) );
						}
					}

					bool on() const
					{
						return _on;
					}

				private:
					FpgaPower()
						: Tag( TAG_FPGA_POWER )
						, _on( false )
					{
					}

					bool _on;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_FpgaPower_H_
