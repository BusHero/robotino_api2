#ifndef _REC_ROBOTINO3_IOCOM_TAG_BatteryMin_H_
#define _REC_ROBOTINO3_IOCOM_TAG_BatteryMin_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/BatteryMinFwd.h"
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
				class BatteryMin : public rec::robotino3::serialio::Tag
				{
				public:
					static BatteryMinPointer create()
					{
						return BatteryMinPointer( new BatteryMin );
					}

					QString print() const
					{
						return QString( "TAG_BATTERY_MIN: %1" ).arg( _voltage );
					}

					static BatteryMinPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 4 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "BatteryMin length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "BatteryMin length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							BatteryMinPointer BatteryMin = create();
							BatteryMin->_voltage = decodeFloat( ba.constData() );
							return BatteryMin;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of BatteryMin tag" ) );
						}
					}

					float voltage()
					{
						return _voltage;
					}

				private:
					BatteryMin()
						: Tag( TAG_BATTERY_MIN )
						, _voltage( 0.0f )
					{
					}

					float _voltage;
					float _y;
					float _rot;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_BatteryMin_H_
