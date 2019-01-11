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

#ifndef _REC_RPC_SERIALIZATION_SINGLETHREADEDSERIALIZER_H_
#define _REC_RPC_SERIALIZATION_SINGLETHREADEDSERIALIZER_H_

#include "rec_rpc_serialization_Serializer.hpp"

namespace rec
{
	namespace rpc
	{
		namespace serialization
		{
			class SingleThreadedSerializer : public Serializer
			{
			public:
				SingleThreadedSerializer( QObject* parent = 0 );

				void serializeRPCRequest( const QString& name, quint32 seqNum, rec::rpc::serialization::SerializablePtrConst param );
				void serializeRPCResponse( const QString& name, quint32 seqNum, rec::rpc::ErrorCode errorCode, rec::rpc::serialization::SerializablePtrConst result, quintptr receiver );
				void serializeTopicData( unsigned int id, rec::rpc::serialization::SerializablePtrConst data );

			private:
				enum EventType
				{
					SerializeRPCRequestEventId = QEvent::User,
					SerializeRPCResponseEventId,
					SerializeTopicDataEventId,
					UserEventId,
				};

				class SerializeRPCRequestEvent : public QEvent
				{
				public:
					SerializeRPCRequestEvent( const QString& name_, quint32 seqNum_, rec::rpc::serialization::SerializablePtrConst param_ )
						: QEvent( (QEvent::Type)rec::rpc::serialization::SingleThreadedSerializer::SerializeRPCRequestEventId )
						, name( name_ )
						, seqNum( seqNum_ )
						, param( param_ )
					{
					}

					QString name;
					quint32 seqNum;
					rec::rpc::serialization::SerializablePtrConst param;
				};

				class SerializeRPCResponseEvent : public QEvent
				{
				public:
					SerializeRPCResponseEvent( const QString& name_, quint32 seqNum_, rec::rpc::ErrorCode errorCode_, rec::rpc::serialization::SerializablePtrConst result_, quintptr receiver_ )
						: QEvent( (QEvent::Type)rec::rpc::serialization::SingleThreadedSerializer::SerializeRPCResponseEventId )
						, name( name_ )
						, seqNum( seqNum_ )
						, errorCode( errorCode_ )
						, result( result_ )
						, receiver( receiver_ )
					{
					}

					QString name;
					quint32 seqNum;
					rec::rpc::ErrorCode errorCode;
					rec::rpc::serialization::SerializablePtrConst result;
					quintptr receiver;
				};

				class SerializeTopicDataEvent : public QEvent
				{
				public:
					SerializeTopicDataEvent( unsigned int id_, rec::rpc::serialization::SerializablePtrConst data_ )
						: QEvent( (QEvent::Type)rec::rpc::serialization::SingleThreadedSerializer::SerializeTopicDataEventId )
						, id( id_ )
						, data( data_ )
					{
					}

					unsigned int id;
					rec::rpc::serialization::SerializablePtrConst data;
				};

				void customEvent( QEvent* e );
			};
		}
	}
}

#endif //_REC_RPC_SERIALIZATION_SINGLETHREADEDSERIALIZER_H_
