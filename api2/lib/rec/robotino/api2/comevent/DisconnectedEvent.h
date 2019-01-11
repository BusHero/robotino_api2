//  Copyright (C) 2004-2013, Robotics Equipment Corporation GmbH

#ifndef _REC_ROBOTINO_API2_COMEVENT_DISCONNECTEDEVENT_H_
#define _REC_ROBOTINO_API2_COMEVENT_DISCONNECTEDEVENT_H_

#include "rec/robotino/api2/comevent/Event.h"

namespace rec
{
	namespace robotino
	{
		namespace api2
		{
			namespace comevent
			{
				class DisconnectedEvent : public Event
				{
				public:
					DisconnectedEvent()
						: Event( DisconnectedEventId )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO_API2_COMEVENT_DISCONNECTEDEVENT_H_
