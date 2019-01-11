/*
Copyright (c) 2013, REC Robotics Equipment Corporation GmbH, Planegg, Germany
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

#include "rec_rpc_client_Serializer.hpp"
#include "rec/rpc/serialization/rec_rpc_serialization_SingleThreadedSerializer.hpp"
#include "rec/rpc/serialization/rec_rpc_serialization_MultiThreadedSerializer.hpp"

using namespace rec::rpc::client;

Serializer::Serializer( bool multiThreaded, QObject* parent )
	: QObject( parent )
	, _serializer( 0 )
{
	setSerializer( multiThreaded );
}

Serializer::~Serializer()
{
	delete _serializer;
}

void Serializer::setup( bool multiThreaded )
{
	qApp->postEvent( this, new SetupEvent( multiThreaded ) );
}

void Serializer::setSerializer( bool multiThreaded )
{
	delete _serializer;
	_serializer = 0;
	if ( multiThreaded )
		_serializer = new rec::rpc::serialization::MultiThreadedSerializer;
	else
		_serializer = new rec::rpc::serialization::SingleThreadedSerializer;

	bool ok = true;
	ok &= (bool)QObject::connect( _serializer,
		SIGNAL( RPCRequestSerialized( const QString&, quint32, const QByteArray& ) ),
		SIGNAL( RPCRequestSerialized( const QString&, quint32, const QByteArray& ) ),
		Qt::DirectConnection );

	ok &= (bool)QObject::connect( _serializer,
		SIGNAL( topicDataSerialized( unsigned int, const QByteArray& ) ),
		SIGNAL( topicDataSerialized( unsigned int, const QByteArray& ) ),
		Qt::DirectConnection );
	Q_ASSERT( ok );
}

void Serializer::customEvent( QEvent* e )
{
	if ( e->type() == QEvent::User )
	{
		setSerializer( static_cast< SetupEvent* >( e )->multiThreaded );
	}
}

void Serializer::serializeRPCRequest( const QString& name, quint32 seqNum, rec::rpc::serialization::SerializablePtrConst param )
{
	Q_ASSERT( _serializer );
	_serializer->serializeRPCRequest( name, seqNum, param );
}

void Serializer::serializeTopicData( unsigned int id, rec::rpc::serialization::SerializablePtrConst data )
{
	Q_ASSERT( _serializer );
	_serializer->serializeTopicData( id, data );
}

QByteArray Serializer::serializeData( const rec::rpc::serialization::Serializable& data )
{
	return rec::rpc::serialization::Serializer::serializeData( data );
}

void Serializer::deserializeData( const QByteArray& serializedData, rec::rpc::serialization::Serializable& data )
{
	rec::rpc::serialization::Serializer::deserializeData( serializedData, data );
}
