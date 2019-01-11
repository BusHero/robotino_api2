//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#ifndef _REC_ROBOTINO_API2_C_ANALOGINPUT_HH_
#define _REC_ROBOTINO_API2_C_ANALOGINPUT_HH_

#include "rec/robotino/api2/AnalogInput.h"

#include <QSharedPointer>

typedef QSharedPointer<rec::robotino::api2::AnalogInput> AnalogInputPtr;

AnalogInputPtr getAnalogInput( AnalogInputId id );

#endif //_REC_ROBOTINO_API2_C_ANALOGINPUT_HH_
