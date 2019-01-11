#include "ConfigReader.h"

ConfigReader _currentConfig;

void configInit(const QString& inifile)
{
	_currentConfig.read(inifile);
	
	qDebug() << inifile;
	qDebug() << _currentConfig.items();
}

QString configValue(const QString& key)
{
	QString value = _currentConfig.value(key);
	return value;
}

void configSetValue(const QString& key, const QString& value)
{
	_currentConfig.setValue(key, value);
}

ConfigReader& currentConfig()
{
	return _currentConfig;
}