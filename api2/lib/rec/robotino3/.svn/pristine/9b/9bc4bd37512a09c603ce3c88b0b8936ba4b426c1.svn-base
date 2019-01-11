#ifndef _REC_ROBOTINO3_IOCOM_TAG_MotorDebug_H_
#define _REC_ROBOTINO3_IOCOM_TAG_MotorDebug_H_

#include "rec/robotino3/serialio/Tag.h"
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
				class MotorDebug;
				typedef QSharedPointer< MotorDebug > MotorDebugPointer;

				class MotorDebug : public rec::robotino3::serialio::Tag
				{
				public:
					static MotorDebugPointer create()
					{
						return MotorDebugPointer(new MotorDebug);
					}

					QString print() const
					{
						QString str("TAG_MOTOR_DEBUG:");
						for (int i = 0; i<_speeds.size(); ++i)
						{
							str += QString("\n  M%1 t:%2 speed:%3 set:%4 current:%5 control:%6").arg(_motor).arg((float)_startTime + 0.1f*i*_deltaTime).arg(_speeds[i]).arg(_speed_setpoints[i]).arg(_currents[i]).arg(_control_points[i]);
						}

						return str;
					}

					static MotorDebugPointer decode(QIODevice* buffer)
					{
						unsigned char ch = 0;
						if (buffer->getChar((char*)&ch))
						{
							if (ch<6 || (ch-6)%8 != 0 )
							{
								throw rec::robotino3::serialio::TagException(QString("TAG_MOTOR_DEBUG length=%1 not allowed").arg((quint32)ch));
							}

							QByteArray ba = buffer->read(ch);
							if (ba.size() != ch)
							{
								throw rec::robotino3::serialio::TagException(QString("TAG_MOTOR_DEBUG length=%1 but only %2 bytes left in body").arg((quint32)ch).arg(ba.size()));
							}

							MotorDebugPointer readings = create();
							const char* p = ba.constData();

							readings->_motor = *(p++);

							readings->_startTime = decodeUInt32(p);
							p += 4;

							readings->_deltaTime = *(p++);

							for (int i = 0; i < ba.size() - 6; i += 8)
							{
								readings->_speed_setpoints << decodeInt16(p);
								p += 2;

								readings->_speeds << decodeInt16(p);
								p += 2;

								readings->_currents << 0.001f*decodeUInt16(p);
								p += 2;

								readings->_control_points << decodeInt16(p);
								p += 2;
							}

							return readings;
						}
						else
						{
							throw rec::robotino3::serialio::TagException(QString("Error getting length of TAG_MOTOR_DEBUG tag"));
						}
					}

					int motor() const { return _motor; }
					unsigned int startTime() const { return _startTime; }
					int deltaTime() const { return _deltaTime; }

					QVector<int> speeds() const
					{
						return _speeds;
					}

					QVector<int> speed_setpoints() const
					{
						return _speed_setpoints;
					}

					QVector<float> currents() const
					{
						return _currents;
					}

					QVector<int> control_points() const
					{
						return _control_points;
					}

				private:
					MotorDebug()
						: Tag(TAG_MOTOR_DEBUG)
					{
					}

					int _motor;
					unsigned int _startTime;
					int _deltaTime;
					QVector<int> _speeds;
					QVector<int> _speed_setpoints;
					QVector<float> _currents;
					QVector<int> _control_points;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_FestoolChargerInfo_H_
