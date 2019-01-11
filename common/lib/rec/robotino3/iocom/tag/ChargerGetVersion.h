#ifndef _REC_ROBOTINO3_IOCOM_TAG_ChargerGetVersion_H_
#define _REC_ROBOTINO3_IOCOM_TAG_ChargerGetVersion_H_

#include "rec/robotino3/iocom/tag/ChargerGetVersionFwd.h"
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
				class ChargerGetVersion : public rec::robotino3::serialio::Tag
				{
				public:
					static ChargerGetVersionPointer create( int chargerID )
					{
						return ChargerGetVersionPointer( new ChargerGetVersion( chargerID ) );
					}

					QString print() const
					{
						return "TAG_CHARGER_GET_VERSION";
					}

					QByteArray encode() const
					{
						QByteArray ba( 3, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 1;
						ba[2] = _chargerID;
						return ba;
					}

					QString signature() const { return QString( "%1 %2" ).arg( value() ).arg( _chargerID ); };

				private:
					ChargerGetVersion( int chargerID )
						: Tag( TAG_CHARGER_GET_VERSION )
						, _chargerID( chargerID )
					{
					}

					int _chargerID;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_ChargerGetVersion_H_
