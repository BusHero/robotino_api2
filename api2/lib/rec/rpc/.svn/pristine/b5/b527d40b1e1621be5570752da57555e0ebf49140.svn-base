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

#ifndef _REC_RPC_SERIALIZATION_MULTITHREADEDSERIALIZER_H_
#define _REC_RPC_SERIALIZATION_MULTITHREADEDSERIALIZER_H_

#include "rec_rpc_serialization_Serializer.hpp"

namespace rec
{
	namespace rpc
	{
		namespace serialization
		{
			class SerializeRPCRequestRunnable : public QObject, public QRunnable
			{
				Q_OBJECT
			public:
				SerializeRPCRequestRunnable( const QString& name, quint32 seqNum, rec::rpc::serialization::SerializablePtrConst param );
				void run();

			Q_SIGNALS:
				void finished( const QString&, quint32, const QByteArray& );

			private:
				QString _name;
				quint32 _seqNum;
				rec::rpc::serialization::SerializablePtrConst _param;
			};

			class SerializeRPCResponseRunnable : public QObject, public QRunnable
			{
				Q_OBJECT
			public:
				SerializeRPCResponseRunnable( const QString& name, quint32 seqNum, rec::rpc::ErrorCode errorCode, rec::rpc::serialization::SerializablePtrConst result, quintptr receiver );
				void run();

			Q_SIGNALS:
				void finished( const QString&, quint32, rec::rpc::ErrorCode, const QByteArray&, quintptr );

			private:
				QString _name;
				quint32 _seqNum;
				rec::rpc::ErrorCode _errorCode;
				rec::rpc::serialization::SerializablePtrConst _result;
				quintptr _receiver;
			};

			class SerializeTopicDataRunnable : public QObject, public QRunnable
			{
				Q_OBJECT
			public:
				SerializeTopicDataRunnable( unsigned int id, rec::rpc::serialization::SerializablePtrConst data );
				void run();

			Q_SIGNALS:
				void finished( unsigned int id, const QByteArray& );

			private:
				unsigned int _id;
				rec::rpc::serialization::SerializablePtrConst _data;
			};

			class MultiThreadedSerializer : public Serializer
			{
			public:
				MultiThreadedSerializer( QObject* parent = 0 );

				void serializeRPCRequest( const QString& name, quint32 seqNum, rec::rpc::serialization::SerializablePtrConst param );
				void serializeRPCResponse( const QString& name, quint32 seqNum, rec::rpc::ErrorCode errorCode, rec::rpc::serialization::SerializablePtrConst result, quintptr receiver );
				void serializeTopicData( unsigned int id, rec::rpc::serialization::SerializablePtrConst data );
			};
		}
	}
}

#endif //_REC_RPC_SERIALIZATION_MULTITHREADEDSERIALIZER_H_
