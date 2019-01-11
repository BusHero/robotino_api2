//  Copyright (C) 2004-2013, Robotics Equipment Corporation GmbH

#ifndef _REC_ROBOTINO_API2_COMEVENT_ERROREVENT_H_
#define _REC_ROBOTINO_API2_COMEVENT_ERROREVENT_H_

#include "rec/robotino/api2/comevent/Event.h"

namespace rec
{
	namespace robotino
	{
		namespace api2
		{
			namespace comevent
			{
				class ErrorEvent : public Event
				{
				public:
					ErrorEvent( const QString& message_ )
						: Event( ErrorEventId )
						, message( message_ )
					{
					}

					QString message;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO_API2_COMEVENT_ERROREVENT_H_
