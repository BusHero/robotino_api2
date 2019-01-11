//  Copyright (C) 2004-2013, Robotics Equipment Corporation GmbH

//Copyright (c) ...
//
//REC Robotics Equipment Corporation GmbH, Planegg, Germany. All rights reserved.
//Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//
//THIS SOFTWARE IS PROVIDED BY REC ROBOTICS EQUIPMENT CORPORATION GMBH ï¿½AS ISï¿½ AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL REC ROBOTICS EQUIPMENT CORPORATION GMBH
//BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
//GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
//LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//Copyright (c) ...
//
//REC Robotics Equipment Corporation GmbH, Planegg, Germany. Alle Rechte vorbehalten.
//Weiterverbreitung und Verwendung in nichtkompilierter oder kompilierter Form, mit oder ohne Verï¿½nderung, sind unter den folgenden Bedingungen zulï¿½ssig:
//1) Weiterverbreitete nichtkompilierte Exemplare mï¿½ssen das obige Copyright, diese Liste der Bedingungen und den folgenden Haftungsausschluss im Quelltext enthalten.
//2) Weiterverbreitete kompilierte Exemplare mï¿½ssen das obige Copyright, diese Liste der Bedingungen und den folgenden Haftungsausschluss in der Dokumentation und/oder anderen Materialien, die mit dem Exemplar verbreitet werden, enthalten.
//
//DIESE SOFTWARE WIRD VON REC ROBOTICS EQUIPMENT CORPORATION GMBH OHNE JEGLICHE SPEZIELLE ODER IMPLIZIERTE GARANTIEN ZUR VERFï¿½GUNG GESTELLT, DIE UNTER
//ANDEREM EINSCHLIESSEN: DIE IMPLIZIERTE GARANTIE DER VERWENDBARKEIT DER SOFTWARE FÜR EINEN BESTIMMTEN ZWECK. AUF KEINEN FALL IST REC ROBOTICS EQUIPMENT CORPORATION GMBH
//FÜR IRGENDWELCHE DIREKTEN, INDIREKTEN, ZUFÄLLIGEN, SPEZIELLEN, BEISPIELHAFTEN ODER FOLGESCHÄDEN (UNTER ANDEREM VERSCHAFFEN VON ERSATZGÜTERN ODER -DIENSTLEISTUNGEN;
//EINSCHRÄNKUNG DER NUTZUNGSFÄHIGKEIT; VERLUST VON NUTZUNGSFÄHIGKEIT; DATEN; PROFIT ODER GESCHÄFTSUNTERBRECHUNG), WIE AUCH IMMER VERURSACHT UND UNTER WELCHER VERPFLICHTUNG
//AUCH IMMER, OB IN VERTRAG, STRIKTER VERPFLICHTUNG ODER UNERLAUBTER HANDLUNG (INKLUSIVE FAHRLÄSSIGKEIT) VERANTWORTLICH, AUF WELCHEM WEG SIE AUCH IMMER DURCH DIE BENUTZUNG
//DIESER SOFTWARE ENTSTANDEN SIND, SOGAR, WENN SIE AUF DIE MÖGLICHKEIT EINES SOLCHEN SCHADENS HINGEWIESEN WORDEN SIND.

#include "rec/robotino/api2/Client.hpp"

using namespace rec::robotino::api2;

Client::Client()
{
	bool connected = true;

	connected &= (bool)connect( this, SIGNAL( is_connected() ), SLOT( on_is_connected() ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( is_disconnected() ), SLOT( on_is_disconnected() ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( error( QAbstractSocket::SocketError, const QString& ) )
		, SLOT( on_error( QAbstractSocket::SocketError, const QString& ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( connectToServer_signal( unsigned int ) )
		, SLOT( connectToServer( unsigned int ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( disconnectFromServer_signal() )
		, SLOT( disconnectFromServer() ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( setAutoReconnectEnabled_signal( bool, unsigned int ) )
		, SLOT( setAutoReconnectEnabled( bool, unsigned int ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );
	
	connected &= (bool)connect( this, SIGNAL( setName_signal( const QString& ) )
		, SLOT( setName( const QString& ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( setMultiThreadedSerializationEnabled_signal( bool ) )
		, SLOT( setMultiThreadedSerializationEnabled( bool ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( setLocalIPCEnabled_signal( bool ) )
		, SLOT( setLocalIPCEnabled( bool ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );
}

Client::~Client()
{
	{
		QMutexLocker lk( &_mutex );
	}

	_connectCondition.wakeAll();
}

bool Client::connectToServerWrapper( bool isBlocking )
{
	QMutexLocker lk( &_mutex );

	if( isConnected() )
	{
		Q_EMIT disconnectFromServer_signal();
		if( isBlocking )
		{
			_connectCondition.wait( &_mutex );
		}
	}

	Q_EMIT connectToServer_signal( 2000 );

	if( isBlocking )
	{
		_connectCondition.wait( &_mutex );
	}

	return isConnected();
}

void Client::disconnectFromServerWrapper()
{
	QMutexLocker lk( &_mutex );

	if( false == isConnected() )
	{
		return;
	}

	Q_EMIT disconnectFromServer_signal();
	_connectCondition.wait( &_mutex );
}

void Client::setAutoReconnectEnabledWrapper( bool enable, unsigned int ms )
{
	Q_EMIT setAutoReconnectEnabled_signal( enable, ms );
}

void Client::setNameWrapper( const QString& name )
{
	Q_EMIT setName_signal( name );
}

void Client::setMultiThreadedSerializationEnabledWrapper( bool enabled )
{
	Q_EMIT setMultiThreadedSerializationEnabled_signal( enabled );
}

void Client::setLocalIPCEnabledWrapper( bool enabled )
{
	Q_EMIT setLocalIPCEnabled_signal( enabled );
}

const char* Client::address() const
{
	if ( !_address.hasLocalData() )
		_address.setLocalData( new QByteArray );
	QByteArray* ba = _address.localData();
	Q_ASSERT( ba != 0 );
	*ba = rec::robotino::rpc::Client::address().toLatin1();

	return ba->constData();
}

unsigned int Client::elapsed() const
{
	return _timer.elapsed();
}

void Client::on_is_connected()
{
	{
		QMutexLocker lk( &_mutex );
	}
	_timer.start();
	_connectCondition.wakeAll();
}

void Client::on_is_disconnected()
{
	{
		QMutexLocker lk( &_mutex );
	}
	_timer = QTime();
	_connectCondition.wakeAll();
}

void Client::on_error( QAbstractSocket::SocketError socketError, const QString& errorString )
{
	QMutexLocker lk( &_mutex );
	_connectCondition.wakeAll();
}
