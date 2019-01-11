#include "ConfigEditor.h"
#include "ConfigReader.h"

ConfigEditor::ConfigEditor()
{
}

void ConfigEditor::setValue( const QString& key, const QString& value )
{
	_values[key] = value;
}

bool ConfigEditor::write( const QString& fileName )
{
	QFile file( fileName );
	QFileInfo fileInfo(file);
	QString basename = fileInfo.baseName();

	QString tempFileName = basename;
	tempFileName.replace( "/|\\\\|\\s", "_" );

#ifdef WIN32
	tempFileName.prepend( "c:\\temp\\" );
#else
	tempFileName.prepend( "/tmp/" );
#endif
	QFile::remove( tempFileName );

	QFile tempFile( tempFileName );
	if( file.open( QIODevice::ReadOnly ) && tempFile.open( QIODevice::WriteOnly ) )
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
		reader.read( tempFileName );

		qDebug() << "****read";
		QMap<QString,QString>::const_iterator iter = reader.items().constBegin();
		while( reader.items().constEnd() != iter )
		{
			qDebug() << iter.key() << " " << iter.value();
			++iter;
		}

		qDebug() << "****set";
		QMap<QString,QString>::const_iterator itemsIter = items.constBegin();
		while( items.constEnd() != itemsIter )
		{
			qDebug() << itemsIter.key() << " " << itemsIter.value();
			++itemsIter;
		}

		if( reader.items() == items )
		{
			QFile::remove( fileName );
			QFile::copy( tempFileName, fileName );
			return true;
		}
	}

	return false;
}
