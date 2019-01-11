#ifndef _REC_ROBOTINO_SERVER_V1_MESSAGES_KEEPALIVE_H_
#define _REC_ROBOTINO_SERVER_V1_MESSAGES_KEEPALIVE_H_

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
					class KeepAlive
					{
					public:
						static QByteArray encode();

					private:
					};
				}
			}
		}
	}
}

#endif //_REC_ROBOTINO_SERVER_V1_MESSAGES_KEEPALIVE_H_
