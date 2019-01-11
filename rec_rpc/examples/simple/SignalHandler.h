#ifndef _SIGNALHANDLER_H_
#define _SIGNALHANDLER_H_

#ifdef WIN32
#include "SignalHandler_win32.h"
#else
#include "SignalHandler_linux.h"
#endif

#endif // _SIGNALHANDLER_H_
