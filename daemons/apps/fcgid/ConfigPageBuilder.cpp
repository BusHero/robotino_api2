#include "ConfigPageBuilder.h"

#include "SettingsItemsXMLGen.h"
#include "BooleanSettingsItem.h"
#include "FloatSettingsItem.h"
#include "IntegerSettingsItem.h"
#include "IPSettingsItem.h"
#include "StringSettingsItem.h"
#include "SelectionSettingsItem.h"

#include "rec/robotino/daemons/Log.h"

ConfigPageBuilder::ConfigPageBuilder()
{

}

ConfigPageBuilder::~ConfigPageBuilder()
{
}


void ConfigPageBuilder::setINI_DIR( const QStringList& dirs )
{
	_dirs = dirs;
}

ConfigRestartItem ConfigPageBuilder::getConfigRestartItem( const QString& filepath ) const
{
	if( _restartItems.contains(filepath) )
	{
		return _restartItems[filepath];
	}
	else
	{
		return ConfigRestartItem();
	}
}

void ConfigPageBuilder::rebuild()
{
	//rec::robotino::daemons::Log::singleton()->log( QString("ConfigPageBuilder::rebuild() ID = %1").arg(QThread::currentThreadId()), 0 );

	_settingItems.clear();
	_categories.clear();
	_restartItems.clear();

	ConfigReader configReader;
	Q_FOREACH( const QString& dirStr, _dirs )
	{
		QFileInfoList fileList;

		QFileInfo info( dirStr );
		if( info.isFile() )
		{
			fileList << info;
		}
		else if( info.isDir() )
		{
			QDir qdir( dirStr );
		 	fileList = qdir.entryInfoList(QDir::Files);
		}
		else
		{
			int index = dirStr.lastIndexOf( "/" );
			if( index > -1 )
			{
				QString path = dirStr.mid( 0, index );
				QString filter = dirStr.mid( index+1 );

				QDir dir( path );
				fileList = dir.entryInfoList( QStringList() << filter, QDir::Files );
			}
		}

		Q_FOREACH( const QFileInfo& fileInfo, fileList )
		{
			if( configReader.read( fileInfo.absoluteFilePath() ) )
			{
				SettingCategory settingCategory( fileInfo.absoluteFilePath(), configReader.restartItem() );
				_categories.append( settingCategory );

				if( false == configReader.restartItem().isEmpty() )
				{
					_restartItems[ fileInfo.absoluteFilePath() ] = configReader.restartItem();
				}
				//rec::robotino::daemons::Log::singleton()->log(QString("configReader.webItems().length() = %1").arg(configReader.webItems().length()).toLatin1() ,0);

				_settingItems.append( configReader.webItems() );
			}
			else
			{
				//TODO
			}
		}
	}

	QSet<QString> categorySet;

	/************** Generate Item XML **************/
	SettingsItemsXMLGen itemGen;
	rec::robotino::daemons::Log::singleton()->log(QString("settingItems.length() = %1").arg( _settingItems.length()).toLatin1() ,0);

	int idx = 0;
	Q_FOREACH( const ConfigItem& item, _settingItems )
	{
		ConfigItem::ConfigItemType type = item.type;

		categorySet.insert( item.category );

		switch( type )
		{
		case ConfigItem::ItemType_ip:
			{
				IPSettingsItem* settingItem = new IPSettingsItem();
				settingItem->setItemId( idx++ );
				settingItem->setCategory(item.category);
				settingItem->setItemName(item.group + "/" +item.name);
				settingItem->setValue(item.value.toString());
				itemGen.appendItem(settingItem);
			}
			break;

		case ConfigItem::ItemType_float:
			{
				FloatSettingsItem* settingItem = new FloatSettingsItem();
				settingItem->setItemId( idx++ );
				settingItem->setCategory(item.category);
				settingItem->setItemName(item.group + "/" +item.name);
				settingItem->setValue(item.value.toFloat());

				if( 3 == item.parameters.count() )
				{
					settingItem->setMin(item.parameters.at(0).toFloat());
					settingItem->setMax(item.parameters.at(1).toFloat());
					settingItem->setPrecision(item.parameters.at(2).toFloat());
				}
				else
				{
					settingItem->setMin(0);
					settingItem->setMax(1);
					settingItem->setPrecision(0.1);
				}
				itemGen.appendItem(settingItem);
			}
			break;

		case ConfigItem::ItemType_int:
			{
				IntegerSettingsItem* settingItem = new IntegerSettingsItem();
				settingItem->setItemId( idx++ );
				settingItem->setCategory(item.category);
				settingItem->setItemName(item.group + "/" +item.name);
				settingItem->setValue(item.value.toInt());
				itemGen.appendItem(settingItem);
			}
			break;

		case ConfigItem::ItemType_selection:
			{
				SelectionSettingsItem* settingItem = new SelectionSettingsItem();
				settingItem->setItemId( idx++ );
				settingItem->setCategory(item.category);
				settingItem->setItemName(item.group + "/" +item.name);
				settingItem->setValue(item.value.toString());
				QStringList options;
				for( int i= 0; i< item.parameters.count(); ++i )
				{
					options.append(item.parameters[i].toString());
				}
				settingItem->setOptions(options);
				itemGen.appendItem(settingItem);
			}
			break;

		case ConfigItem::ItemType_bool:
			{
				SelectionSettingsItem* settingItem = new SelectionSettingsItem();
				settingItem->setItemId( idx++ );
				settingItem->setCategory(item.category);
				settingItem->setItemName(item.group + "/" +item.name);
				settingItem->setValue(item.value.toString());
				QStringList options = QStringList() << "true" << "false";
				settingItem->setOptions(options);
				itemGen.appendItem(settingItem);
			}
			break;

		default:
			{
				StringSettingsItem * settingItem = new StringSettingsItem();
				settingItem->setItemId( idx++ );
				settingItem->setCategory(item.category);
				settingItem->setItemName(item.group + "/" +item.name);
				settingItem->setValue(item.value.toString());
				itemGen.appendItem(settingItem);
			}
			break;
		}
	}
	_settingItemXML = itemGen.getSettingsItemsXML();


	/************** Generate Category XML **************/
	_categoryXML.clear();
	QXmlStreamWriter xmlWriter(&_categoryXML);
	xmlWriter.setAutoFormatting(true);
	xmlWriter.writeStartElement("List");

	Q_FOREACH( const SettingCategory& sc, _categories )
	{
		if( !categorySet.contains( sc.filepath ) )
		{
			continue;
		}
		xmlWriter.writeStartElement("settingCategory");

		xmlWriter.writeStartElement("categoryName");
		xmlWriter.writeCharacters( sc.filepath );
		xmlWriter.writeEndElement();

		xmlWriter.writeStartElement("needRestart");
		xmlWriter.writeCharacters( sc.restartItem.isEmpty()? "__NOTHING" : sc.restartItem.name );
		xmlWriter.writeEndElement();

		xmlWriter.writeStartElement("parentID");
		xmlWriter.writeCharacters("root");
		xmlWriter.writeEndElement();

		xmlWriter.writeEndElement();
	}

	xmlWriter.writeEndElement();

	itemGen.clear();
}
