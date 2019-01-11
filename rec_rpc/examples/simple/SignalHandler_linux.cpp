#include "SignalHandler_linux.h"

int SignalHandler::sighupFd[2] = {-1, -1};
int SignalHandler::sigtermFd[2] = {-1, -1};
int SignalHandler::sigintFd[2] = {-1, -1};

SignalHandler::SignalHandler()
: snHup( 0 )
, snTerm( 0 )
, snInt( 0 )
{
	struct sigaction hup, term, intact;

	hup.sa_handler = hupSignalHandler;
	sigemptyset(&hup.sa_mask);
	hup.sa_flags = 0;
	hup.sa_flags |= SA_RESTART;

	Q_ASSERT( 0 == sigaction(SIGHUP, &hup, 0) );

	term.sa_handler = termSignalHandler;
	sigemptyset(&term.sa_mask);
	term.sa_flags |= SA_RESTART;

	Q_ASSERT( 0 == sigaction(SIGTERM, &term, 0) );

	intact.sa_handler = intSignalHandler;
	sigemptyset(&intact.sa_mask);
	intact.sa_flags |= SA_RESTART;

	Q_ASSERT( 0 == sigaction(SIGINT, &intact, 0) );

	if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sighupFd))
	{
		qFatal("Couldn't create HUP socketpair");
		Q_ASSERT( false );
	}

	if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigtermFd))
	{
		qFatal("Couldn't create TERM socketpair");
		Q_ASSERT( false );
	}

	if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigintFd))
	{
		qFatal("Couldn't create INT socketpair");
		Q_ASSERT( false );
	}

	snHup = new QSocketNotifier(sighupFd[1], QSocketNotifier::Read, this);
	snTerm = new QSocketNotifier(sigtermFd[1], QSocketNotifier::Read, this);
	snInt = new QSocketNotifier(sigintFd[1], QSocketNotifier::Read, this);

	bool connected = true;
	connected &= (bool)connect( snHup, SIGNAL( activated( int ) ), this, SLOT( handleSigHup() ) );
	connected &= (bool)connect( snTerm, SIGNAL( activated( int ) ), this, SLOT( handleSigTerm() ) );
	connected &= (bool)connect( snInt, SIGNAL( activated( int ) ), this, SLOT( handleSigInt() ) );

	connected &= (bool)connect( this, SIGNAL( sigHup() ), qApp, SLOT( quit() ) );
	connected &= (bool)connect( this, SIGNAL( sigTerm() ), qApp, SLOT( quit() ) );
	connected &= (bool)connect( this, SIGNAL( sigInt() ), qApp, SLOT( quit() ) );
	Q_ASSERT( connected );
}

void SignalHandler::hupSignalHandler(int unused)
{
	char a = 1;
	::write(sighupFd[0], &a, sizeof(a));
}

void SignalHandler::termSignalHandler(int unused)
{
	char a = 1;
	::write(sigtermFd[0], &a, sizeof(a));
}

void SignalHandler::intSignalHandler(int unused)
{
	char a = 1;
	::write(sigintFd[0], &a, sizeof(a));
}

void SignalHandler::handleSigHup()
{
	bool ok = disconnect( snHup, SIGNAL( activated(int) ), this, SLOT(handleSigHup()));
	Q_ASSERT( ok );

	Q_EMIT sigHup();
}

void SignalHandler::handleSigTerm()
{
	bool ok = disconnect(snTerm, SIGNAL(activated(int)), this, SLOT(handleSigTerm()));
	Q_ASSERT( ok );

	Q_EMIT sigTerm();
}

void SignalHandler::handleSigInt()
{
	bool ok = disconnect(snInt, SIGNAL(activated(int)), this, SLOT(handleSigInt()));
	Q_ASSERT( ok );

	Q_EMIT sigInt();
}
