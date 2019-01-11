#ifndef _SIGNALHANDLER_WIN32_H_
#define _SIGNALHANDLER_WIN32_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <QtCore>

class SignalHandler : public QThread
{
	Q_OBJECT
public:
	SignalHandler();
	~SignalHandler();

private:
	static BOOL WINAPI ConsoleHandler(DWORD CEvent);

	void run();

	static SignalHandler* instance;

Q_SIGNALS:
	void sigTerm();
};

#endif // _SIGNALHANDLER_WIN32_H_
