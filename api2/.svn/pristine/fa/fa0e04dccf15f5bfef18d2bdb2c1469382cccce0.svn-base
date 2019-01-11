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

#include "rec/robotino/api2/Kinect.h"
#include "rec/robotino/api2/KinectImpl.hpp"
#include "rec/robotino/api2/RobotinoException.h"
#include "rec/robotino/api2/WorkerThread.hpp"

using namespace rec::robotino::api2;

Kinect::Kinect()
: _impl( new KinectImpl( this ) )
{
	try
	{
		setComId( _comID );
	}
	catch( const rec::robotino::api2::RobotinoException& )
	{
	}
}

Kinect::~Kinect()
{
	try
	{
		QMutexLocker lk( &_impl->_mutex );
		WorkerThread* thread = WorkerThread::instance( comId() );
		thread->removeKinectReader( _impl );
	}
	catch( const rec::robotino::api2::RobotinoException& )
	{
	}

	delete _impl;
}

void Kinect::setComId( const ComId& id )
{
	QMutexLocker lk( &_impl->_mutex );

	try
	{
		WorkerThread* thread = WorkerThread::instance( comId() );
		thread->removeKinectReader( _impl );
	}
	catch( const rec::robotino::api2::RobotinoException& )
	{
	}

	_comID = id;

	if( ComId::null() == comId() )
	{
		return;
	}

	if( _impl->_number > -1 )
	{
		WorkerThread* thread = WorkerThread::instance( comId() );
		thread->addKinectReader( _impl, _impl->_number );
	}
}

unsigned int Kinect::numKinects()
{
	return WorkerThread::numKinects;
}

int Kinect::kinectNumber() const
{
	QMutexLocker lk( &_impl->_mutex );
	return _impl->_number;
}

void Kinect::setKinectNumber( int number )
{
	if( number < 0 )
	{
		number = -1;
	}

	if( number >= (int)numKinects() )
	{
		throw RobotinoException( "Kinect number out of range" );
	}
	QMutexLocker lk( &_impl->_mutex );
	if( number != _impl->_number )
	{
		_impl->_number = number;

		WorkerThread* thread = WorkerThread::instance( comId() );
		thread->removeKinectReader( _impl );

		if( number > -1 )
		{
			thread->addKinectReader( _impl, _impl->_number );
		}
	}
}

void Kinect::processEvents()
{
	try
	{
		QMutexLocker lk( &_impl->_mutex );
		WorkerThread* thread = WorkerThread::instance( comId() );
		thread->processKinectEvents( _impl->_number );
	}
	catch( const rec::robotino::api2::RobotinoException& )
	{
	}
}

void Kinect::setTilt( double angleDeg )
{
	QMutexLocker lk( &_impl->_mutex );
	WorkerThread* thread = WorkerThread::instance( comId() );

	thread->set_kinect_set_tilt( _impl->_number, angleDeg );
}

void Kinect::setLED( unsigned int state )
{
	QMutexLocker lk( &_impl->_mutex );
	WorkerThread* thread = WorkerThread::instance( comId() );

	thread->set_kinect_set_led( _impl->_number, state );
}

void Kinect::setDepthFormat( unsigned int format )
{
	QMutexLocker lk( &_impl->_mutex );
	WorkerThread* thread = WorkerThread::instance( comId() );

	thread->set_kinect_set_depth_format( _impl->_number, format );
}

void Kinect::setVideoFormat( unsigned int format )
{
	QMutexLocker lk( &_impl->_mutex );
	WorkerThread* thread = WorkerThread::instance( comId() );

	thread->set_kinect_set_video_format( _impl->_number, format );
}

void Kinect::setBGREnabled( bool enable )
{
	QMutexLocker lk( &_impl->_mutex );
	_impl->_bgrEnabled = enable;
}

bool Kinect::isBGRenabled() const
{
	QMutexLocker lk( &_impl->_mutex );
	return _impl->_bgrEnabled;
}

void Kinect::depthEvent( const unsigned short* data, unsigned int dataSize, const unsigned short* object_data, unsigned int object_dataSize, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
}

void Kinect::videoEvent( const unsigned char* data, unsigned int dataSize, unsigned int width, unsigned int height, unsigned int step, unsigned int format, unsigned int stamp )
{
}

void Kinect::tiltEvent( double angleDeg )
{
}

void Kinect::accelEvent( double x, double y, double z )
{
}

