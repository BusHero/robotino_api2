#ifndef _REC_ROBOTINO3_IOCOM_TAG_ChargerClearError_H_
#define _REC_ROBOTINO3_IOCOM_TAG_ChargerClearError_H_

#include "rec/robotino3/iocom/tag/ChargerClearErrorFwd.h"
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
				class ChargerClearError : public rec::robotino3::serialio::Tag
				{
				public:
					static ChargerClearErrorPointer create( int chargerID )
					{
						return ChargerClearErrorPointer( new ChargerClearError( chargerID ) );
					}

					QString print() const
					{
						return "TAG_CHARGER_CLEAR_ERROR";
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
					ChargerClearError( int chargerID )
						: Tag( TAG_CHARGER_CLEAR_ERROR )
						, _chargerID( chargerID )
					{
					}

					int _chargerID;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_ChargerClearError_H_
