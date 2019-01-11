#ifndef _TestChargerPage_H_
#define _TestChargerPage_H_

#include <QtGui>
#include "IOTestPage.h"

class TestChargerPage : public IOTestPage
{
	Q_OBJECT
public:
	TestChargerPage( int number );

private Q_SLOTS:
	void on_chargerInfo( int chargerID, unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state );
	void on_chargerVersion( int chargerID, int major, int minor, int patch );
	void on_chargerError( int chargerID, unsigned int time, const QString& message );
	void on_powerSourceReadings( float battery_voltage, float system_current, bool ext_power, int num_chargers, const QString& batteryType, bool batteryLow, int batteryLowShutdownCounter );

	void on_clearErrorButton_clicked();

private:
	virtual bool startButtonClicked();
	virtual void comTagRequest();

	const int _number;

	typedef enum
	{
		ResetState,
		StartState,
		Stage1State,
		Stage2State,
		Stage3State,
		Stage4State,
		FinishedState
	} State_t;

	State_t _state;

	QLabel* _timeLabel;
	QLabel* _batteryVoltageLabel;
	QLabel* _chargingCurrentLabel;
	QLabel* _bat1tempLabel;
	QLabel* _bat2tempLabel;
	QLabel* _state_strLabel;
	QLabel* _versionLabel;
	QLabel* _errorLabel;
	QPushButton* _clearErrorButton;

	bool _startStateCurrentSeen;
};

#endif //_TestChargerPage_H_
