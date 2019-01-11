#include "rec/robotino/daemons/SignalHandler.h"

#ifdef WIN32
rec::robotino::daemons::SignalHandler* rec::robotino::daemons::SignalHandler::instance = NULL;
#else
int rec::robotino::daemons::SignalHandler::sighupFd[2] = {-1, -1};
int rec::robotino::daemons::SignalHandler::sigtermFd[2] = {-1, -1};
int rec::robotino::daemons::SignalHandler::sigintFd[2] = {-1, -1};

#endif