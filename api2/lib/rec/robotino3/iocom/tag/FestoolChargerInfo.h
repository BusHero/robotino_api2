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

						for (int i = 0; i < _time.size(); ++i)
						{
							str += QString("\n time %1: %2").arg(i).arg(_time[i]);
						}

						for (int i = 0; i < _accuLoading.size(); ++i)
						{
							str += QString("\n %1 %2").arg(i).arg(_accuLoading[i] ? "loading" : "not loading");
						}

						for (int i = 0; i < _accuConnected.size(); ++i)
						{
							str += QString("\n %1 %2").arg(i).arg(_accuConnected[i] ? "connected" : "not connected");
						}

						for (int i = 0; i < _externalPower.size(); ++i)
						{
							str += QString("\n %1 %2").arg(i).arg(_externalPower[i] ? "external power" : "no external power");
						}

						for (int i = 0; i < _chargerConnected.size(); ++i)
						{
							str += QString("\n %1 %2").arg(i).arg(_chargerConnected[i] ? "charger connected" : "charger not connected");
						}

						for (int i = 0; i < _voltages.size(); ++i)
						{
							str += QString("\n voltage %1: %2").arg(i).arg(_voltages[i]);
						}

						for (int i = 0; i < _capacities.size(); ++i)
						{
							str += QString("\n capacity %1: %2").arg(i).arg(_capacities[i]);
						}

						str += "\n" + _message;

						return str;
					}

					static FestoolChargerInfoPointer decode(QIODevice* buffer)
					{
						unsigned char ch = 0;
						if (buffer->getChar((char*)&ch))
						{
							if (ch<41)
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

							readings->_time[0] = decodeUInt32(p);
							p += 4;

							readings->_time[1] = decodeUInt32(p);
							p += 4;
							
							unsigned char ch = *(p++);
							for (int i = 0; i < readings->_accuLoading.size(); ++i)
							{
								readings->_accuLoading[i] = ch & (1<<i);
							}

							ch = *(p++);
							for (int i = 0; i < readings->_accuConnected.size(); ++i)
							{
								readings->_accuConnected[i] = (ch & (1 << i));
							}

							unsigned char extPower = *(p++);

							readings->_externalPower[0] = (extPower & 0x1) != 0;
							readings->_chargerConnected[0] = (extPower & 0x2) != 0;
							readings->_externalPower[1] = (extPower & 0x4) != 0;
							readings->_chargerConnected[1] = (extPower & 0x8) != 0;

							for (int i = 0; i < readings->_voltages.size(); ++i)
							{
								readings->_voltages[i] = 0.001f * decodeUInt16(p);
								p += 2;
							}

							for (int i = 0; i < readings->_capacities.size(); ++i)
							{
								readings->_capacities[i] = *(p++);
							}
							
							if(ba.size()>41)
							{
								readings->_message = ba.mid(41);
							}

							return readings;
						}
						else
						{
							throw rec::robotino3::serialio::TagException(QString("Error getting length of TAG_FESTOOL_CHARGER_INFO tag"));
						}
					}

					QVector<unsigned int> time() const { return _time; }
					QVector<bool> accuLoading() const{ return _accuLoading;  }
					QVector<bool> accuConnected() const{ return _accuConnected;  }
					QVector<bool> externalPower() const{ return _externalPower; }
					QVector<bool> chargerConnected() const { return _chargerConnected; }
					QVector<float> voltages() const { return _voltages; }
					QVector<int> capacities() const { return _capacities; }
					QString message() const { return _message; }

				private:
					FestoolChargerInfo()
						: Tag(TAG_FESTOOL_CHARGER_INFO)
						, _time(2,0)
						, _accuLoading(8,false)
						, _accuConnected(8,false)
						, _externalPower(2,false)
						, _chargerConnected(2,false)
						, _voltages(8, 0)
						, _capacities(8, 0)
					{
					}

					QVector<unsigned int> _time;
					QVector<bool> _accuLoading;
					QVector<bool> _accuConnected;
					QVector<bool> _externalPower;
					QVector<bool> _chargerConnected;
					QVector<float> _voltages;
					QVector<int> _capacities;
					QString _message;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_FestoolChargerInfo_H_
