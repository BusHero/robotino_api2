#ifndef _COMOBJECTHELPER_HPP_
#define _COMOBJECTHELPER_HPP_

#define COMOBJECT_CONSTRUCTORS(CLASSNAME) \
CLASSNAME::CLASSNAME() \
: _impl(new CLASSNAME##Impl(this)) \
{ \
	try \
		{ \
		setComId(_comID); \
		} \
	catch (const rec::robotino::api2::RobotinoException&) \
		{ \
		} \
} \
	 \
CLASSNAME::CLASSNAME(const ComId& id) \
	: _impl(new CLASSNAME##Impl(this)) \
{ \
	try \
		{ \
		setComId(id); \
		} \
	catch (const rec::robotino::api2::RobotinoException&) \
		{ \
		} \
}

#define COMOBJECT_DESTRUCTOR(CLASSNAME) \
CLASSNAME::~CLASSNAME() \
{ \
	try \
		{ \
		QMutexLocker lk(&_impl->_mutex); \
		WorkerThread* thread = WorkerThread::instance(comId()); \
		thread->remove##CLASSNAME##Reader(_impl); \
		} \
	catch (const rec::robotino::api2::RobotinoException&) \
		{ \
		} \
	 \
	delete _impl; \
}

#define COMOBJECT_SETCOMID(CLASSNAME) \
void CLASSNAME::setComId( const ComId& id ) \
{ \
	QMutexLocker lk(&_impl->_mutex); \
	 \
	try \
				{ \
		WorkerThread* thread = WorkerThread::instance(comId()); \
		thread->remove##CLASSNAME##Reader(_impl); \
				} \
	catch (const rec::robotino::api2::RobotinoException&) \
				{ \
				} \
		 \
	_comID = id; \
	 \
	if (ComId::null() == comId()) \
			{ \
		return; \
			} \
		 \
				{ \
		WorkerThread* thread = WorkerThread::instance(comId()); \
		thread->add##CLASSNAME##Reader(_impl); \
				} \
}

#endif //_COMOBJECTHELPER_HPP_
