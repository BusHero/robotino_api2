//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#ifndef _REC_ROBOTINO_API2_C_NORTHSTAR_HH_
#define _REC_ROBOTINO_API2_C_NORTHSTAR_HH_

#include "rec/robotino/api2/NorthStar.h"

#include <QSharedPointer>

typedef QSharedPointer<rec::robotino::api2::NorthStar> NorthStarPtr;

NorthStarPtr getNorthStar( NorthStarId id );

#endif //_REC_ROBOTINO_API2_C_NORTHSTAR_HH_
