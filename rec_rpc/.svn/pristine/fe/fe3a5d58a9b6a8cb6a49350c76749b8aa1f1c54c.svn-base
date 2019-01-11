#include "SignalHandler_win32.h"

SignalHandler* SignalHandler::instance = 0;

SignalHandler::SignalHandler()
{
	Q_ASSERT( NULL == instance );
	instance = this;

	BOOL res = SetConsoleCtrlHandler( (PHANDLER_ROUTINE)ConsoleHandler, TRUE );
	Q_ASSERT( (BOOL)TRUE == res );

	bool connected = true;
	connected &= (bool)connect( this, SIGNAL( sigTerm() ), qApp, SLOT( quit() ) );
	Q_ASSERT( connected );

	start();
}

SignalHandler::~SignalHandler()
{
	exit( 0 );
	wait();

	instance = NULL;
}

BOOL WINAPI SignalHandler::ConsoleHandler(DWORD CEvent)
{
	switch(CEvent)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		{
			instance->exit( (int)CEvent );
			::Sleep( 500 );
			break;
		}
	}
	return TRUE;
}

void SignalHandler::run()
{
	int ret = exec();

	switch( ret )
	{
	case CTRL_C_EVENT:
		Q_EMIT sigTerm();
		break;
	case CTRL_BREAK_EVENT:
		Q_EMIT sigTerm();
		break;
	case CTRL_CLOSE_EVENT:
		Q_EMIT sigTerm();
		break;
	case CTRL_LOGOFF_EVENT:
		Q_EMIT sigTerm();
		break;
	case CTRL_SHUTDOWN_EVENT:
		Q_EMIT sigTerm();
		break;
	}
}
