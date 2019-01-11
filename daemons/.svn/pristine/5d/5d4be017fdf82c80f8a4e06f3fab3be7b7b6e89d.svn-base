#include "UdpLogger.h"
#include "rec/robotino/daemons/Log.h"
UdpLogger* GLOBAL_UDPLOGGER_INSTANCE = NULL;

UdpLogger* UdpLogger::Singleton()
{
	if(GLOBAL_UDPLOGGER_INSTANCE == NULL)
	{
		GLOBAL_UDPLOGGER_INSTANCE = new UdpLogger();
	}
	return GLOBAL_UDPLOGGER_INSTANCE;
}

UdpLogger::UdpLogger(QObject *parent)
	: _socket( new QUdpSocket( this ) )
{
	broadcastLogViaUDP = false;
	ip = "127.0.0.1";
	port = 7086;
	verbocityLevel = 2;
}

void UdpLogger::broadcastLog(const QString& msg, int ver)
{
	sendMsg(msg, ver);
}

void UdpLogger::sendMsg(const QString& msg,int verbosity)
{
	QMutexLocker lk( &_mutex );

	//rec::robotino::daemons::Log::singleton()->log("UdpLogger::sendMsg" + msg, 0);
	if(broadcastLogViaUDP && verbosity <= verbocityLevel)
	{
		_socket->writeDatagram((msg).toLatin1(), QHostAddress(ip), port);
	}
}
