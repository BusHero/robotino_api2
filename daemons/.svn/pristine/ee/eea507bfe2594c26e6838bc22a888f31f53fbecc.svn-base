#include "DataHandler.h"
#include "ConfigHandler.h"
#include "ScriptRunner.h"
#include "ProgramHandler.h"

DataHandler::DataHandler()
{
	ScriptRunner::init();
}

DataHandler::~DataHandler()
{
	ScriptRunner::done();
}

QByteArray DataHandler::data(const QString& path)
{
	if("/data/config" == path)
	{
		return createConfigData();
	}
	else if("/data/program" == path)
	{
		return createProgramData();
	}
	else if("/data/globalvars" == path)
	{
		return createGlobalVarsData();
	}
	else if("/data/robviewout" == path)
	{
		return createRobviewOutData();
	}
	
	return "";
}

bool DataHandler::setData(const QString& path, const QByteArray& data, QByteArray& response)
{
	if("/data/config" == path)
	{
		return writeConfigData(data,response);
	}
	else if("/data/restartdaemons" == path)
	{
		return restartDaemons(data,response);
	}
	else if("/data/renameprogram" == path)
	{
		return renameProgram(data,response);
	}
	else if("/data/startStopProgram" == path)
	{
		return programClicked(data,response);
	}
	else if("/data/remove" == path)
	{
		return removeClicked(data,response);
	}
	else
	{
		return false;
	}
}