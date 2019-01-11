//  Copyright (C) 2004-2010, Robotics Equipment Corporation GmbH

#include <QCoreApplication>
#include "ConfigReader.h"
#include "ConfigEditor.h"

#include <iostream>

#ifdef WIN32
//#define INIFILE "C:\\temp\\SmartFestoFleetCom.ini"
#define INIFILE "C:\\temp\\controld3.conf"
#else
#define INIFILE "/home/verbeek/svn.openrobotino.org/daemons/trunk/apps/controld3/controld3.conf"
#endif

int main( int argc, char** argv )
{
	QCoreApplication app( argc, argv );
	app.setApplicationName( "test" );

	std::cout << "Try reading " << INIFILE << std::endl;

	ConfigReader configReader;
	if( configReader.read( INIFILE ) )
	//if( configReader.read( "C:\\srcs\\svn.openrobotino.org\\daemons\\apps\\fcgid\\fcgid.conf" ) )
	{
		std::cout << "Read " << INIFILE;
	}
	else
	{
		std::cout << "Error reading " << INIFILE;
	}

	std::cout << "*****" << std::endl;
	std::cout << "*****" << std::endl;

	QList<ConfigItem>::const_iterator webItemIter = configReader.webItems().constBegin();
	while( configReader.webItems().constEnd() != webItemIter )
	{
		const ConfigItem& item = *webItemIter;

		std::cout << item.category.toStdString() << " " << item.name.toStdString() << " " << item.type << std::endl;
		++webItemIter;
	}

	std::cout << "*****" << std::endl;
	std::cout << "*****" << std::endl;

	QMap<QString,QString>::const_iterator iter = configReader.items().constBegin();
	while( configReader.items().constEnd() != iter )
	{
		std::cout << iter.key().toStdString() << " " << iter.value().toStdString() << std::endl;
		++iter;
	}

	std::cout << "*****" << std::endl;
	std::cout << "*****" << std::endl;

	std::cout << configReader.restartItem().name.toStdString() << std::endl;
	std::cout << configReader.restartItem().script.toStdString() << std::endl;

	ConfigEditor editor;
	editor.setValue( "internal_gyroscope/publish", "false" );
	editor.write( INIFILE );

#ifdef WIN32
	char ch;
	std::cin >> ch;
#endif

	return 0;
}
