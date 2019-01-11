#include "ConfigReader.h"
#include <iostream>

ConfigReader::ConfigReader()
: verbosity( 0 )
{
}

bool ConfigReader::read( const QString& fileName )
{
	_items.clear();
	_webItems.clear();
	_restartItem.clear();

	QFile file( fileName );
	QFileInfo fileInfo(file);
	QString basename = fileInfo.baseName();
	QString filepath = fileInfo.absoluteFilePath();

	if( file.open( QIODevice::ReadOnly ) )
	{
		QTextStream stream( &file );

		bool isWebItemFound = false;
		ConfigItem webItem;

		QString group;
		QString line;
		do
		{
			line = stream.readLine();
			line = line.trimmed();

			if( line.contains( QRegExp( "^(#|;)@restart command begin$" ) ) )
			{
				do
				{
					line = stream.readLine();
					line = line.trimmed();
					if( false == line.startsWith("#") )
					{
						break;
					}

					if( line.contains( QRegExp("^(#|;)@name") ) )
					{
						_restartItem.name = line;
						_restartItem.name.remove( QRegExp("(#|;)@name") );
						_restartItem.name = _restartItem.name.trimmed();
					}
					else if( line.contains( QRegExp("^(#|;)@cmd") ) )
					{
						QString s = line;
						s.remove( QRegExp("(#|;)@cmd") );
						s = s.trimmed();
						_restartItem.script.append( s + '\n' );
					}
				}
				while( false == line.isNull() );
			}

			if( line.length() >= 5 )
			{
				if( line.contains( QRegExp("^(#|;)@web") ) )
				{
					webItem.clear();

					QStringList l = line.split( QRegExp( "\\s" ), QString::SkipEmptyParts );
					isWebItemFound = true;

					if( l.count() > 1 )
					{
						QString type = l.at( 1 );
						if( "ip" == type )
						{
							webItem.type = ConfigItem::ItemType_ip;
						}
						else if( "float" == type )
						{
							webItem.type = ConfigItem::ItemType_float;
							if( 5 == l.count() )
							{
								webItem.parameters.append( l.at(2).toFloat() ); //min
								webItem.parameters.append( l.at(3).toFloat() ); //max
								webItem.parameters.append( l.at(4).toFloat() ); //step
							}
							else
							{
								webItem.parameters.append( 0 ); //min
								webItem.parameters.append( 1 ); //max
								webItem.parameters.append( 0.1 ); //step
							}
						}
						else if( "int" == type )
						{
							webItem.type = ConfigItem::ItemType_int;
						}
						else if( "selection" == type )
						{
							webItem.type = ConfigItem::ItemType_selection;
							for( int i=2; i<l.count(); ++i )
							{
								webItem.parameters.append( l.at(i) );
							}
						}
						else if( "bool" == type )
						{
							webItem.type = ConfigItem::ItemType_bool;
						}
						else if( "fileselection" == type )
						{
							webItem.type = ConfigItem::ItemType_selection;
							QStringList fileList;
							if( l.count() > 2 )
							{
								QString path = l.at(2);
								QStringList filters;
								for( int i=3; i<l.count(); ++i )
								{
									filters << l.at(i);
								}
								QDir dir( path );
								fileList = dir.entryList( filters, QDir::Files );
							}
							for( int i=0; i<fileList.count(); ++i )
							{
								webItem.parameters.append( fileList.at(i) );
							}
						}
						else
						{
							webItem.type = ConfigItem::ItemType_text;
						}
					}
					else
					{
						webItem.type = ConfigItem::ItemType_text;
					}
				}
			}

			if( ( false == line.contains(  QRegExp("^(#|;)") ) ) 
				&& ( false == line.isEmpty() )
				)
			{
				if( line.startsWith("[") && line.endsWith("]") )
				{
					group = line.mid( 1, line.length()-2 );
				}
				else
				{
					int index = line.indexOf( QRegExp( "\\s|=" ) );
					if( index > -1 )
					{
						QString name = line.mid( 0, index );
						QString key = group + "/" + name;
						QString value =  line.mid( index+1 );

						_items[key] = value;

						if( verbosity > 0 )
						{
							std::cout << group.toStdString() << "/" << name.toStdString() << " : " << value.toStdString() << std::endl;
						}

						if( isWebItemFound )
						{
							webItem.name = name;
							webItem.group = group;
							webItem.category = filepath;
							
							switch( webItem.type )
							{
							//case ConfigItem::ItemType_ip:
							//	break;

							case ConfigItem::ItemType_float:
								webItem.value = value.toFloat();
								break;

							case ConfigItem::ItemType_int:
								webItem.value = value.toInt();
								break;

							case ConfigItem::ItemType_bool:
								webItem.value = ( value.toInt() > 0 ? true : false );
								break;

							default:
								webItem.value = value;
								break;
							}

							if( verbosity > 1 )
							{
								std::cout << "webItemFound " << webItem.name.toStdString() << std::endl;
							}
							_webItems << webItem;
						}
					}
				}
				isWebItemFound = false;
			}
		}
		while( false == line.isNull() );

		file.close();
		return true;
	}

	return false;
}

const QMap<QString,QString>& ConfigReader::items() const
{
	return _items;
}

const QList<ConfigItem>& ConfigReader::webItems() const
{
	return _webItems;
}
