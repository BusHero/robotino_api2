#ifndef _HANDLER_H_
#define _HANDLER_H_

#include <QtCore>
#include <rec/LaserScanMessage.h>

class MyScanner;

class Handler : public QObject
{
	Q_OBJECT
public:
	enum
	{
		StartEventId = QEvent::User,
		LogEventId
	};

	Handler(const QString& port, unsigned int channel, MyScanner* scanner);
	~Handler();

	void init();

private Q_SLOTS:
	void on_laser_status_timer_timeout();
	void on_scan( const rec::LaserScanMessage& scan );

private:
	void customEvent( QEvent* e );

	MyScanner* _scanner;

	QString _port;
	unsigned int _channel;
	float _angle_min_deg;
	float _angle_max_deg;

	QTimer* _laser_status_timer;
};

class StartEvent : public QEvent
{
public:
	StartEvent()
		: QEvent( (QEvent::Type)Handler::StartEventId )
	{
	}
};

#endif //_HANDLER_H_
