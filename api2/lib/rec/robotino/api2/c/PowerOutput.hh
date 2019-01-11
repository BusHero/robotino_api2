//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#ifndef _REC_ROBOTINO_API2_C_POWEROUTPUT_HH_
#define _REC_ROBOTINO_API2_C_POWEROUTPUT_HH_

#include "rec/robotino/api2/PowerOutput.h"

#include <QSharedPointer>

typedef QSharedPointer<rec::robotino::api2::PowerOutput> PowerOutputPtr;

PowerOutputPtr getPowerOutput( PowerOutputId id );

#endif //_REC_ROBOTINO_API2_C_POWEROUTPUT_HH_
