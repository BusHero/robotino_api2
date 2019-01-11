#ifndef _MYCOM_H_
#define _MYCOM_H_

#include <rec/robotino/api2/Com.h>

class MyCom : public rec::robotino::api2::Com
{
public:
	static void init();
	static void done();
	
	static MyCom* singleton();
	
private:
	MyCom();
	~MyCom();
	
	static MyCom* _impl;
	
	void connectedEvent();
	void connectionClosedEvent();
};

#endif //_MYCOM_H_
