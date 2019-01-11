//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#ifndef _REC_ROBOTINO_API2_C_Kinect_HH_
#define _REC_ROBOTINO_API2_C_Kinect_HH_

#include "rec/robotino/api2/c/globals.h"

#include "rec/robotino/api2/Kinect.h"

#include <QVector>
#include <QSharedPointer>
#include <QMutex>

#include <string.h>

class MyKinect : public rec::robotino::api2::Kinect
{
public:
	MyKinect()
		: _width( 0 )
		, _widthBuffer( 0 )
		, _height( 0 )
		, _heightBuffer( 0 )
		, _format( 0 )
		, _formatBuffer( 0 )
		, _stamp( 0 )
		, _stampBuffer( 0 )
		, _depthData( NULL )
		, _objectData( NULL )
		, _depthDataBuffer( NULL )
		, _objectDataBuffer( NULL )
	{
	}

	~MyKinect()
	{
		delete _depthData;
		delete _objectData;
		delete _depthDataBuffer;
		delete _objectDataBuffer;
	}

	BOOL grab()
	{
		processEvents();

		QMutexLocker lk( &_mutex );

		if( _stamp != _stampBuffer )
		{
			if( _width != _widthBuffer || _height != _heightBuffer )
			{
				_widthBuffer = _width;
				_heightBuffer = _height;

				delete _depthDataBuffer;
				_depthDataBuffer = new unsigned short[_width*_height];

				delete _objectDataBuffer;
				_objectDataBuffer = new unsigned short[_width*_height];
			}

			memcpy( _depthDataBuffer, _depthData, _width*_height*sizeof(unsigned short) );
			memcpy( _objectDataBuffer, _objectData, _width*_height*sizeof(unsigned short) );

			_formatBuffer = _format;
			_stampBuffer = _stamp;

			return TRUE;
		}

		return FALSE;
	}

	BOOL getSize( unsigned int* depthDataWidth, unsigned int* depthDataHeight, unsigned int* objectDataWidth, unsigned int* objectDataHeight )
	{
		*depthDataWidth = _widthBuffer;
		*depthDataHeight = _heightBuffer;

		*objectDataWidth = _widthBuffer;
		*objectDataHeight = _heightBuffer;

		return TRUE;
	}

	BOOL getDepth(
		float* depth_data
		, float* object_data
		, unsigned int* depthDataWidth, unsigned int* depthDataHeight
		, unsigned int* objectDataWidth, unsigned int* objectDataHeight
		, unsigned int* format, unsigned int* stamp
		)
	{
		getSize( depthDataWidth, depthDataHeight, objectDataWidth, objectDataHeight );

		*format = _formatBuffer;
		*stamp = _stampBuffer;

		for( int y=0; y<(int)_heightBuffer; ++y )
		{
			for( int x=0; x<(int)_widthBuffer; ++x )
			{
				int index = x+y*_widthBuffer;
				*(depth_data++) = _depthDataBuffer[index];
			}
		}
		
		
		for( int y=0; y<(int)_heightBuffer; ++y )
		{
			for( int x=0; x<(int)_widthBuffer; ++x )
			{
				int index = x+y*_widthBuffer;
				*(object_data++) = _objectDataBuffer[index];
			}
		}
			
		return TRUE;
	}

private:
	void depthEvent( const unsigned short* data, unsigned int dataSize, const unsigned short* object_data, unsigned int object_dataSize, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
	{
		QMutexLocker lk( &_mutex );

		if( dataSize != width*height )
		{
			return;
		}

		if( width != _width || height != _height )
		{
			delete _depthData;
			_width = width;
			_height = height;
			_depthData = new unsigned short[width*height];

			delete _objectData;
			_objectData = new unsigned short[width*height];
		}

		memcpy( _depthData, data, dataSize*sizeof(unsigned short) );

		if( object_dataSize == dataSize )
		{
			memcpy( _objectData, object_data, object_dataSize*sizeof(unsigned short) );
		}
		else
		{
			memset( _objectData, 0,  object_dataSize*sizeof(unsigned short) );
		}

		_format = format;
		_stamp = stamp;
	}

	QMutex _mutex;

	unsigned short* _depthData;
	unsigned short* _objectData;
	unsigned int _width;
	unsigned int _height;
	unsigned int _format;
	unsigned int _stamp;

	unsigned short* _depthDataBuffer;
	unsigned short* _objectDataBuffer;
	unsigned int _widthBuffer;
	unsigned int _heightBuffer;
	unsigned int _formatBuffer;
	unsigned int _stampBuffer;
};

typedef QSharedPointer<MyKinect> KinectPtr;

KinectPtr getKinect( KinectId id );

#endif //_REC_ROBOTINO_API2_C_Kinect_HH_
