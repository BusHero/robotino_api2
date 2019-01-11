#include "ConfigEditor.h"
#include "ConfigReader.h"
#include <iostream>

ConfigEditor::ConfigEditor()
	: verbosity( 0 )
{
}

void ConfigEditor::setValues(const QMap<QString, QString>& values)
{
	_values = values;
}

bool ConfigEditor::write( const QString& fileName )
{
	QFile file( fileName );
	QFileInfo fileInfo(file);
	QString basename = fileInfo.baseName();

	QTemporaryFile tempFile;
	if( file.open( QIODevice::ReadOnly ) && tempFile.open() )
	{
		QTextStream tempStream( &tempFile );
		QTextStream stream( &file );

		QMap<QString,QString> items;

		QString group;
		QString line = stream.readLine();
		while( false == line.isNull() )
		{
			bool lineModified = false;

			QString trimmedlLine = line.trimmed();

			if( ( false == trimmedlLine.startsWith("#") ) 
				&& ( false == trimmedlLine.startsWith(";") )
				&& ( false == trimmedlLine.isEmpty() )
				)
			{
				if( trimmedlLine.startsWith("[") && trimmedlLine.endsWith("]") )
				{
					group = trimmedlLine.mid( 1, line.length()-2 );
				}
				else
				{
					int index = trimmedlLine.indexOf( QRegExp( "\\s|=" ) );
					if( index > -1 )
					{
						QString name = line.mid( 0, index );
						QString key = group + "/" + name;
						QString value =  line.mid( index+1 );

						QString sep = line;
						sep.remove( name );
						sep.remove( value );

						if( _values.contains( key ) )
						{
							tempStream << name << sep << _values[key] << '\n';
							lineModified = true;

							items[key] = _values[key];
						}
						else
						{
							items[key] = value;
						}
					}
				}
			}

			if( false == lineModified )
			{
				tempStream << line;
			}

			line = stream.readLine();
			if( false == lineModified && false == line.isNull() )
			{
				tempStream << '\n';
			}
		}

		tempFile.close();
		file.close();

		ConfigReader reader;
		reader.read( tempFile.fileName() );

		if( verbosity > 1 )
		{
			std::cout << "****read" << std::endl;
			QMap<QString,QString>::const_iterator iter = reader.items().constBegin();
			while( reader.items().constEnd() != iter )
			{
				std::cout << iter.key().toStdString() << " " << iter.value().toStdString() << std::endl;
				++iter;
			}

			std::cout << "****set" << std::endl;
			QMap<QString,QString>::const_iterator itemsIter = items.constBegin();
			while( items.constEnd() != itemsIter )
			{
				std::cout << itemsIter.key().toStdString() << " " << itemsIter.value().toStdString() << std::endl;
				++itemsIter;
			}
		}

		if( reader.items() == items )
		{
			QFile::remove( fileName );
			QFile::copy(tempFile.fileName(), fileName);
			return true;
		}
	}

	return false;
}
