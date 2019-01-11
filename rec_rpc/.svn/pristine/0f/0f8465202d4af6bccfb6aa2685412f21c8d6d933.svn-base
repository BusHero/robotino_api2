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

#include "rec/rpc/serialization/rec_rpc_serialization_SingleThreadedSerializer.hpp"

using namespace rec::rpc::serialization;

SingleThreadedSerializer::SingleThreadedSerializer( QObject* parent )
	: Serializer( parent )
{
}

void SingleThreadedSerializer::serializeRPCRequest( const QString& name, quint32 seqNum, rec::rpc::serialization::SerializablePtrConst param )
{
	qApp->postEvent( this, new SerializeRPCRequestEvent( name, seqNum, param ) );
}

void SingleThreadedSerializer::serializeRPCResponse( const QString& name, quint32 seqNum, rec::rpc::ErrorCode errorCode, rec::rpc::serialization::SerializablePtrConst result, quintptr receiver )
{
	qApp->postEvent( this, new SerializeRPCResponseEvent( name, seqNum, errorCode, result, receiver ) );
}

void SingleThreadedSerializer::serializeTopicData( unsigned int id, rec::rpc::serialization::SerializablePtrConst data )
{
	qApp->postEvent( this, new SerializeTopicDataEvent( id, data ) );
}

void SingleThreadedSerializer::customEvent( QEvent* e )
{
	switch( e->type() )
	{
	case rec::rpc::serialization::SingleThreadedSerializer::SerializeRPCRequestEventId:
		{
			SerializeRPCRequestEvent* ev = static_cast< SerializeRPCRequestEvent* >( e );
			Q_EMIT RPCRequestSerialized( ev->name, ev->seqNum, serializeData( *ev->param ) );
			break;
		}
	case rec::rpc::serialization::SingleThreadedSerializer::SerializeRPCResponseEventId:
		{
			SerializeRPCResponseEvent* ev = static_cast< SerializeRPCResponseEvent* >( e );
			Q_EMIT RPCResponseSerialized( ev->name, ev->seqNum, ev->errorCode, serializeData( *ev->result ), ev->receiver );
			break;
		}
	case rec::rpc::serialization::SingleThreadedSerializer::SerializeTopicDataEventId:
		{
			SerializeTopicDataEvent* ev = static_cast< SerializeTopicDataEvent* >( e );
			Q_EMIT topicDataSerialized( ev->id, serializeData( *ev->data ) );
			break;
		}
	default:
		break;
	}
}
