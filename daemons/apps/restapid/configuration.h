#ifndef _CONFIGURATION_
#define _CONFIGURATION_

#include <QString>

class ConfigReader;

void configInit(const QString& inifile);

QString configValue(const QString& key);

void configSetValue(const QString& key, const QString& value);

ConfigReader& currentConfig();

#endif //_CONFIGURATION_