#include "Config.h"

Config* Config::_impl = NULL;

Config::Config()
{
	_configFilePath = QDir::currentPath() + "/iotest.ini";
	if( QFile::exists( _configFilePath ) )
	{
		_settings = new QSettings( _configFilePath, QSettings::IniFormat );
	}
	else
	{
		_configFilePath = qApp->applicationDirPath() + "/iotest.ini";
		if( QFile::exists( _configFilePath ) )
		{
			_settings = new QSettings( _configFilePath, QSettings::IniFormat );
		}
		else
		{
			_configFilePath = INIFILE;
			if( QFile::exists( _configFilePath ) )
			{
				_settings = new QSettings( _configFilePath, QSettings::IniFormat );
			}
			else
			{
				_settings = new QSettings;
				_configFilePath.clear();
			}
		}
	}

	qDebug() << _configFilePath;

	qDebug() << _settings->allKeys();
	Q_FOREACH( const QString& key, _settings->allKeys() )
	{
		qDebug() << key << " : " << _settings->value( key ).toString();
	}
}

Config::~Config()
{
	delete _settings;
}

void Config::init()
{
	Q_ASSERT( NULL == _impl );
	_impl = new Config;
}

void Config::done()
{
	delete _impl;
	_impl = NULL;
}

QString Config::configFilePath() const
{
	return _configFilePath;
}

QString Config::pathToUV4() const
{
	return _settings->value( "Firmware/pathToUV4" ).toString();
}

QString Config::pathToFirmware() const
{
	QString firmwarePath;
	QString configStr = _settings->value( "Firmware/pathToFirmware" ).toString();
	if( configStr.startsWith(".") )
	{
		firmwarePath = qApp->applicationDirPath() + "/" + configStr;
	}
	else
	{
		firmwarePath = configStr;
	}
	return firmwarePath;
}

QString Config::firmwareProject() const
{
	return _settings->value( "Firmware/firmwareProject" ).toString();
}

QString Config::firmwareTarget() const
{
	return _settings->value( "Firmware/firmwareTarget" ).toString();
}

QString Config::debugComPort() const
{
	return _settings->value( "Firmware/debugComPort" ).toString();
}

QString Config::firmwareVersionFile() const
{
	return _settings->value( "Firmware/firmwareVersionFile" ).toString();
}

QString Config::pathToFTProg() const
{
	return _settings->value("FT232/FTProg","c:/Program Files (x86)/FTDI/FT_Prog/FT_Prog-CmdLine.exe").toString();
}

QString Config::ft232TemplateFile() const
{
	QString filePath;
	QString configStr = _settings->value("FT232/template", qApp->applicationDirPath() + "/ftdi232.xml").toString();
	if (configStr.startsWith("."))
	{
		filePath = qApp->applicationDirPath() + "/" + configStr;
	}
	else
	{
		filePath = configStr;
	}
	return filePath;
}

int Config::firmwareVersionReadTimeout() const
{
	return _settings->value( "Firmware/firmwareVersionReadTimeout" ).toInt();
}

int Config::digitalTimerInterval() const
{
	return _settings->value( "Digital/timerInterval" ).toInt();
}

QVector<int> Config::digitalTestPattern() const
{
	QString str =_settings->value( "Digital/testPattern" ).toString();
	QStringList l = str.split(",");
	QVector<int> v;
	Q_FOREACH( const QString& s, l )
	{
		bool ok;
		int i = s.toInt( &ok, 16 );
		if( ok )
		{
			v.append( i );
		}
	}
	return v;
}

QVector<float> Config::analogTestPattern() const
{
	QString str =_settings->value( "Analog/testPattern" ).toString();
	QStringList l = str.split(",");
	QVector<float> v;
	Q_FOREACH( const QString& s, l )
	{
		bool ok;
		float f = s.toFloat( &ok );
		if( ok )
		{
			v.append( f );
		}
	}
	return v;
}

float Config::analogDelta() const
{
	return _settings->value( "Analog/delta" ).toFloat();
}

int Config::relayTimerInterval() const
{
	return _settings->value( "Relay/timerInterval" ).toInt();
}

QVector<int> Config::relayTestPattern() const
{
	QString str =_settings->value( "Relay/testPattern" ).toString();
	QStringList l = str.split(",");
	QVector<int> v;
	Q_FOREACH( const QString& s, l )
	{
		bool ok;
		int i = s.toInt( &ok, 16 );
		if( ok )
		{
			v.append( i );
		}
	}
	return v;
}

int Config::bumperTimeout() const
{
	return _settings->value( "Bumper/timeout" ).toInt();
}

int Config::motorTimerInterval() const
{
	return _settings->value( "Motor/timerInterval" ).toInt();
}

int Config::motorUpdateInterval() const
{
	return _settings->value( "Motor/updateInterval" ).toInt();
}

QVector<int> Config::motorSpeedPattern() const
{
	QString str =_settings->value( "Motor/speedPattern" ).toString();
	QStringList l = str.split(",");
	QVector<int> v;
	Q_FOREACH( const QString& s, l )
	{
		bool ok;
		int i = s.toInt( &ok, 10 );
		if( ok )
		{
			v.append( i );
		}
	}
	return v;
}

int Config::motorSpeedDelta() const
{
	return _settings->value( "Motor/speedDelta" ).toInt();
}

QVector<int> Config::motorPositionPattern() const
{
	QString str =_settings->value( "Motor/positionPattern" ).toString();
	QStringList l = str.split(",");
	QVector<int> v;
	Q_FOREACH( const QString& s, l )
	{
		bool ok;
		int i = s.toInt( &ok, 16 );
		if( ok )
		{
			v.append( i );
		}
	}
	return v;
}

int Config::motorPositionDelta() const
{
	return _settings->value( "Motor/positionDelta" ).toInt();
}

int Config::distanceTimeout() const
{
	return _settings->value( "DistanceSensor/timeout" ).toInt();
}

float Config::distanceSensorMin() const
{
	return _settings->value( "DistanceSensor/min" ).toFloat();
}

float Config::distanceSensorMax() const
{
	return _settings->value( "DistanceSensor/max" ).toFloat();
}

int Config::ethernetTimeout() const
{
	return _settings->value( "Ethernet/timeout" ).toInt();
}

QString Config::ethernetRemoteAddress() const
{
	return _settings->value( "Ethernet/remoteAddress" ).toString();
}

int Config::ethernetRemotePort() const
{
	return _settings->value( "Ethernet/remotePort" ).toInt();
}

int Config::ethernetPort() const
{
	return _settings->value( "Ethernet/port" ).toInt();
}

int Config::gyroTimeout() const
{
	return _settings->value( "Gyro/timeout" ).toInt();
}

float Config::gyroDelta() const
{
	return _settings->value( "Gyro/delta" ).toFloat();
}

QString Config::chargerTarget() const
{
	return _settings->value( "Charger/target" ).toString();
}

float Config::chargerStartStateCurrent() const
{
	return _settings->value( "Charger/startStateCurrent" ).toFloat();
}
