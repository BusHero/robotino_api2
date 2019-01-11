#include "ConfigHandler.h"
#include "ConfigReader.h"
#include "ConfigEditor.h"
#include "ScriptRunner.h"

#define CONFIG_DIR "/etc/robotino"

QByteArray createConfigData()
{	
	QJsonArray arrayObj;
	
	QDir dir( CONFIG_DIR );
	QFileInfoList fileList = dir.entryInfoList(QDir::Files);
	
	int id=1;
	Q_FOREACH( const QFileInfo& fileInfo, fileList )
	//QFileInfo fileInfo("/etc/robotino/controld3.conf");
	{
		ConfigReader configReader;
		
		const QString absoluteFilePath = fileInfo.absoluteFilePath();
		if( configReader.read( absoluteFilePath ) )
		{			
			if(configReader.webItems().isEmpty())
			{
				continue;
			}
			
			QJsonObject fileObj;
			fileObj["name"] = absoluteFilePath;
			
			QJsonArray fileArrayObj;
			
			QMap<QString,QJsonArray> name2group;
			
			Q_FOREACH(const ConfigItem& item, configReader.webItems())
			{
				QJsonObject obj;
					
				obj["id"] = id++;
				obj["file"] = absoluteFilePath;
				obj["group"] = item.group;
				obj["name"] = item.name;
				obj["value"] = item.value.toString();
				
				switch(item.type)
				{
					/*case ConfigItem::ItemType_float:
						break;
						
					case ConfigItem::ItemType_int:
						break;*/
						
					case ConfigItem::ItemType_selection:
						obj["type"] = "select";
						{
							QJsonArray arr;
							Q_FOREACH(QVariant v, item.parameters)
							{
								arr.append(v.toString());
							}
							obj["params"] = arr;
						}
						break;
						
					case ConfigItem::ItemType_bool:
						obj["type"] = "select";
						{
							QJsonArray arr;
							arr.append("true");
							arr.append("false");
							obj["params"] = arr;
						}
						break;
						
					default:
						obj["type"] = "input";
						break;
				}
				
				if(item.group.isEmpty())
				{
					fileArrayObj.append(obj);
				}
				else
				{
					QJsonArray& arr = name2group[item.group];
					arr.append(obj);
				}
			}
			
			Q_FOREACH(const QString& name, name2group.keys())
			{
				const QJsonArray& arr = name2group[name];
				
				QJsonObject groupObj;
				groupObj["name"] = name;
				groupObj["_children"] = arr;
				
				fileArrayObj.append(groupObj);
			}
			
			if(false == fileArrayObj.isEmpty())
			{
				fileObj["_children"] = fileArrayObj;
			}
			
			arrayObj.append(fileObj);
		}
		else
		{
			//TODO
		}
	}
	
	QJsonDocument doc(arrayObj);
	QByteArray data = doc.toJson();
	
	return data;
}

void iterateConfig(const QJsonObject& obj, QMap< QString, QMap< QString, QString > >& file2key2value)
{
	QJsonObject::const_iterator iter = obj.constFind("value");
	if(obj.constEnd()!=iter)
	{
		QString value = iter.value().toString();
		
		QString key;
		QString file;
		
		for(iter = obj.constBegin(); obj.constEnd()!= iter; ++iter)
		{
			if(iter.key()=="group")
			{
				QString str = iter.value().toString();
				if(false == str.isEmpty())
				{
					key.prepend(str);
				}
			}
			else if(iter.key()=="name")
			{
				QString str = iter.value().toString();
				key.append("/"+str);
			}
			else if(iter.key()=="file")
			{
				file = iter.value().toString();
			}
		}
		
		QMap<QString,QString>& key2value = file2key2value[file];
		key2value[key] = value;
	}
	else
	{
		QJsonObject::const_iterator iter = obj.find("_children");
		
		QJsonArray arr = iter.value().toArray();
		
		Q_FOREACH(QJsonValue val, arr)
		{
			iterateConfig(val.toObject(),file2key2value);
		}
	}
}

bool writeConfigData(const QByteArray jsonData, QByteArray& response)
{
//	qDebug() << "writeConfigData";
//	qDebug() << jsonData;
	
	QJsonParseError  error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData,&error);

    if(QJsonParseError::NoError == error.error)
    {
		QMap< QString, QMap< QString, QString > > file2key2value;
		
        QJsonArray arr = jsonDoc.array();
		
		Q_FOREACH(QJsonValue val, arr)
		{
			iterateConfig(val.toObject(),file2key2value);
		}
		
		
		ConfigEditor editor;
		
		QJsonArray names;
		
		for(QMap< QString, QMap< QString, QString > >::const_iterator iter = file2key2value.constBegin(); file2key2value.constEnd() != iter; ++iter)
		{
			QString file = iter.key();
			
			const QMap< QString, QString >& key2value = iter.value();
			
			//qDebug() << file;
			//qDebug() << key2value;
			
			editor.setValues(key2value);
			
			ConfigRestartItem restartItem;
			
			editor.write(file,restartItem);
			
			if(!restartItem.isEmpty())
			{
				names.append( QJsonValue(restartItem.name) );
			}
		}
		
		if(false == names.empty())
		{
			QJsonObject obj;
			obj["daemons"] = names;
			
			QJsonDocument doc(obj);
			QByteArray data = doc.toJson();
			
			response = data;
		}
	}
	else
	{
		return false;
	}
}

bool restartDaemons(const QByteArray jsonData, QByteArray& response)
{
	QJsonParseError  error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData,&error);
	
	qDebug() << jsonData;

    if(QJsonParseError::NoError == error.error)
	{
		QJsonObject root = jsonDoc.object();
		
		QJsonArray daemons = root["daemons"].toArray();
		
		QString daemonslist;
		
		for(QJsonArray::const_iterator iter = daemons.constBegin(); daemons.constEnd() != iter; ++iter)
		{
			qDebug() << "Restart " << (*iter).toString();
			daemonslist += (*iter).toString() + " ";
		}
		
		ScriptRunner::singleton()->runScript( QString("/home/verbeek/srcs/openrobotino/daemons/scripts/Robotino3/xenial/usr/local/bin/reload-config.sh"), QStringList() << daemonslist );
		
		return true;
	}
	else
	{
		return false;
	}
}