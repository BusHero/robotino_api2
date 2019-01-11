#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <QtCore>

class Camera : public QObject
{
	Q_OBJECT
public:
	Camera()
	{
	}

	virtual ~Camera()
	{
	}

	virtual void setVideoDevice( const QString& name ) = 0;

	// possible values: mmap, read, userptr
	virtual void setIOMethod( const QString& name ) = 0;
	
	virtual void setResolution( int width, int height ) = 0;

	virtual void resolution( int* width, int* height ) = 0;

	// possible values: yuyv, uyvy, mjpeg
	virtual void setFormat( const QString& format ) = 0;

	virtual void setBrightness( int value ) = 0;
	virtual void setContrast( int value ) = 0;
	virtual void setSaturation( int value ) = 0;
	virtual void setAutoWhiteBalanceEnabled( bool enable ) = 0;
	virtual void setGain( int value ) = 0;
	virtual void setWhiteBalanceTemperature( int value ) = 0;
	virtual void setBacklightCompensation( int value ) = 0;
	virtual void setAutoExposureEnabled( bool enable ) = 0;
	virtual void setExposure( int value ) = 0;
	virtual void setAutoFocusEnabled( bool enable ) = 0;
	virtual void setFocus( int focus ) = 0;
	virtual void setSharpness( int value ) = 0;

	/**
	@throws CameraException
	*/
	virtual void start() = 0;

	virtual void stop() = 0;

	/**
	@param width Image width. If width=0 and bytearray not empty the bytearray contains an mjpeg image
	@throws CameraException
	*/
	virtual QByteArray grab( unsigned int* width, unsigned int* height, unsigned int* bytes_per_pixel, QString* format ) = 0;

Q_SIGNALS:
	void log( const QString&, int );
};

class CameraException : public std::exception
{
protected:
	std::string msg;

public:
	CameraException( const std::string &message )
		: msg( message )
	{
	}

	virtual ~CameraException() throw ()
	{
	}

	virtual const std::string& getMessage() const
	{
		return msg;
	}

	//Compatibility functions for std::exception
	virtual const char* what() const throw ()
	{
		return msg.c_str();
	}
};

#endif //_CAMERA_H_
