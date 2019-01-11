#ifndef _REC_ROBOTINO3_IOCOM_TAG_SetMotorDebug_H_
#define _REC_ROBOTINO3_IOCOM_TAG_SetMotorDebug_H_

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
				class SetMotorDebug;
				typedef QSharedPointer< SetMotorDebug > SetMotorDebugPointer;

				class SetMotorDebug : public rec::robotino3::serialio::Tag
				{
				public:
					static SetMotorDebugPointer create(int motor)
					{
						return SetMotorDebugPointer(new SetMotorDebug(motor));
					}

					QString signature() const { return QString("%1 %2").arg(value()).arg(_motor); };

					QString print() const
					{
						return QString("TAG_SET_MOTOR_DEBUG motor:%1").arg((int)_motor);
					}

					QByteArray encode() const
					{
						QByteArray ba(3, 0);
						ba[0] = static_cast<quint8>(value());
						ba[1] = 1;
						ba[2] = _motor;
						return ba;
					}

				private:
					SetMotorDebug(int motor)
						: Tag(TAG_SET_MOTOR_DEBUG)
						, _motor(motor)
					{
					}

					int _motor;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_SetMotorDebug_H_
