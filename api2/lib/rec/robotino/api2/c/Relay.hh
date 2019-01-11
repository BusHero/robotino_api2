//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#ifndef _REC_ROBOTINO_API2_C_RELAY_HH_
#define _REC_ROBOTINO_API2_C_RELAY_HH_

#include "rec/robotino/api2/Relay.h"

#include <QSharedPointer>

typedef QSharedPointer<rec::robotino::api2::Relay> RelayPtr;

RelayPtr getRelay( RelayId id );

#endif //_REC_ROBOTINO_API2_C_RELAY_HH_
