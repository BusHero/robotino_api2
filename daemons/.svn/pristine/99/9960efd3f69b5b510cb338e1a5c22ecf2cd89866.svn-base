#ifndef _CAMERA_LINUX_H_
#define _CAMERA_LINUX_H_

#include "Camera.h"

#include "usb_cam/usb_cam.h"

class Camera_Linux : public Camera
{
public:
	Camera_Linux();
	~Camera_Linux();

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
	QString _video_device_name;
	QString _io_method_name;
	usb_cam_io_method _io_method;

	QString _pixel_format_name;
	usb_cam_pixel_format _pixel_format;
	int _image_width;
	int _image_height;

	usb_cam_camera_image_t* _camera_image;
};

#endif //_CAMERA_LINUX_H_
