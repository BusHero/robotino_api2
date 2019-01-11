#ifndef _REC_ROBOTINO3_IOCOM_TAG_FestoolChargerInfo_H_
#define _REC_ROBOTINO3_IOCOM_TAG_FestoolChargerInfo_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/AllMotorReadingsFwd.h"
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
				class FestoolChargerInfo;
				typedef QSharedPointer< FestoolChargerInfo > FestoolChargerInfoPointer;

				class FestoolChargerInfo : public rec::robotino3::serialio::Tag
				{
				public:
					static FestoolChargerInfoPointer create()
					{
						return FestoolChargerInfoPointer(new FestoolChargerInfo);
					}

					QString print() const
					{
						QString str("TAG_FESTOOL_CHARGER_INFO:");

						str += QString("\nTime %1").arg(_time);
						
						str += QString("\nExternal power is %1").arg(_externalPower ? "connected" : "not connected");
						str += QString("\nCharger is %1").arg(_chargerConnected ? "connected" : "not connected");

						for (int i = 0; i<4; ++i)
						{
							str += QString("\nAkku %1").arg(i + 1);
							str += QString("\n  sys connect %1").arg(_accuConnected[i]?"On":"Off");
							str += QString("\n  loading %1").arg(_accuLoading[i] ? "On" : "Off");
							str += QString("\n  voltage %1V").arg(_voltages[i]);
							str += QString("\n  capacity %1\%").arg(_capacities[i]);
						}

						return str;
					}

					static FestoolChargerInfoPointer decode(QIODevice* buffer)
					{
						unsigned char ch = 0;
						if (buffer->getChar((char*)&ch))
						{
							if (ch<18)
							{
								throw rec::robotino3::serialio::TagException(QString("TAG_FESTOOL_CHARGER_INFO length=%1 not allowed").arg((quint32)ch));
							}

							QByteArray ba = buffer->read(ch);
							if (ba.size() != ch)
							{
								throw rec::robotino3::serialio::TagException(QString("TAG_FESTOOL_CHARGER_INFO length=%1 but only %2 bytes left in body").arg((quint32)ch).arg(ba.size()));
							}

							FestoolChargerInfoPointer readings = create();
							const char* p = ba.constData();

							readings->_time = decodeUInt32(p);
							p += 4;
							
							unsigned char accuLoad = *(p++);

							if (accuLoad & 0x1)	readings->_accuLoading[0] = true;
							else if (accuLoad & 0x2) readings->_accuLoading[1] = true;
							else if (accuLoad & 0x4) readings->_accuLoading[2] = true;
							else if (accuLoad & 0x8) readings->_accuLoading[3] = true;

							if (accuLoad & 0x10)	readings->_accuConnected[0] = true;
							else if (accuLoad & 0x20)	readings->_accuConnected[1] = true;
							else if (accuLoad & 0x40)	readings->_accuConnected[2] = true;
							else if (accuLoad & 0x80)	readings->_accuConnected[3] = true;

							unsigned char extPower = *(p++);

							readings->_externalPower = (extPower & 0x1) != 0;
							readings->_chargerConnected = (extPower & 0x2) != 0;

							for (int i = 0; i < 4; ++i)
							{
								readings->_voltages[i] = 0.001f * decodeUInt16(p);
								p += 2;
							}

							for (int i = 0; i < 4; ++i)
							{
								readings->_capacities[i] = *(p++);
							}
							
							if(ba.size()>18)
							{
								readings->_message = ba.mid(18);
							}

							return readings;
						}
						else
						{
							throw rec::robotino3::serialio::TagException(QString("Error getting length of TAG_FESTOOL_CHARGER_INFO tag"));
						}
					}

					unsigned int time() const { return _time; }
					QVector<bool> accuLoading() const{ return _accuLoading;  }
					QVector<bool> accuConnected() const{ return _accuConnected;  }
					bool externalPower() const{ return _externalPower; }
					bool chargerConnected() const { return _chargerConnected; }
					QVector<float> voltages() const { return _voltages; }
					QVector<int> capacities() const { return _capacities; }
					QString message() const { return _message; }

				private:
					FestoolChargerInfo()
						: Tag(TAG_FESTOOL_CHARGER_INFO)
						, _time(0)
						, _accuLoading(4,false)
						, _accuConnected(4,false)
						, _externalPower(false)
						, _chargerConnected(false)
						, _voltages(4, 0)
						, _capacities(4, 0)
					{
					}

					unsigned int _time;
					QVector<bool> _accuLoading;
					QVector<bool> _accuConnected;
					bool _externalPower;
					bool _chargerConnected;
					QVector<float> _voltages;
					QVector<int> _capacities;
					QString _message;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_FestoolChargerInfo_H_
