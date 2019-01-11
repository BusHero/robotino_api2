#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

#include <QtCore>

class WatchDog : public QObject
{
	Q_OBJECT
public:
	WatchDog();
	~WatchDog();

	void setMaxCPULoad( int maxCPULoad );
	void setWatchDogDelay( int watchDogDelay );

Q_SIGNALS:
	void bit();
	void log( const QString&, int );

private Q_SLOTS:
	void on_process_readyRead();
	void on_process_finished( int exitCode, QProcess::ExitStatus exitStatus );
	void on_process_error( QProcess::ProcessError );

private:
	QProcess* _process;
	float _maxCPULoad;
	int _watchDogDelay;
	QTime _watchDogTime;
};



#endif //_WATCHDOG_H_
