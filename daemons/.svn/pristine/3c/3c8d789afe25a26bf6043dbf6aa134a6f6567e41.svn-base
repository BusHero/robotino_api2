#ifndef _CameraTest_H_
#define _CameraTest_H_

#include "Camera.h"

class CameraTest : public Camera
{
public:
	CameraTest();
	~CameraTest();

	void setVideoDevice( const QString& name );

	// possible values: mmap, read, userptr
	void setIOMethod( const QString& name );
	
	void setResolution( int width, int height );

	void resolution( int* width, int* height );

	// possible values: yuyv, uyvy, mjpeg
	void setFormat( const QString& format );

	void setBrightness( int value );
	void setContrast( int value );
	void setSaturation( int value );
	void setAutoWhiteBalanceEnabled( bool enable );
	void setGain( int value );
	void setWhiteBalanceTemperature( int value );
	void setBacklightCompensation( int value );
	void setAutoExposureEnabled( bool enable );
	void setExposure( int value );
	void setAutoFocusEnabled( bool enable );
	void setFocus( int focus );
	void setSharpness( int value );

	/**
	@throws CameraException
	*/
	void start();

	void stop();

	/**
	@param width Image width. If width=0 and bytearray not empty the bytearray contains an mjpeg image
	@throws CameraException
	*/
	QByteArray grab( unsigned int* width, unsigned int* height, unsigned int* bytes_per_pixel, QString* format );

private:
	int _width;
	int _height;
};

#endif //_CameraTest_H_
