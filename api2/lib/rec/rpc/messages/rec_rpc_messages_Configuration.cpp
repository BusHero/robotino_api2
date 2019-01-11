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

#include "rec/rpc/messages/rec_rpc_messages_Configuration.h"
#include "rec/rpc/messages/rec_rpc_messages_Message.h"
#include "rec/rpc/rec_rpc_utils.h"

using namespace rec::rpc::messages;

QByteArray Configuration::encode( const rec::rpc::configuration::Configuration& config )
{
	QByteArray configData = config.save();

	unsigned int configDataSize = configData.size();

	QByteArray data( configDataSize + headerSize, 0 );

	data[0] = ConfigurationId;
	
	encodeUInt32( data.data() + 1, configDataSize );

	memcpy( data.data() + headerSize, configData.constData(), configDataSize );

	return data;
}

bool Configuration::decode( const QByteArray& data, rec::rpc::configuration::Configuration* configuration )
{
	return configuration->load( data );
}
