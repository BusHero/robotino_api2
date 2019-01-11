#ifndef _HANDLER_H_
#define _HANDLER_H_

#include <QtCore>

class Handler : public QThread
{
	Q_OBJECT
public:
	enum
	{
		StartEventId = QEvent::User,
		LogEventId
	};

	Handler( unsigned int device, unsigned int channel );
	~Handler();

	void init();

Q_SIGNALS:
	void log( const QString& message, int verbosity );

private Q_SLOTS:
	void on_error();
	void on_watchDog_timeout();

private:
	void customEvent( QEvent* e );
	void run();

	unsigned int _device;
	unsigned int _channel;

	QTimer* _watchDog;
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
