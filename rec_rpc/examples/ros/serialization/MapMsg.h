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

#ifndef _MAPMSG_H_
#define _MAPMSG_H_

#include "rec/rpc/serialization/Complex.h"
#include "rec/rpc/serialization/String.h"
#include "rec/rpc/serialization/ByteArray.h"
#include "rec/rpc/serialization/Image.h"
#include "rec/rpc/serialization/Primitive.h"

BEGIN_COMPLEX_DATA_DECLARATION_AND_IMPLEMENTATION( mapData, 1.0 )
ADD_MEMBER( map )
ADD_MEMBER( resolution )
ADD_MEMBER( offsetx )
ADD_MEMBER( offsety )
END_COMPLEX_DATA_DECLARATION_AND_IMPLEMENTATION_CONSTRUCTOR
DECLARE_IMAGE_MEMBER( map )
DECLARE_PRIMITIVE_MEMBER( double, resolution )
DECLARE_PRIMITIVE_MEMBER( double, offsetx )
DECLARE_PRIMITIVE_MEMBER( double, offsety )
END_COMPLEX_DATA_DECLARATION_AND_IMPLEMENTATION

USE_COMPLEX_DATA_AS_TOPICDATA( mapData, map )

#endif //_MAPMSG_H_
