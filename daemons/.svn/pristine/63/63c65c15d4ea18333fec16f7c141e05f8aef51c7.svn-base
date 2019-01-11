#ifndef _REALSENSE_H_
#define _REALSENSE_H_

#include <QtCore>

class Realsense : public QObject
{
	Q_OBJECT
public:
	static Realsense* singleton() { return _impl; }
	
	static void init();
	static void done();
	
public slots:
	void start();
	void stop();
	void quit();
	
private slots:
	void errorString(QString);
	void grabThread_finished();
	
private:
	static Realsense* _impl;
	
	Realsense();
	~Realsense();
	
	QThread* _grabThread;
};

class RealsenseGrab : public QObject
{
	Q_OBJECT
public:
	RealsenseGrab();
	~RealsenseGrab();
	
public slots:
	void process();
	
signals:
    void finished();
    void error(QString err);
	
private:
	bool _run;
};

#endif //_REALSENSE_H_
