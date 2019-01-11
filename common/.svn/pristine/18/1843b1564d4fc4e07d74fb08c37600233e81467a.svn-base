#ifndef _REC_ROBOTINO3_IOCOM_TAG_FestoolChargerHotswapStart_H_
#define _REC_ROBOTINO3_IOCOM_TAG_FestoolChargerHotswapStart_H_

#include "rec/robotino3/iocom/tag/SetMotorModeFwd.h"
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
				class FestoolChargerHotswapStart;
				typedef QSharedPointer< FestoolChargerHotswapStart > FestoolChargerHotswapStartPointer;

				class FestoolChargerHotswapStart : public rec::robotino3::serialio::Tag
				{
				public:
					static FestoolChargerHotswapStartPointer create()
					{
						return FestoolChargerHotswapStartPointer(new FestoolChargerHotswapStart);
					}

					QString print() const
					{
						return QString("TAG_FESTOOL_CHARGER_HOTSWAP_START");
					}

					QByteArray encode() const
					{
						QByteArray ba(2, 0);
						ba[0] = static_cast<quint8>(value());
						ba[1] = 0;
						return ba;
					}

				private:
					FestoolChargerHotswapStart()
						: Tag(TAG_FESTOOL_CHARGER_HOTSWAP_START)
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_FestoolChargerHotswapStart_H_
