#include "Realsense.h"
#include "rec/robotino/daemons/Log.h"
#include "RobotinoClient.h"
#include <QImage>

#ifdef foreach
  #undef foreach
#endif
#include <librealsense2/rs.hpp>

extern rec::robotino::daemons::Log* logHandler;

Realsense* Realsense::_impl = NULL;

void Realsense::init()
{
	_impl = new Realsense;
}

void Realsense::done()
{
	delete _impl;
}

Realsense::Realsense()
: _grabThread(NULL)
{
}

Realsense::~Realsense()
{
	stop();
}

void Realsense::start()
{
	static int count = 0;
	stop();
	
	_grabThread = new QThread;
	_grabThread->setObjectName(QString("Thread #%1").arg(count++));
	
	RealsenseGrab* worker = new RealsenseGrab;
	worker->moveToThread(_grabThread);
	
	connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
	connect(_grabThread, SIGNAL(started()), worker, SLOT(process()));
	connect(worker, SIGNAL(finished()), _grabThread, SLOT(quit()));
	connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
	connect(_grabThread, SIGNAL(finished()), this, SLOT(grabThread_finished()));
	connect(_grabThread, SIGNAL(finished()), _grabThread, SLOT(deleteLater()));
	_grabThread->start();
	
	logHandler->log( QString("Started %1").arg(_grabThread->objectName()), 0);
}

void Realsense::stop()
{
	if(_grabThread)
	{
		_grabThread->requestInterruption();
	}
}

void Realsense::quit()
{
	if(_grabThread)
	{
		bool ok = connect(_grabThread, SIGNAL(finished()), qApp, SLOT(quit()));
		Q_ASSERT(ok);
		
		_grabThread->requestInterruption();
	}
	else
	{
		qApp->quit();
	}
}

void Realsense::errorString(QString str)
{
	logHandler->log( str, 0 );
	quit();
}

void Realsense::grabThread_finished()
{
	logHandler->log( QString("Stopped %1").arg(sender()->objectName()), 0);
	_grabThread = NULL;
}

RealsenseGrab::RealsenseGrab()
: _run(true)
{
	
}

RealsenseGrab::~RealsenseGrab()
{
	
}

void RealsenseGrab::process()
{
	try
	{
		rs2::config config;
		config.enable_stream(RS2_STREAM_COLOR,640,480,RS2_FORMAT_RGB8,30);
		
		rs2::pipeline pipe;
		pipe.start(config);
		
		int i=0;
		while (false == QThread::currentThread()->isInterruptionRequested())
		{	
			rs2::frameset f = pipe.wait_for_frames(1000);
			
			rs2::video_frame vf = f.get_color_frame();
			
			qDebug() << i;
			
			if (vf.get_width() > 0)
			{
				QImage image((const uchar*)vf.get_data(),vf.get_width(), vf.get_height(),vf.get_stride_in_bytes(),QImage::Format_RGB888);
				
				QByteArray ba;
				QBuffer buffer(&ba);
				image.save( &buffer, "jpg", 90 );
				
				RobotinoClient::singleton()->set_image0(ba,0,0,0,"jpg");
			}
		}
	}
	catch(const rs2::error & e)
	{
		QString str = QString("RealSense error calling %1 (%2)  %3").arg(e.get_failed_function().c_str()).arg(e.get_failed_args().c_str()).arg(e.what());
		emit error(str);
	}
	catch(const std::exception & e)
	{
		emit error(e.what());
	}
	
	emit finished();
}