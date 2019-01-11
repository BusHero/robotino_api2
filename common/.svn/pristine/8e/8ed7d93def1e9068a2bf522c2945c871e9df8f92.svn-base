#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <QtGui>

class Config
{
public:
	static const Config& singleton()
	{
		return *_impl;
	}

	static void init();
	static void done();

	QString configFilePath() const;

	QString pathToUV4() const;
	QString pathToFirmware() const;
	QString firmwareProject() const;
	QString firmwareTarget() const;
	QString debugComPort() const;
	QString firmwareVersionFile() const;

	QString pathToFTProg() const;
	QString ft232TemplateFile() const;

	int firmwareVersionReadTimeout() const;

	int digitalTimerInterval() const;
	QVector<int> digitalTestPattern() const;

	QVector<float> analogTestPattern() const;
	float analogDelta() const;

	int relayTimerInterval() const;
	QVector<int> relayTestPattern() const;

	int bumperTimeout() const;

	int motorTimerInterval() const;
	int motorUpdateInterval() const;
	QVector<int> motorSpeedPattern() const;
	int motorSpeedDelta() const;
	QVector<int> motorPositionPattern() const;
	int motorPositionDelta() const;

	int distanceTimeout() const;
	float distanceSensorMin() const;
	float distanceSensorMax() const;

	int ethernetTimeout() const;
	QString ethernetRemoteAddress() const;
	int ethernetRemotePort() const;
	int ethernetPort() const;

	int gyroTimeout() const;
	float gyroDelta() const;

	QString chargerTarget() const;
	float chargerStartStateCurrent() const;

private:
	Config();
	~Config();

	QSettings* _settings;
	QString _configFilePath;

	static Config* _impl;
};

#endif //_CONFIG_H_
