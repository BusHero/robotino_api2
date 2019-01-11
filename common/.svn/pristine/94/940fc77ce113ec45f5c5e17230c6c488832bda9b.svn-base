//  Copyright (C) 2004-2011, Robotics Equipment Corporation GmbH

#include <QtCore>
#include <iostream>

#include "confmerge_version.h"

#include "ConfigReader.h"
#include "ConfigEditor.h"

void printHelp()
{
	std::cout << "confmerge version: " << VersionString << std::endl;
	std::cout << "confmerge [-v|-vv|-vvv] oldconf newconf" << std::endl;
	std::cout << "oldconf is merged into newconf" << std::endl;
	exit(0);
}

int main( int argc, char** argv )
{
	QCoreApplication app( argc, argv );
	app.setApplicationName("confmerge");

	int verbosity = 0;

	QStringList parameters;
	QStringList arguments = app.arguments();
	arguments.pop_front(); //remove app name

	QStringList::iterator iter = arguments.begin();
	while (arguments.end() != iter)
	{
		if ("-v" == arguments.at(0))
		{
			verbosity = 1;
			iter = arguments.erase(iter);
		}
		else if ("-vv" == arguments.at(0))
		{
			verbosity = 2;
			iter = arguments.erase(iter);
		}
		else if ("-vvv" == arguments.at(0))
		{
			verbosity = 3;
			iter = arguments.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	if (2 != arguments.count())
	{
		printHelp();
	}

	QString olfConfFile = arguments.at(0);
	QString newConfFile = arguments.at(1);

	ConfigReader oldConfReader;
	oldConfReader.verbosity = verbosity;
	if (false == oldConfReader.read(olfConfFile))
	{
		std::cerr << "Error reading " << olfConfFile.toStdString() << std::endl;
		exit(1);
	}

	ConfigEditor editor;
	editor.verbosity = verbosity;
	editor.setValues(oldConfReader.items());
	if (false == editor.write(newConfFile))
	{
		std::cerr << "Error merging " << olfConfFile.toStdString() << " into " << newConfFile.toStdString() << std::endl;
		exit(1);
	}

	return 0;
}
