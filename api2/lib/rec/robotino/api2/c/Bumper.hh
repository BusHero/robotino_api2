//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#ifndef _REC_ROBOTINO_API2_C_BUMPER_HH_
#define _REC_ROBOTINO_API2_C_BUMPER_HH_

#include "rec/robotino/api2/Bumper.h"

#include <QSharedPointer>

typedef QSharedPointer<rec::robotino::api2::Bumper> BumperPtr;

BumperPtr getBumper( BumperId id );
void Bumper_destroyWithComId( rec::robotino::api2::ComId id );

#endif //_REC_ROBOTINO_API2_C_BUMPER_HH_
