//  Copyright (C) 2004-2013, Robotics Equipment Corporation GmbH

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

#include "rec/robotino/api2/Motor.h"
#include "rec/robotino/api2/MotorImpl.hpp"
#include "rec/robotino/api2/RobotinoException.h"
#include "rec/robotino/api2/WorkerThread.hpp"

using namespace rec::robotino::api2;

Motor::Motor()
: _impl( new MotorImpl( this ) )
{
	try
	{
		setComId( _comID );
	}
	catch( const rec::robotino::api2::RobotinoException& )
	{
	}
}

Motor::~Motor()
{
	try
	{
		QMutexLocker lk( &_impl->_mutex );
		WorkerThread* thread = WorkerThread::instance( comId() );
		thread->removeMotorReader( _impl );
	}
	catch( const rec::robotino::api2::RobotinoException& )
	{
	}

	delete _impl;
}

unsigned int Motor::numMotors()
{
	return 3;
}

void Motor::setComId( const ComId& id )
{
	QMutexLocker lk( &_impl->_mutex );

	try
	{
		WorkerThread* thread = WorkerThread::instance( comId() );
		thread->removeMotorReader( _impl );
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
		thread->addMotorReader( _impl );
	}
}

void Motor::setMotorNumber( unsigned int n )
{
	if( n >= numMotors() )
		throw RobotinoException( "Invalid motor number" );

	QMutexLocker lk( &_impl->_mutex );
	_impl->_motorNumber = n;
}

void Motor::setSpeedSetPoint( float rpm )
{
	QMutexLocker lk( &_impl->_mutex );
	if( _impl->_brake )
	{
		return;
	}

	WorkerThread* thread = WorkerThread::instance( comId() );

	switch( _impl->_motorNumber )
	{
	case 0:
		thread->set_motor0_setpoint( rpm );
		break;

	case 1:
		thread->set_motor1_setpoint( rpm );
		break;

	case 2:
		thread->set_motor2_setpoint( rpm );
		break;

	default:
		throw RobotinoException( "Invalid motor number" );
		break;
	}
}

void Motor::resetPosition( int position )
{
	WorkerThread* thread = WorkerThread::instance( comId() );

	switch( _impl->_motorNumber )
	{
	case 0:
		thread->motor0_reset_position( position );
		break;

	case 1:
		thread->motor1_reset_position( position );
		break;

	case 2:
		thread->motor2_reset_position( position );
		break;

	default:
		throw RobotinoException( "Invalid motor number" );
		break;
	}
}

void Motor::setBrake( bool brake )
{
	QMutexLocker lk( &_impl->_mutex );
	if( brake )
	{
		setSpeedSetPoint( 0 );
	}

	_impl->_brake = brake;
}

void Motor::setPID( float kp, float ki, float kd )
{
	QMutexLocker lk( &_impl->_mutex );
	WorkerThread* thread = WorkerThread::instance( comId() );
	thread->set_set_pid_parameters( _impl->_motorNumber, kp, ki, kd );
}

float Motor::actualVelocity() const
{
	QMutexLocker lk( &_impl->_mutex );
	return _impl->_speed;
}

int Motor::actualPosition() const
{
	QMutexLocker lk( &_impl->_mutex );
	return _impl->_position;
}

float Motor::motorCurrent() const
{
	QMutexLocker lk( &_impl->_mutex );
	return _impl->_current;
}

void Motor::motorReadingsChanged( float velocity, int position, float current )
{
}
