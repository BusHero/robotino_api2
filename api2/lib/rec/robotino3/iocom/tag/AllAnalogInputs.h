#ifndef _REC_ROBOTINO3_IOCOM_TAG_AllAnalogInputs_H_
#define _REC_ROBOTINO3_IOCOM_TAG_AllAnalogInputs_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/AllAnalogInputsFwd.h"
#include "rec/robotino3/iocom/utils.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class AllAnalogInputs : public rec::robotino3::serialio::Tag
				{
				public:
					static AllAnalogInputsPointer create()
					{
						return AllAnalogInputsPointer( new AllAnalogInputs );
					}

					QString print() const
					{
						QString str( "TAG_ALL_ANALOG_INPUTS:\n" );
						for( int i=0; i<_readings.size(); ++i )
						{
							str += QString( "%1\n" ).arg( _readings[i], 0, 'g', 2 );
						}

						return str;
					}

					static AllAnalogInputsPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 32 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "AllAnalogInputs length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "AllAnalogInputs length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							AllAnalogInputsPointer AllAnalogInputs = create();
							
							const char* p = ba.constData();
							for( int i=0; i<ba.size(); i = i+4 )
							{
								AllAnalogInputs->_readings << decodeFloat( p );
								p += 4;
							}

							return AllAnalogInputs;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of AllAnalogInputs tag" ) );
						}
					}

					QVector< float > readings() const
					{
						return _readings;
					}

				private:
					AllAnalogInputs()
						: Tag( TAG_ALL_ANALOG_INPUTS )
					{
					}

					QVector< float > _readings;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_AllAnalogInputs_H_
