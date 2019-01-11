#include "MyCamera.h"
#include "MyCom.h"

MyCamera* MyCamera::_impl = NULL;

void MyCamera::init()
{
	_impl = new MyCamera;
}

void MyCamera::done()
{
	delete _impl;
	_impl = NULL;
}
	
MyCamera* MyCamera::singleton()
{
	if(_impl->comId() != MyCom::singleton()->id())
	{
		_impl->setComId( MyCom::singleton()->id() );
	}

	_impl->_idleCount = 10;
	
	return _impl;
}
	
MyCamera::MyCamera()
	: _idleTimer(new QTimer)
	, _idleCount(0)
{
	setJPGDecodingEnabled(false);
	
	QObject::connect(_idleTimer, &QTimer::timeout, [this]() {this->checkIdle(); });
	_idleTimer->setSingleShot(false);
	_idleTimer->setInterval(1000);
	_idleTimer->start();
}

MyCamera::~MyCamera()
{
	delete _idleTimer;
}

void MyCamera::checkIdle()
{
	if (_idleCount>0)
	{
		--_idleCount;

		if (0 == _idleCount)
		{
			MyCamera::singleton()->setComId(rec::robotino::api2::ComId::null());
		}
	}
}
