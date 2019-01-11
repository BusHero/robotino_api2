//  Copyright (C) 2004-2013, Robotics Equipment Corporation GmbH

#ifndef _REC_ROBOTINO_API2_C_COMPACTBHA_HH_
#define _REC_ROBOTINO_API2_C_COMPACTBHA_HH_

#include "rec/robotino/api2/CompactBHA.h"

#include <QSharedPointer>

typedef QSharedPointer<rec::robotino::api2::CompactBHA> CompactBHAPtr;

static CompactBHAPtr getCompactBHA( CompactBHAId id );

#endif //_REC_ROBOTINO_API2_C_COMPACTBHA_HH_
