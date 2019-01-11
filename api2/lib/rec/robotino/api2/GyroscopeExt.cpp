//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

//Copyright (c) ...
//
//REC Robotics Equipment Corporation GmbH, Planegg, Germany. All rights reserved.
//Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//
//THIS SOFTWARE IS PROVIDED BY REC ROBOTICS EQUIPMENT CORPORATION GMBH �AS IS� AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL REC ROBOTICS EQUIPMENT CORPORATION GMBH
//BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
//GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
//LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//Copyright (c) ...
//
//REC Robotics Equipment Corporation GmbH, Planegg, Germany. Alle Rechte vorbehalten.
//Weiterverbreitung und Verwendung in nichtkompilierter oder kompilierter Form, mit oder ohne Ver�nderung, sind unter den folgenden Bedingungen zul�ssig:
//1) Weiterverbreitete nichtkompilierte Exemplare m�ssen das obige Copyright, diese Liste der Bedingungen und den folgenden Haftungsausschluss im Quelltext enthalten.
//2) Weiterverbreitete kompilierte Exemplare m�ssen das obige Copyright, diese Liste der Bedingungen und den folgenden Haftungsausschluss in der Dokumentation und/oder anderen Materialien, die mit dem Exemplar verbreitet werden, enthalten.
//
//DIESE SOFTWARE WIRD VON REC ROBOTICS EQUIPMENT CORPORATION GMBH OHNE JEGLICHE SPEZIELLE ODER IMPLIZIERTE GARANTIEN ZUR VERF�GUNG GESTELLT, DIE UNTER
//ANDEREM EINSCHLIESSEN: DIE IMPLIZIERTE GARANTIE DER VERWENDBARKEIT DER SOFTWARE F�R EINEN BESTIMMTEN ZWECK. AUF KEINEN FALL IST REC ROBOTICS EQUIPMENT CORPORATION GMBH
//F�R IRGENDWELCHE DIREKTEN, INDIREKTEN, ZUF�LLIGEN, SPEZIELLEN, BEISPIELHAFTEN ODER FOLGESCH�DEN (UNTER ANDEREM VERSCHAFFEN VON ERSATZG�TERN ODER -DIENSTLEISTUNGEN;
//EINSCHR�NKUNG DER NUTZUNGSF�HIGKEIT; VERLUST VON NUTZUNGSF�HIGKEIT; DATEN; PROFIT ODER GESCH�FTSUNTERBRECHUNG), WIE AUCH IMMER VERURSACHT UND UNTER WELCHER VERPFLICHTUNG
//AUCH IMMER, OB IN VERTRAG, STRIKTER VERPFLICHTUNG ODER UNERLAUBTER HANDLUNG (INKLUSIVE FAHRL�SSIGKEIT) VERANTWORTLICH, AUF WELCHEM WEG SIE AUCH IMMER DURCH DIE BENUTZUNG
//DIESER SOFTWARE ENTSTANDEN SIND, SOGAR, WENN SIE AUF DIE M�GLICHKEIT EINES SOLCHEN SCHADENS HINGEWIESEN WORDEN SIND.

#include "rec/robotino/api2/GyroscopeExt.h"
#include "rec/robotino/api2/GyroscopeExtImpl.hpp"
#include "rec/robotino/api2/RobotinoException.h"
#include "rec/robotino/api2/WorkerThread.hpp"

using namespace rec::robotino::api2;

GyroscopeExt::GyroscopeExt()
: _impl( new GyroscopeExtImpl( this ) )
{
	try
	{
		setComId( _comID );
	}
	catch( const rec::robotino::api2::RobotinoException& )
	{
	}
}

GyroscopeExt::~GyroscopeExt()
{
	try
	{
		QMutexLocker lk( &_impl->_mutex );
		WorkerThread* thread = WorkerThread::instance( comId() );
		thread->removeGyroscopeExtReader( _impl );
	}
	catch( const rec::robotino::api2::RobotinoException& )
	{
	}

	delete _impl;
}

void GyroscopeExt::setComId( const ComId& id )
{
	QMutexLocker lk( &_impl->_mutex );

	try
	{
		WorkerThread* thread = WorkerThread::instance( comId() );
		thread->removeGyroscopeExtReader( _impl );
	}
	catch( const rec::robotino::api2::RobotinoException& )
	{
	}

	_comID = id;

	if( ComId::null() == comId() )
	{
		return;
	}
	
	{
		WorkerThread* thread = WorkerThread::instance( comId() );
		thread->addGyroscopeExtReader( _impl );
	}
}

void GyroscopeExt::processEvents()
{
	try
	{
		QMutexLocker lk( &_impl->_mutex );
		WorkerThread* thread = WorkerThread::instance( comId() );
		thread->processGyroscopeExtEvents();
	}
	catch( const rec::robotino::api2::RobotinoException& )
	{
	}
}

float GyroscopeExt::angle() const
{
	QMutexLocker lk( &_impl->_mutex );
	return _impl->_angle;
}

float GyroscopeExt::rate() const
{
	QMutexLocker lk( &_impl->_mutex );
	return _impl->_rate;
}

void GyroscopeExt::gyroscopeExtEvent(float angle, float rate)
{
}

