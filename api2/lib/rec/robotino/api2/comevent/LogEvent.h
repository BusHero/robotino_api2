//  Copyright (C) 2004-2013, Robotics Equipment Corporation GmbH

#ifndef _REC_ROBOTINO_API2_COMEVENT_LOGEVENT_H_
#define _REC_ROBOTINO_API2_COMEVENT_LOGEVENT_H_

#include "rec/robotino/api2/comevent/Event.h"

namespace rec
{
	namespace robotino
	{
		namespace api2
		{
			namespace comevent
			{
				class LogEvent : public Event
				{
				public:
					LogEvent( const QString& message_, int level_ )
						: Event( LogEventId )
						, message( message_ )
						, level( level_ )
					{
					}

					QString message;
					int level;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO_API2_COMEVENT_LOGEVENT_H_
