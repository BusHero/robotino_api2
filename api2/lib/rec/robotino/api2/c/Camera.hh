//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#ifndef _REC_ROBOTINO_API2_C_CAMERA_HH_
#define _REC_ROBOTINO_API2_C_CAMERA_HH_

#include "rec/robotino/api2/c/globals.h"

#include "rec/robotino/api2/Camera.h"

#include <QSharedPointer>
#include <QMutex>
#include <algorithm>

class ImageBuffer
{
public:
	ImageBuffer()
		: dataSize( 0xfffff )
		, data ( new unsigned char[ dataSize ] )
		, imageSize( 0 )
		, width( 0 )
		, height( 0 )
	{
	}

	~ImageBuffer()
	{
		delete [] data;
	}

	const unsigned int dataSize;
	unsigned char* data;

	unsigned int imageSize;
	unsigned int width;
	unsigned int height;
};

class MyCamera : public rec::robotino::api2::Camera
{
public:
	MyCamera()
		: _readImageBuffer( 1 )
		, _isNewImageAvailable( false )
	{
	}

	~MyCamera()
	{
	}

	BOOL grab()
	{
		processEvents();

		QMutexLocker lk( &_imageBufferMutex );

		if( false == _isNewImageAvailable )
		{
			return FALSE;
		}

		++_readImageBuffer;
		_readImageBuffer = _readImageBuffer % 2;

		_isNewImageAvailable = false;

		return TRUE;
	}

	BOOL imageSize( unsigned int* width, unsigned int* height )
	{
		QMutexLocker lk( &_imageBufferMutex );

		if( 0 == _buffer[_readImageBuffer].imageSize )
		{
			*width = 0;
			*height = 0;
			return FALSE;
		}

		*width = _buffer[_readImageBuffer].width;
		*height = _buffer[_readImageBuffer].height;
		return TRUE;
	}

	BOOL getImage( unsigned char* imageBuffer, unsigned int imageBufferSize, unsigned int* width, unsigned int* height )
	{
		QMutexLocker lk( &_imageBufferMutex );
		
		if( 0 == _buffer[_readImageBuffer].imageSize )
		{
			*width = 0;
			*height = 0;
			return FALSE;
		}

		if( _buffer[_readImageBuffer].imageSize > imageBufferSize )
		{
			*width = 0;
			*height = 0;
			return FALSE;
		}

		*width = _buffer[_readImageBuffer].width;
		*height = _buffer[_readImageBuffer].height;
			
		memcpy( (void*)imageBuffer, (const void*)_buffer[_readImageBuffer].data, _buffer[_readImageBuffer].imageSize );
			
		return TRUE;
	}

private:
	void imageReceivedEvent( const unsigned char* data,
							 unsigned int dataSize,
							 unsigned int width,
							 unsigned int height,
							 unsigned int step )
	{
		QMutexLocker lk( &_imageBufferMutex );

		_isNewImageAvailable = true;

		unsigned int writeBuffer = (_readImageBuffer+1)%2;

		memcpy( (void*)_buffer[writeBuffer].data, (const void*)data, std::min<unsigned int>( dataSize, _buffer[writeBuffer].dataSize ) );

		_buffer[writeBuffer].imageSize = dataSize;
		_buffer[writeBuffer].width = width;
		_buffer[writeBuffer].height = height;
	}

	ImageBuffer _buffer[2];
	unsigned int _readImageBuffer;
	bool _isNewImageAvailable;
	QMutex _imageBufferMutex;
};

typedef QSharedPointer<MyCamera> CameraPtr;

CameraPtr getCamera( CameraId id );

#endif //_REC_ROBOTINO_API2_C_CAMERA_HH_
