//  Copyright (C) 2004-2009, Robotics Equipment Corporation GmbH

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

#include "rec/robotino/api2/Com.h"
#include "rec/robotino/api2/Client.hpp"
#include "rec/robotino/api2/WorkerThread.hpp"
#include <QtNetwork>

using namespace rec::robotino::api2;

Com::Com()
{
	WorkerThread::initQt();
	_thread = new WorkerThread( this );
#ifdef WIN32
	_thread->setLocalIPCEnabled(false);
#endif
}

Com::Com( const char* name )
{
	WorkerThread::initQt();
	_thread = new WorkerThread( this );
	_thread->setName( name );
#ifdef WIN32
	_thread->setLocalIPCEnabled(false);
#endif
}

Com::Com( const char* name, bool multiThreadedSerialization )
{
	WorkerThread::initQt();
	_thread = new WorkerThread( this );
	_thread->setName( name );
	_thread->setMultiThreadedSerializationEnabled( multiThreadedSerialization );
#ifdef WIN32
	_thread->setLocalIPCEnabled(false);
#endif
}

Com::Com( const char* name, bool multiThreadedSerialization, bool localIPCEnabled )
{
	WorkerThread::initQt();
	_thread = new WorkerThread( this );
	_thread->setName( name );
	_thread->setMultiThreadedSerializationEnabled( multiThreadedSerialization );
	_thread->setLocalIPCEnabled( localIPCEnabled );
}


Com::~Com()
{
	delete _thread;
}

ComId Com::id() const
{
	return _thread->comid;
}

void Com::connectToServer( bool isBlocking )
{
	_thread->connectToServer( isBlocking );
}

void Com::disconnectFromServer()
{
	_thread->disconnectFromServer();
}

bool Com::isConnected() const
{
	return _thread->isConnected();
}

void Com::setAutoReconnectEnabled( bool enable )
{
	_thread->setAutoReconnectEnabled( enable );
}

void Com::setAddress( const char* address )
{
	_thread->setAddress( address );
}

const char* Com::address() const
{
	return _thread->address();
}

bool Com::isLocalConnection() const
{
	return _thread->isLocalConnection();
}

void Com::setName( const char* name )
{
	_thread->setName( name );
}

void Com::setMultiThreadedSerializationEnabled( bool enabled )
{
	_thread->setMultiThreadedSerializationEnabled( enabled );
}

unsigned int Com::msecsElapsed() const
{
	return _thread->elapsed();
}

void Com::processEvents()
{
	_thread->processEvents();
}

void Com::processComEvents()
{
	_thread->processComEvents();
}

void Com::processPowerButtonEvents()
{
	_thread->processPowerButtonEvents();
}

void Com::processFactory4Events()
{
	_thread->processFactory4Events();
}

void Com::processChargerEvents()
{
	_thread->processChargerEvents();
}

void Com::errorEvent( const char* errorString )
{
}

void Com::connectedEvent()
{
}

void Com::connectionClosedEvent()
{
}

void Com::logEvent( const char* message, int level )
{
}
