//  Copyright (C) 2004-2011, Robotics Equipment Corporation GmbH

#include <QtCore>
#include <iostream>

#include "yamlreader_version.h"

#include "rec/yaml/yaml.h"

void printHelp()
{
	std::cout << "yamlreader version: " << VersionString << std::endl;
	std::cout << "yamlreader yamlFile" << std::endl;
	exit(0);
}

int main( int argc, char** argv )
{
	QCoreApplication app( argc, argv );
	app.setApplicationName("yamlreader");

	int verbosity = 0;

	QStringList parameters;
	QStringList arguments = app.arguments();
	arguments.pop_front(); //remove app name

	if (1 != arguments.count())
	{
		printHelp();
	}

	QString yamlFile = arguments.at(0);

	rec::yaml::Reader reader;
	if (reader.parse(yamlFile))
	{
		Q_FOREACH(const QString& key, reader.keys())
		{
			std::cout << key.toLatin1().constData() << " : ";

			rec::yaml::entryType t = reader.entryTypeForKey(key);
			switch (t)
			{
			case rec::yaml::StringEntry:
				std::cout << reader.entryForKey(key).toLatin1().constData() << "  - as real " << reader.realEntryForKey(key);
				break;

			case rec::yaml::ListEntry:
			{
				QStringList l = reader.listEntryForKey(key);
				std::cout << "[ ";
				Q_FOREACH(const QString& str, l)
				{
					std::cout << str.toLatin1().constData() << " ";
				}
				std::cout << "]" << " - as real ";

				QVector<qreal> vec = reader.realVectorEntryForKey(key);
				std::cout << "[ ";
				Q_FOREACH(qreal r, vec)
				{
					std::cout << r << " ";
				}
				std::cout << "]";
			}
				break;

			default:
				std::cout << "Unkown entry type";
				break;
			}

			std::cout << std::endl;
		}
	}
	else
	{
		std::cerr << "Error parsing " << yamlFile.toLatin1().constData();
	}

	return 0;
}
