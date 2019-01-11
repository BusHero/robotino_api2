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

#include "rec/rpc/serialization/rec_rpc_serialization_MultiThreadedSerializer.hpp"

using namespace rec::rpc::serialization;

SerializeRPCRequestRunnable::SerializeRPCRequestRunnable( const QString& name, quint32 seqNum, rec::rpc::serialization::SerializablePtrConst param )
	: _name( name )
	, _seqNum( seqNum )
	, _param( param )
{
}

void SerializeRPCRequestRunnable::run()
{
	Q_EMIT finished( _name, _seqNum, Serializer::serializeData( *_param ) );
}

SerializeRPCResponseRunnable::SerializeRPCResponseRunnable( const QString& name, quint32 seqNum, rec::rpc::ErrorCode errorCode, rec::rpc::serialization::SerializablePtrConst result, quintptr receiver )
	: _name( name )
	, _seqNum( seqNum )
	, _errorCode( errorCode )
	, _result( result )
	, _receiver( receiver )
{
}

void SerializeRPCResponseRunnable::run()
{
	Q_EMIT finished( _name, _seqNum, _errorCode, Serializer::serializeData( *_result ), _receiver );
}

SerializeTopicDataRunnable::SerializeTopicDataRunnable( unsigned int id, rec::rpc::serialization::SerializablePtrConst data )
	: _id( id )
	, _data( data )
{
}

void SerializeTopicDataRunnable::run()
{
	Q_EMIT finished( _id, Serializer::serializeData( *_data ) );
}

MultiThreadedSerializer::MultiThreadedSerializer( QObject* parent )
	: Serializer( parent )
{
}

void MultiThreadedSerializer::serializeRPCRequest( const QString& name, quint32 seqNum, rec::rpc::serialization::SerializablePtrConst param )
{
	SerializeRPCRequestRunnable* func = new SerializeRPCRequestRunnable( name, seqNum, param );
	connect( func, SIGNAL( finished( const QString&, quint32, const QByteArray& ) ),
		SIGNAL( RPCRequestSerialized( const QString&, quint32, const QByteArray& ) ), Qt::QueuedConnection );
	QThreadPool::globalInstance()->start( func );
}

void MultiThreadedSerializer::serializeRPCResponse( const QString& name, quint32 seqNum, rec::rpc::ErrorCode errorCode, rec::rpc::serialization::SerializablePtrConst result, quintptr receiver )
{
	SerializeRPCResponseRunnable* func = new SerializeRPCResponseRunnable( name, seqNum, errorCode, result, receiver );
	connect( func, SIGNAL( finished( const QString&, quint32, rec::rpc::ErrorCode, const QByteArray&, quintptr ) ),
		SIGNAL( RPCResponseSerialized( const QString&, quint32, rec::rpc::ErrorCode, const QByteArray&, quintptr ) ), Qt::QueuedConnection );
	QThreadPool::globalInstance()->start( func );
}

void MultiThreadedSerializer::serializeTopicData( unsigned int id, rec::rpc::serialization::SerializablePtrConst data )
{
	SerializeTopicDataRunnable* func = new SerializeTopicDataRunnable( id, data );
	connect( func, SIGNAL( finished( unsigned int, const QByteArray& ) ),
		SIGNAL( topicDataSerialized( unsigned int, const QByteArray& ) ), Qt::QueuedConnection );
	QThreadPool::globalInstance()->start( func );
}
