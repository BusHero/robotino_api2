//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#ifndef _REC_ROBOTINO_API2_C_ENCODERINPUT_HH_
#define _REC_ROBOTINO_API2_C_ENCODERINPUT_HH_

#include "rec/robotino/api2/EncoderInput.h"

#include <QSharedPointer>

typedef QSharedPointer<rec::robotino::api2::EncoderInput> EncoderInputPtr;

EncoderInputPtr getEncoderInput( EncoderInputId id );

#endif //_REC_ROBOTINO_API2_C_ENCODERINPUT_HH_