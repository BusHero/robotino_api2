//  Copyright (C) 2004-2013, Robotics Equipment Corporation GmbH

#ifndef _REC_ROBOTINO_API2_COMEVENT_EVENT_H_
#define _REC_ROBOTINO_API2_COMEVENT_EVENT_H_

#include <QTime>

namespace rec
{
	namespace robotino
	{
		namespace api2
		{
			namespace comevent
			{
				typedef enum
				{
					ConnectedEventId,
					DisconnectedEventId,
					ErrorEventId,
					MotorReadingsEventId,
					SensorEventId,
					LogEventId
				}
				Type;

				class Event
				{
				public:
					Event( Type t )
						: type( t )
						, time( QTime::currentTime() )
					{
					}

					virtual ~Event()
					{
					}

					const Type type;
					QTime time;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO_API2_COMEVENT_EVENT_H_
