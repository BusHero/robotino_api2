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

#include "rec/robotino/api2/Camera.h"
#include "rec/robotino/api2/CameraImpl.hpp"
#include "rec/robotino/api2/RobotinoException.h"
#include "rec/robotino/api2/WorkerThread.hpp"
#include <algorithm>

#include "rec/robotino/api2/ComObjectHelper.hpp"

using namespace rec::robotino::api2;

COMOBJECT_CONSTRUCTORS(Camera)
COMOBJECT_DESTRUCTOR(Camera)

void Camera::setComId( const ComId& id )
{
	QMutexLocker lk( &_impl->_mutex );

	try
	{
		WorkerThread* thread = WorkerThread::instance( comId() );
		thread->removeCameraReader( _impl );
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
		thread->addCameraReader( _impl, _impl->_number );
	}
}

unsigned int Camera::numCameras()
{
	return WorkerThread::numCameras;
}

int Camera::cameraNumber() const
{
	QMutexLocker lk( &_impl->_mutex );
	return _impl->_number;
}

void Camera::setJPGDecodingEnabled(bool enable)
{
	QMutexLocker lk(&_impl->_mutex);
	_impl->_jpgDecodingEnabled = enable;
}

bool Camera::isJPGDecodingEnabled() const
{
	QMutexLocker lk(&_impl->_mutex);
	return _impl->_jpgDecodingEnabled;
}

void Camera::setCameraNumber( int number )
{
	if( number < 0 )
	{
		number = -1;
	}

	if( number >= (int)numCameras() )
	{
		throw RobotinoException( "camera number out of range" );
	}
	QMutexLocker lk( &_impl->_mutex );
	if( number != _impl->_number )
	{
		_impl->_number = number;

		WorkerThread* thread = WorkerThread::instance( comId() );
		thread->removeCameraReader( _impl );

		_impl->_capabilities = CameraCapabilities();
		capabilitiesChangedEvent( _impl->_capabilities );

		_impl->_settings_width = 0;
		_impl->_settings_height = 0;
		_impl->_settings_format.clear();
		settingsChangedEvent( _impl->_settings_width, _impl->_settings_height, _impl->_settings_format.c_str() );

		_impl->_calibration.clear();
		calibrationChanged( _impl->_calibration.constData(), _impl->_calibration.size() );

		if( number > -1 )
		{
			thread->addCameraReader( _impl, _impl->_number );
		}
	}
}

void Camera::processEvents()
{
	try
	{
		QMutexLocker lk( &_impl->_mutex );
		WorkerThread* thread = WorkerThread::instance( comId() );
		thread->processCameraEvents( _impl->_number );
	}
	catch( const rec::robotino::api2::RobotinoException& )
	{
	}
}

void Camera::setReadWrite()
{
	WorkerThread* thread = WorkerThread::instance( comId() );
	thread->removeCameraReader( _impl );

	if( _impl->_number > -1 )
	{
		thread->addCameraReader( _impl, _impl->_number );
	}
}

void Camera::setWriteOnly()
{
	WorkerThread* thread = WorkerThread::instance( comId() );
	thread->removeCameraReader( _impl );
}

void Camera::setJPGImage( const unsigned char* data, unsigned int dataSize )
{
	QMutexLocker lk( &_impl->_mutex );
	WorkerThread* thread = WorkerThread::instance( comId() );

	thread->set_image( _impl->_number, QByteArray( (const char*)data, (int)dataSize ), 0, 0, 0, "jpg" );
}

void Camera::setFormat( unsigned int width, unsigned int height, const char* format )
{
	QMutexLocker lk( &_impl->_mutex );
	WorkerThread* thread = WorkerThread::instance( comId() );

	thread->set_set_camera_settings( _impl->_number, width, height, format );
}

void Camera::setBrightness( int value )
{
	QMutexLocker lk( &_impl->_mutex );
	WorkerThread* thread = WorkerThread::instance( comId() );

	thread->set_set_camera_control( _impl->_number, "brightness", value );
}

void Camera::setContrast( int value )
{
	QMutexLocker lk( &_impl->_mutex );
	WorkerThread* thread = WorkerThread::instance( comId() );

	thread->set_set_camera_control( _impl->_number, "contrast", value );
}

void Camera::setSaturation( int value )
{
	QMutexLocker lk( &_impl->_mutex );
	WorkerThread* thread = WorkerThread::instance( comId() );

	thread->set_set_camera_control( _impl->_number, "saturation", value );
}

void Camera::setAutoWhiteBalanceEnabled( bool enable )
{
	QMutexLocker lk( &_impl->_mutex );
	WorkerThread* thread = WorkerThread::instance( comId() );

	thread->set_set_camera_control( _impl->_number, "autowhitebalance", ( enable ? 1 : 0 ) );
}

void Camera::setGain( int value )
{
	QMutexLocker lk( &_impl->_mutex );
	WorkerThread* thread = WorkerThread::instance( comId() );

	thread->set_set_camera_control( _impl->_number, "gain", value );
}

void Camera::setWhiteBalanceTemperature( int value )
{
	QMutexLocker lk( &_impl->_mutex );
	WorkerThread* thread = WorkerThread::instance( comId() );

	thread->set_set_camera_control( _impl->_number, "whitebalancetemperature", value );
}

void Camera::setBacklightCompensation( int value )
{
	QMutexLocker lk( &_impl->_mutex );
	WorkerThread* thread = WorkerThread::instance( comId() );

	thread->set_set_camera_control( _impl->_number, "backlightcompensation", value );
}

void Camera::setAutoExposureEnabled( bool enable )
{
	QMutexLocker lk( &_impl->_mutex );
	WorkerThread* thread = WorkerThread::instance( comId() );

	thread->set_set_camera_control( _impl->_number, "autoexposure", ( enable ? 1 : 0 ) );
}

void Camera::setExposure( int value )
{
	QMutexLocker lk( &_impl->_mutex );
	WorkerThread* thread = WorkerThread::instance( comId() );

	thread->set_set_camera_control( _impl->_number, "exposure", value );
}

void Camera::setAutoFocusEnabled( bool enable )
{
	QMutexLocker lk( &_impl->_mutex );
	WorkerThread* thread = WorkerThread::instance( comId() );

	thread->set_set_camera_control( _impl->_number, "autofocus", ( enable ? 1 : 0 ) );
}

void Camera::setFocus( int value )
{
	QMutexLocker lk( &_impl->_mutex );
	WorkerThread* thread = WorkerThread::instance( comId() );

	thread->set_set_camera_control( _impl->_number, "focus", value );
}

void Camera::setSharpness( int value )
{
	QMutexLocker lk( &_impl->_mutex );
	WorkerThread* thread = WorkerThread::instance( comId() );

	thread->set_set_camera_control( _impl->_number, "sharpness", value );
}

CameraCapabilities Camera::capabilities() const
{
	QMutexLocker lk( &_impl->_mutex );
	return _impl->_capabilities;
}

int Camera::calibration( double** buffer, unsigned int bufferSize )
{
	QMutexLocker lk( &_impl->_mutex );
	int toCopy = std::min<int>( bufferSize, _impl->_calibration.size() );

	memcpy( *buffer, _impl->_calibration.constData(), toCopy * sizeof( double ) );
	return toCopy;
}

bool Camera::isNewImageAvailable( unsigned int* dataSize ) const
{
	QMutexLocker lk( &_impl->_mutex );
	*dataSize = _impl->_data.size();
	return _impl->_isNewImageAvailable;
}

bool Camera::getImage( unsigned char** data,
					unsigned int dataSize,
					unsigned int* width,
					unsigned int* height,
					unsigned int* step )
{
	QMutexLocker lk( &_impl->_mutex );
	_impl->_isNewImageAvailable = false;

	memcpy( *data, _impl->_data.constData(), std::min<int>( dataSize, _impl->_data.size() ) );
	*width = _impl->_width;
	*height = _impl->_height;
	*step = _impl->_step;

	return (int)dataSize >= _impl->_data.size();
}

void Camera::setBGREnabled( bool enable )
{
	QMutexLocker lk( &_impl->_mutex );
	_impl->_bgrEnabled = enable;
}

bool Camera::isBGRenabled() const
{
	QMutexLocker lk( &_impl->_mutex );
	return _impl->_bgrEnabled;
}

bool Camera::isLocalConnection() const
{
	QMutexLocker lk( &_impl->_mutex );
	WorkerThread* thread = WorkerThread::instance( comId() );
	return thread->isLocalConnection();
}

void Camera::imageReceivedEvent( const unsigned char* data,
	unsigned int dataSize,
	unsigned int width,
	unsigned int height,
	unsigned int step )
{
}

void Camera::capabilitiesChangedEvent( const CameraCapabilities& capablities )
{
}

void Camera::settingsChangedEvent( unsigned int width, unsigned int height, const char* format )
{
}

void Camera::calibrationChanged( const double* data, unsigned int dataSize )
{
}
