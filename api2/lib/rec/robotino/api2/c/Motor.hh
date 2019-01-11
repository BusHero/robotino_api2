//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#ifndef _REC_ROBOTINO_API2_C_MOTOR_HH_
#define _REC_ROBOTINO_API2_C_MOTOR_HH_

#include "rec/robotino/api2/Motor.h"

#include <QSharedPointer>

typedef QSharedPointer<rec::robotino::api2::Motor> MotorPtr;

MotorPtr getMotor( MotorId id );

#endif //_REC_ROBOTINO_API2_C_MOTOR_HH_