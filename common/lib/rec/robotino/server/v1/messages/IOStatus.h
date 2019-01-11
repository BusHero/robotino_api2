#ifndef _REC_ROBOTINO_SERVER_V1_MESSAGES_IOSTATUS_H_
#define _REC_ROBOTINO_SERVER_V1_MESSAGES_IOSTATUS_H_

#include "rec/iocontrol/remotestate/SensorState.h"

#include <QByteArray>
#include <QString>

namespace rec
{
	namespace robotino
	{
		namespace server
		{
			namespace v1
			{
				namespace messages
				{
					class IOStatus
					{
					public:
						static QByteArray encode( const rec::iocontrol::remotestate::SensorState& sensorState );

					private:
					};
				}
			}
		}
	}
}

#endif //_REC_ROBOTINO_SERVER_V1_MESSAGES_IOSTATUS_H_
