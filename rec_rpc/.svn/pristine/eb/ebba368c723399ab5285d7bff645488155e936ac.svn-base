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

#include "Client.h"

Client::Client()
: _blocking( false )
{
	// Register the notifiers.
	REGISTER_NOTIFIER( add );
	REGISTER_NOTIFIER( sub );
	REGISTER_NOTIFIER( mult );
	REGISTER_NOTIFIER( div );

	setName( "REC RPC test client" );
	setMultiThreadedSerializationEnabled( true );
	setExpectedGreeting( "REC RPC test server" );
}

Client::~Client()
{
}

void Client::setTopicsEnabled( bool enabled )
{	// Register or unregister topic listener and topic info listener for topic "image".
	if ( enabled )
	{
		try
		{
			REGISTER_TOPICLISTENER( image );
			REGISTER_TOPICINFOCHANGED( image );
			REGISTER_TOPICLISTENER( text );
			REGISTER_TOPICLISTENER( text2 );
		}
		catch( const rec::rpc::Exception& e )
		{
			Q_EMIT log( QString( "Failed to register topic listeners: " ) + e.getMessage() );
		}
	}
	else
	{
		try
		{
			UNREGISTER_TOPICLISTENER( image );
			UNREGISTER_TOPICINFOCHANGED( image );
			UNREGISTER_TOPICLISTENER( text );
			UNREGISTER_TOPICLISTENER( text2 );
		}
		catch( const rec::rpc::Exception& e )
		{
			Q_EMIT log( QString( "Failed to unregister topic listeners: " ) + e.getMessage() );
		}
	}
}

bool Client::blocking() const
{
	return _blocking;
}

void Client::setBlocking( bool blocking )
{
	_blocking = blocking;
}

float Client::add( float a, float b )
{	// RPC function invokation. The parameter type is complex, so INVOKE_SIMPLE does not work here.
	PREPARE( add );			// Declare and initialize parameter and result.
	param.a() = a;			// Assign parameter values.
	param.b() = b;
	INVOKE( _blocking );	// Invoke the function.
							// If the call is blocking, a rec::rpc::Exception will be thrown in case of an error.
	return result;			// The result is stored in "result".
}

float Client::sub( float a, float b )
{
	rec::rpc::serialization::SerializablePtr s;
	s= rec::rpc::serialization::Serializable::empty;
	PREPARE( sub );
	param.a() = a;
	param.b() = b;
	INVOKE( _blocking );
	return result;
}

float Client::mult( float a, float b )
{
	PREPARE( mult );
	param.a() = a;
	param.b() = b;
	INVOKE( _blocking );
	return result;
}

float Client::div( float a, float b )
{
	PREPARE( div );
	param.a() = a;
	param.b() = b;
	INVOKE( _blocking );
	return result;
}

void Client::publishImage( const QImage& img )
{	// Publish the given QImage in the topic "image".
	PUBLISH_TOPIC_SIMPLE( image, img );
}

void Client::publishText( const QString& text )
{
	PUBLISH_TOPIC_SIMPLE( text, text );
}

void Client::publishText2( const QString& text )
{
	PUBLISH_TOPIC_SIMPLE( text2, text );
}

// Implementation of the notifiers.
BEGIN_NOTIFIER( Client, add )
	// Just notify the GUI that a result has been received.
	Q_EMIT calcResultReceived( result, errorCode );
END_NOTIFIER

BEGIN_NOTIFIER( Client, sub )
	Q_EMIT calcResultReceived( result, errorCode );
END_NOTIFIER

BEGIN_NOTIFIER( Client, mult )
	Q_EMIT calcResultReceived( result, errorCode );
END_NOTIFIER

BEGIN_NOTIFIER( Client, div )
	Q_EMIT calcResultReceived( result, errorCode );
END_NOTIFIER

BEGIN_TOPICLISTENER_DEFINITION( Client, image )
	// Topic listener definition for "image". Just notify the GUI that there is a new image.
	Q_EMIT log( QString( "Image received from %1:%2" ).arg( client.address.toString() ).arg( client.port ) );
	Q_EMIT imageReceived( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, image )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT imageInfoChanged( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, text )
	// Topic listener definition for "text".
	Q_EMIT log( QString( "Text received from %1:%2 - %3" ).arg( client.address.toString() ).arg( client.port ).arg( data.ref() ) );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, text2 )
	// Topic listener definition for "text2".
	Q_EMIT log( QString( "Text received from %1:%2 - %3" ).arg( client.address.toString() ).arg( client.port ).arg( data.ref() ) );
END_TOPICLISTENER_DEFINITION
