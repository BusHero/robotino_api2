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

#include "rec/rpc/messages/rec_rpc_messages_Topic.h"
#include "rec/rpc/messages/rec_rpc_messages_Message.h"
#include "rec/rpc/rec_rpc_utils.h"

#include <QDataStream>

using namespace rec::rpc::messages;

// Topic meta data layout:
// 1 message id
// 4 message length without header
// QDataStream serialized data:
//   channel (topic) id
//   sender's address and port
//   payload size

// Topic payload will be sent separately immedately after the meta data package

QByteArray Topic::encode( unsigned int id, const int serClientInfoSize, const int payloadSize )
{
	QByteArray data;

	data.resize( headerSize + metaDataSize );

	//Header (Teil 1)************************
	//message id
	char* p = data.data();
	*p++ = rec::rpc::messages::TopicId;

	//Header (Teil 2)
	//Laenge berechnen
	quint32 dataSizeWithoutHeader = metaDataSize + serClientInfoSize + payloadSize;
	p = encodeUInt32( p , dataSizeWithoutHeader );

	// Metadaten
	p = encodeUInt32( p, id );
	/*p =*/ encodeInt32( p, serClientInfoSize );

	return data;
}

bool rec::rpc::messages::Topic::decode( const QByteArray& data, unsigned int* id, rec::rpc::ClientInfo* client )
{
	//Daten ohne Header und Nutzdatenlaenge

	const char* p = data.constData();
	p = decodeUInt32( p, id );

	if ( 0 != client )
	{
		int serClientInfoSize = decodeInt32( p );
		if ( serClientInfoSize == 0 )
		{
			client->clear();
		}
		else
		{
			p += sizeof( int );
			QByteArray serClientInfo = QByteArray::fromRawData( p, serClientInfoSize );
			QDataStream s( serClientInfo );
			s >> *client;
		}
	}

	return true;
}
