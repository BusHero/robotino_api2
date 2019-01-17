//  Copyright (C) 2004-2009, Robotics Equipment Corporation GmbH

#include "rec/serialport/SerialPortRTAI.h"
#include "rec/serialport/SerialPortImpl_rtai.h"

using rec::serialport::SerialPortRTAI;

SerialPortRTAI::SerialPortRTAI()
: rec::serialport::SerialPort( new rec::serialport::SerialPortImpl_rtai )
{
}
