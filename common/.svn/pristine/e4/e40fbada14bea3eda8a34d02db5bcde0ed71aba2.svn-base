//  Copyright (C) 2004-2011, Robotics Equipment Corporation GmbH

#include "servercheck_version.h"
#include "Client.h"
#include <iostream>
#include <QtCore>

void printHelp()
{
	std::cout << "servercheck version " << VersionString << std::endl;
	std::cout << "Usage: servercheck address" << std::endl;
}

int main( int argc, char** argv )
{
	QCoreApplication app( argc, argv );
	app.setApplicationName( "servercheck" );

	if( app.arguments().size() != 2 )
	{
		printHelp();
		return 1;
	}

	Client client;
	client.connectToServer( app.arguments().at( 1 ) );

	app.exec();

	return 0;
}
