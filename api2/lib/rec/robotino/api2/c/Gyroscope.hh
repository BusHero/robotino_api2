//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#ifndef _REC_ROBOTINO_API2_C_Gyroscope_HH_
#define _REC_ROBOTINO_API2_C_Gyroscope_HH_

#include "rec/robotino/api2/Gyroscope.h"

#include <QSharedPointer>

typedef QSharedPointer<rec::robotino::api2::Gyroscope> GyroscopePtr;

GyroscopePtr getGyroscope( GyroscopeId id );
void Gyroscope_destroyWithComId( rec::robotino::api2::ComId id );

#endif //_REC_ROBOTINO_API2_C_Gyroscope_HH_
