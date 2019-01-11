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

#ifndef _REC_RPC_UTILS_H_
#define _REC_RPC_UTILS_H_

#include "rec/rpc/defines.h"
#include <qglobal.h>

namespace rec
{
	namespace rpc
	{
		static const float PI = 3.14159265358979323846f;

		static REC_RPC_FUNCTION_IS_NOT_USED float deg2rad( float deg )
		{
			return PI * deg / 180.0f;
		}

		static REC_RPC_FUNCTION_IS_NOT_USED float rad2deg( float rad )
		{
			return 180.0f * rad / PI;
		}

		static char* encodeInt32( char* data, const qint32 value )
		{
			*data++ = 0xFF & value;
			*data++ = 0xFF & ( value >> 8 );
			*data++ = 0xFF & ( value >> 16 );
			*data++ = 0xFF & ( value >> 24 );
			return data;
		}

		static char* encodeUInt32( char* data, const quint32 value )
		{
			*data++ = 0xFF & value;
			*data++ = 0xFF & ( value >> 8 );
			*data++ = 0xFF & ( value >> 16 );
			*data++ = 0xFF & ( value >> 24 );
			return data;
		}

		static const char* decodeInt32( const char* data, qint32* value )
		{
			*value = qint32( quint8( *data++ ) );
			*value |= ( qint32( quint8( *data++ ) ) << 8);
			*value |= ( qint32( quint8( *data++ ) ) << 16);
			*value |= ( qint32( quint8( *data++ ) ) << 24);
			return data;
		}

		static const char* decodeUInt32( const char* data, quint32* value )
		{
			*value = quint32( quint8( *data++ ) );
			*value |= ( quint32( quint8( *data++ ) ) << 8);
			*value |= ( quint32( quint8( *data++ ) ) << 16);
			*value |= ( quint32( quint8( *data++ ) ) << 24);
			return data;
		}

		static qint32 decodeInt32( const char* data )
		{
			qint32 value = quint8( *data++ );
			value |= ( qint32( quint8( *data++ ) ) << 8);
			value |= ( qint32( quint8( *data++ ) ) << 16);
			value |= ( qint32( quint8( *data++ ) ) << 24);
			return value;
		}

		static quint32 decodeUInt32( const char* data )
		{
			quint32 value = quint8( *data++ );
			value |= ( quint32( quint8( *data++ ) ) << 8);
			value |= ( quint32( quint8( *data++ ) ) << 16);
			value |= ( quint32( quint8( *data++ ) ) << 24);
			return value;
		}
	}
}

#endif //_REC_RPC_UTILS_H_
