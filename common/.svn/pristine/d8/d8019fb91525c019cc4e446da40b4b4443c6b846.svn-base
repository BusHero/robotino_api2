#ifndef _REC_ROBOTINO_RPC_MOTOR_DEBUG_H_
#define _REC_ROBOTINO_RPC_MOTOR_DEBUG_H_

#include "rec/rpc/serialization/Complex.h"
#include "rec/rpc/serialization/ByteArray.h"
#include "rec/rpc/serialization/String.h"

BEGIN_COMPLEX_DATA_DECLARATION_AND_IMPLEMENTATION( rec_robotino_rpc_motor_debug_t, 1.0 )
ADD_MEMBER( motor )
ADD_MEMBER( times ) // in s
ADD_MEMBER( speed_setpoints ) // in rpm
ADD_MEMBER( speeds ) // in rpm
ADD_MEMBER( currents ) // in A
ADD_MEMBER( control_points ) // in units
END_COMPLEX_DATA_DECLARATION_AND_IMPLEMENTATION_CONSTRUCTOR
DECLARE_PRIMITIVE_MEMBER(int, motor)
DECLARE_PRIMITIVE_MEMBER(QVector<unsigned int>, times)
DECLARE_PRIMITIVE_MEMBER(QVector<float>, speed_setpoints)
DECLARE_PRIMITIVE_MEMBER(QVector<float>, speeds)
DECLARE_PRIMITIVE_MEMBER(QVector<float>, currents)
DECLARE_PRIMITIVE_MEMBER(QVector<float>, control_points)
END_COMPLEX_DATA_DECLARATION_AND_IMPLEMENTATION

USE_COMPLEX_DATA_AS_TOPICDATA(rec_robotino_rpc_motor_debug_t, rec_robotino_rpc_motor_debug)

#endif //_REC_ROBOTINO_RPC_MOTOR_DEBUG_H_
