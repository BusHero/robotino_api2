#include "ProgramHandler.h"
#include "ConfigReader.h"
#include "configuration.h"
#include "ScriptRunner.h"

#define PROGRAM_DIR "/home/robotino/programs"

QByteArray createProgramData()
{
	QJsonArray arrayObj;
	
	QDir dir( currentConfig().value("Robview/programpath",PROGRAM_DIR) );
	
	QFileInfoList fileList = dir.entryInfoList(QDir::Files);
	
	int id=1;
	Q_FOREACH( const QFileInfo& fileInfo, fileList )
	{
		const QString absoluteFilePath = fileInfo.absoluteFilePath();
		
		QJsonObject obj;
		obj["id"] = id++;
		obj["name"] = fileInfo.baseName();
		obj["old"] = fileInfo.baseName();
		obj["suffix"] = fileInfo.completeSuffix();
		
		if( ScriptRunner::singleton()->isRobotinoViewRunning(absoluteFilePath) )
		{
			obj["action"] = "running";
		}
		else
		{
			obj["action"] = "stopped";
		}
		
		arrayObj.append(obj);
	}
	
	QJsonDocument doc(arrayObj);
	QByteArray data = doc.toJson();
	
	return data;
}

QByteArray createGlobalVarsData()
{
	const QMap<QString,QString> vars = ScriptRunner::singleton()->globalVars();
	
	QJsonArray arrayObj;
	
	int id=1;
	for(QMap<QString,QString>::const_iterator iter = vars.constBegin(); vars.constEnd() != iter; ++iter)
	{
		QJsonObject obj;
		obj["id"] = id++;
		obj["name"] = iter.key();
		obj["value"] = iter.value();
		
		arrayObj.append(obj);
	}
	
	QJsonDocument doc(arrayObj);
	QByteArray data = doc.toJson();
	
	return data;
}

QByteArray createRobviewOutData()
{
	QJsonArray arrayObj;
	
	const QStringList l = ScriptRunner::singleton()->robviewOutput();
	
	int id=1;
	Q_FOREACH(const QString& s, l)
	{
		QJsonObject obj;
		obj["id"] = id++;
		obj["value"] = s;
		
		arrayObj.append(obj);
	}
	
	QJsonDocument doc(arrayObj);
	QByteArray data = doc.toJson();
	
	return data;
}

bool renameProgram(const QByteArray jsonData, QByteArray& response)
{
	//qDebug() << jsonData;
	
	QJsonParseError  error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData,&error);

    if(QJsonParseError::NoError == error.error)
	{
		QJsonArray root = jsonDoc.array();
		
		for(QJsonArray::const_iterator iter=root.constBegin();root.constEnd()!=iter;++iter)
		{
			QJsonObject obj = iter->toObject();
			
			QString newName = obj["name"].toString();
			newName = newName.trimmed();
			
			QString oldName = obj["old"].toString();
			QString programSuffix = obj["suffix"].toString();
			
			if(newName != oldName && false == newName.isEmpty())
			{
				QString programDir = currentConfig().value("Robview/programpath",PROGRAM_DIR);
				
				QString fullNewPath = programDir + "/" + newName + "." + programSuffix;
				QString fullOldPath = programDir + "/" + oldName + "." + programSuffix;
				
				QFile::rename(fullOldPath,fullNewPath);
			}
			
		}
		
		return true;
	}
	else
	{
		return false;
	}
}

bool programClicked(const QByteArray jsonData, QByteArray& response)
{
	QJsonParseError  error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData,&error);

    if(QJsonParseError::NoError == error.error)
	{
		QJsonObject root = jsonDoc.object();
		
		QString programName = root["name"].toString();
		QString programSuffix = root["suffix"].toString();
		QString fullProgramPath = currentConfig().value("Robview/programpath",PROGRAM_DIR) + "/" + programName + "." + programSuffix;
		
		if( ScriptRunner::singleton()->isRobotinoViewRunning(fullProgramPath) )
		{
			ScriptRunner::singleton()->stopRobotinoView();
		}
		else
		{
			ScriptRunner::singleton()->runRobotinoView(fullProgramPath);
		}
		
		return true;
	}
	else
	{
		return false;
	}
}

bool removeClicked(const QByteArray jsonData, QByteArray& response)
{
	QJsonParseError  error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData,&error);

    if(QJsonParseError::NoError == error.error)
	{
		QJsonObject root = jsonDoc.object();
		
		QString programName = root["name"].toString();
		QString programSuffix = root["suffix"].toString();
		QString fullProgramPath = currentConfig().value("Robview/programpath",PROGRAM_DIR) + "/" + programName + "." + programSuffix;
		
		return QFile::remove(fullProgramPath);
	}
	else
	{
		return false;
	}
}
