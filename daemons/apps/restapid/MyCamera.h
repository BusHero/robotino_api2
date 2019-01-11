#ifndef _MYCAMERA_H_
#define _MYCAMERA_H_

#include <rec/robotino/api2/Camera.h>
#include <QtCore>

class MyCamera : public rec::robotino::api2::Camera
{
public:
	static void init();
	static void done();
	
	static MyCamera* singleton();
	
private:
	MyCamera();
	~MyCamera();

	void checkIdle();
	
	static MyCamera* _impl;

	QTimer* _idleTimer;
	int _idleCount;
};

#endif //_MYCAMERA_H_
