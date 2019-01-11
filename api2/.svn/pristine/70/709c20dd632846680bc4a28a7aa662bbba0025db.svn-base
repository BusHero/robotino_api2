//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#include "rec/robotino/api2/c/Camera.h"
#include "rec/robotino/api2/c/Camera.hh"
#include "rec/robotino/api2/c/Com.hh"

#include <QMap>
#include <QMutex>

typedef QMap<CameraId,CameraPtr> CameraContainer;

CameraContainer cameras;

QMutex _camerasMutex;

CameraId nextCameraId= 0;

CameraId Camera_construct()
{
	QMutexLocker lk( &_camerasMutex );
	
	CameraId currentCameraId = nextCameraId;
	++nextCameraId;

	cameras[ currentCameraId ] = CameraPtr( new MyCamera );
	return currentCameraId;
}

BOOL Camera_destroy( CameraId id )
{
	QMutexLocker lk( &_camerasMutex );
	
	CameraContainer::iterator iter = cameras.find( id );
	if( cameras.end() != iter )
	{
		cameras.erase( iter );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL Camera_setComId( CameraId id, ComId comId )
{
	CameraPtr camera = getCamera( id );
	ComPtr com = getCom( comId );

	if( camera && com )
	{
		try
		{
			camera->setComId( com->id() );
		}
		catch( const rec::robotino::api2::RobotinoException& )
		{
			return FALSE;
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL Camera_setCameraNumber(CameraId id, unsigned int n)
{
	CameraPtr camera = getCamera(id);
	if (camera)
	{
		try
		{
			camera->setCameraNumber(n);
			return TRUE;
		}
		catch (const rec::robotino::api2::RobotinoException&)
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

unsigned int numCameras()
{
	return rec::robotino::api2::Camera::numCameras();
}

BOOL Camera_grab( CameraId id )
{
	CameraPtr camera = getCamera( id );

	if( camera )
	{
		return camera->grab();
	}
	else
	{
		return FALSE;
	}
}

BOOL Camera_imageSize( CameraId id, unsigned int* width, unsigned int* height )
{
	CameraPtr camera = getCamera( id );

	if( camera )
	{
		return camera->imageSize( width, height );
	}
	else
	{
		return FALSE;
	}
}


BOOL Camera_getImage( CameraId id, unsigned char* imageBuffer, unsigned int imageBufferSize, unsigned int* width, unsigned int* height )
{
	CameraPtr camera = getCamera( id );
	if( camera )
	{
		return camera->getImage( imageBuffer, imageBufferSize, width, height );
	}
	else
	{
		return FALSE;
	}
}

BOOL Camera_setFormat(CameraId id, unsigned int width, unsigned int height)
{
	CameraPtr camera = getCamera(id);

	if (camera)
	{
		camera->setFormat(width, height, "mjpg");
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL Camera_setStreaming( CameraId id, BOOL streaming )
{
	return TRUE;
}

CameraPtr getCamera( CameraId id )
{
	QMutexLocker lk( &_camerasMutex );
	
	CameraContainer::const_iterator iter = cameras.find( id );
	if( cameras.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return CameraPtr();
	}
}

