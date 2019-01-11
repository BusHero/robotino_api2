//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#include "rec/robotino/api2/c/Kinect.h"
#include "rec/robotino/api2/c/Kinect.hh"
#include "rec/robotino/api2/c/Com.hh"

#include <QMap>
#include <QMutex>

typedef QMap<KinectId,KinectPtr> KinectContainer;

KinectContainer kinects;

QMutex _kinectsMutex;

KinectId nextKinectId= 0;

KinectId Kinect_construct()
{
	QMutexLocker lk( &_kinectsMutex );
	
	KinectId currentKinectId = nextKinectId;
	++nextKinectId;

	kinects[ currentKinectId ] = KinectPtr( new MyKinect );
	return currentKinectId;
}

BOOL Kinect_destroy( KinectId id )
{
	QMutexLocker lk( &_kinectsMutex );
	
	KinectContainer::iterator iter = kinects.find( id );
	if( kinects.end() != iter )
	{
		kinects.erase( iter );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL Kinect_setComId( KinectId id, ComId comId )
{
	KinectPtr kinect = getKinect( id );
	ComPtr com = getCom( comId );

	if( kinect && com )
	{
		try
		{
			kinect->setComId( com->id() );
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

BOOL Kinect_setKinectNumber( KinectId id, unsigned int number )
{
	KinectPtr kinect = getKinect( id );

	if( kinect )
	{
		try
		{
			kinect->setKinectNumber( number );
			return TRUE;
		}
		catch( rec::robotino::api2::RobotinoException& /*e*/ )
		{
		}
	}

	return FALSE;
}

BOOL Kinect_grab( KinectId id )
{
	KinectPtr kinect = getKinect( id );

	if( kinect )
	{
		return kinect->grab();
	}
	else
	{
		return FALSE;
	}
}

BOOL Kinect_dataSize( KinectId id, unsigned int* depthDataWidth, unsigned int* depthDataHeight, unsigned int* objectDataWidth, unsigned int* objectDataHeight )
{
	KinectPtr kinect = getKinect( id );

	if( kinect )
	{
		return kinect->getSize( depthDataWidth, depthDataHeight, objectDataWidth, objectDataHeight );
	}
	else
	{
		return FALSE;
	}
}

BOOL Kinect_getReadings(
	KinectId id,
	float* depth_data, float* object_data
	, unsigned int* depthDataWidth, unsigned int* depthDataHeight
	, unsigned int* objectDataWidth, unsigned int* objectDataHeight
	, unsigned int* format, unsigned int* stamp
	)
{
	KinectPtr kinect = getKinect( id );

	if( kinect )
	{
		return kinect->getDepth( depth_data, object_data, depthDataWidth, depthDataHeight, objectDataWidth, objectDataHeight, format, stamp );
	}
	else
	{
		return FALSE;
	}
}

KinectPtr getKinect( KinectId id )
{
	QMutexLocker lk( &_kinectsMutex );
	
	KinectContainer::const_iterator iter = kinects.find( id );
	if( kinects.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return KinectPtr();
	}
}

