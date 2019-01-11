/*
Copyright (c) 2011, REC Robotics Equipment Corporation GmbH, Planegg, Germany
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
- Neither the name of the REC Robotics Equipment Corporation GmbH nor the names of
  its contributors may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _SERIALIZATION_H_
#define _SERIALIZATION_H_

#include "rec/rpc/serialization/Complex.h"
#include "rec/rpc/serialization/Primitive.h"
#include "rec/rpc/serialization/String.h"
#include "rec/rpc/Exception.h"

#include <QImage>

// Custom error codes.
enum ErrorCode
{
	DivisionByZero = rec::rpc::User,
};

// Parameter type definition for the binary functions "add", "sub", "mult" and "div".
// Two floating point numbers, therefore a "complex" type is needed.
BEGIN_COMPLEX_DATA_DECLARATION( BinaryFunctionFloatOperands )
DECLARE_PRIMITIVE_MEMBER( float, a );
DECLARE_PRIMITIVE_MEMBER( float, b );
END_COMPLEX_DATA_DECLARATION

// The four functions use all the same parameter type.
// So it can be reused (which means in fact that a typedef is used instead of
// declaring and implementing the same new rec::rpc::serialization::Complex subclass again and again).
USE_COMPLEX_DATA_AS_PARAM( BinaryFunctionFloatOperands, add );
USE_COMPLEX_DATA_AS_PARAM( BinaryFunctionFloatOperands, sub );
USE_COMPLEX_DATA_AS_PARAM( BinaryFunctionFloatOperands, mult );
USE_COMPLEX_DATA_AS_PARAM( BinaryFunctionFloatOperands, div );

// Result type definitions for the rpc function.
// One floating point number, so a "primitive" type is sufficient.
DEFINE_PRIMITIVE_RESULT( add, float );
DEFINE_PRIMITIVE_RESULT( sub, float );
DEFINE_PRIMITIVE_RESULT( mult, float );
DEFINE_PRIMITIVE_RESULT( div, float );

// Topic data type definition for "image".
// One QImage, so a "primitive" type is sufficient.
DEFINE_PRIMITIVE_TOPICDATA( image, QImage );

DEFINE_STRING_TOPICDATA( text );
DEFINE_STRING_TOPICDATA( text2 );

#endif // _SERIALIZATION_H_
