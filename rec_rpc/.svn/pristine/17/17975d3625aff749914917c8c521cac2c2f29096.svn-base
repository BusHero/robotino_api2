#ifndef _SIGNALHANDLER_LINUX_H_
#define _SIGNALHANDLER_LINUX_H_

#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

#include <QtCore>

class SignalHandler : public QObject
{
	Q_OBJECT
public:
	SignalHandler();

private:
	// Unix signal handlers.
	static void hupSignalHandler(int unused);
	static void termSignalHandler(int unused);
	static void intSignalHandler(int unused);

	static int sighupFd[2];
	static int sigtermFd[2];
	static int sigintFd[2];

	QSocketNotifier* snHup;
	QSocketNotifier* snTerm;
	QSocketNotifier* snInt;

Q_SIGNALS:
	void sigHup();
	void sigTerm();
	void sigInt();

private Q_SLOTS:
	void handleSigHup();
	void handleSigTerm();
	void handleSigInt();
};

#endif // _SIGNALHANDLER_LINUX_H_
