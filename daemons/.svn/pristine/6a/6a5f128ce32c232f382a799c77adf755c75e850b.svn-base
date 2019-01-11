//  Copyright (C) 2004-2010, Robotics Equipment Corporation GmbH

#include <QApplication>
#include "fcgid_version.h"
#include "rec/robotino/daemons/SignalHandler.h"
#include "rec/robotino/daemons/Log.h"
#include "Client.h"
#include "Fcgi.h"
#include "ScriptRunner.h"
#include "utils.h"
#include <iostream>
#include "fcgi_stdio.h"
#include "UdpLogger.h"

#ifdef WIN32
#define LOGFILE "fcgid_log.txt"
#else
#define LOGFILE "/tmp/fcgid.log"
#endif

int main()
{
	int argc = 0;
	char** argv = NULL;
	bool isDaemon = false;
	QString address;

#ifndef WIN32
	if( isDaemon )
	{
		pid_t pidId = fork();
		if (pidId<0) exit(1); /* fork error */
		if (pidId>0) exit(0); /* parent exits */
		/* child (daemon) continues */
	}
#endif

	QCoreApplication app( argc, argv, false );
	app.setApplicationName( "fcgid" );

	rec::robotino::daemons::SignalHandler sigHandler;
	sigHandler.setDefaultConnectionsEnabled( true );

	rec::robotino::daemons::Log log( LOGFILE, 0, isDaemon );

	ScriptRunner::init();
	Client::init();

	bool connected = true;

	connected &= (bool)QObject::connect( &sigHandler, SIGNAL( log( const QString&, int ) ), &log, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( Client::singleton(), SIGNAL( logLevelChanged( int ) ), &log, SLOT( setVerbosity( int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( &log, SIGNAL( logChanged( const QString&, int ) ), Client::singleton(), SLOT( set_log( const QString&, int ) ) );
	Q_ASSERT( connected );

	Client::singleton()->readParameters( "/etc/robotino/fcgid.conf", "/home/robotino/.config/fcgid.conf" );
	int verbosity = Client::singleton()->getParameter( "~/Log/verbosity", 0 ).toInt();
	log.setVerbosity( verbosity );
	log.log(QString("Log level = %1").arg(verbosity),1);

	UdpLogger::Singleton()->verbocityLevel = verbosity;

	int broadCastViaUdp = Client::singleton()->getParameter( "~/Debug/broadcastLogViaUDP", 0 ).toInt();
	log.log(QString("broadCastViaUdp = %1").arg(broadCastViaUdp),1);
	if(broadCastViaUdp == 1)
	{
		UdpLogger::Singleton()->broadcastLogViaUDP = true;
		UdpLogger::Singleton()->ip = Client::singleton()->getParameter( "~/Debug/udpIP", "127.0.0.1" ).toString();
		UdpLogger::Singleton()->port = Client::singleton()->getParameter( "~/Debug/udpPort", 7086 ).toInt();

		log.log(QString("ip = %1, port = %2").arg(UdpLogger::Singleton()->ip).arg(UdpLogger::Singleton()->port),1);
	}
	else
	{
		UdpLogger::Singleton()->broadcastLogViaUDP = false;
	}

	log.log("Start fcgid",1);
	Fcgi fcgi;//Fcgi runs as a thread
	fcgi.start();

	address = Client::singleton()->getParameter( "~/Network/server", "127.0.0.1:12080" ).toString();
	log.log( QString("address = ") + address, 1);
	Client::singleton()->setAddress( address );
	Client::singleton()->connectToServer( 5000 );

	app.exec();

	Client::done();

	fcgi.stop();

	ScriptRunner::done();

	qDebug() << "Quit";

	return 0;
}
