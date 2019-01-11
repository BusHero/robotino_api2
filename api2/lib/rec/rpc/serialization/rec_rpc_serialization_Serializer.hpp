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

#ifndef _REC_RPC_SERIALIZATION_SERIALIZER_H_
#define _REC_RPC_SERIALIZATION_SERIALIZER_H_

#include "rec/rpc/rec_rpc_Exception.h"
#include "rec/rpc/serialization/rec_rpc_serialization_Serializable.h"

#include <QtCore>

namespace rec
{
	namespace rpc
	{
		namespace serialization
		{
			class Serializer : public QObject
			{
				Q_OBJECT
			public:
				Serializer( QObject* parent );

				virtual void serializeRPCRequest( const QString& name, quint32 seqNum, rec::rpc::serialization::SerializablePtrConst param ) = 0;
				virtual void serializeRPCResponse( const QString& name, quint32 seqNum, rec::rpc::ErrorCode errorCode, rec::rpc::serialization::SerializablePtrConst result, quintptr receiver ) = 0;
				virtual void serializeTopicData( unsigned int id, rec::rpc::serialization::SerializablePtrConst data ) = 0;

				static QByteArray serializeData( const rec::rpc::serialization::Serializable& data );
				static void deserializeData( const QByteArray& serializedData, rec::rpc::serialization::Serializable& data );

			Q_SIGNALS:
				void RPCRequestSerialized( const QString&, quint32, const QByteArray& );
				void RPCResponseSerialized( const QString&, quint32, rec::rpc::ErrorCode, const QByteArray&, quintptr );
				void topicDataSerialized( unsigned int, const QByteArray& );
			};
		}
	}
}

#endif //_REC_RPC_SERIALIZATION_SERIALIZER_H_
