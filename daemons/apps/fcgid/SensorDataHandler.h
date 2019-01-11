#ifndef _SENSORDATAHANDLER_
#define _SENSORDATAHANDLER_
#include <QMap>
#include <QtCore>
#include <QVector>

class SensorDataHandler : public QObject
{
	Q_OBJECT
public:
	explicit SensorDataHandler( QObject* parent );

	const QVector< float >& getDistance();
	int timeSinceLastBump() const;

	float getBatteryBoltage() const;
	float getSystemCurrent() const;
	bool isBatteryLow() const;
	int getBatteryLowShutdownCounter() const;
	const QString& getBatteryType() const;
	bool isExtPower() const;
	int getNumChargers() const;

	float getCharger0Bat1temp() const;
	float getCharger0Bat2temp() const;
	float getCharger0BatteryVoltage() const;
	float getCharger0ChargingCurrent() const;
	const QString& getCharger0State() const;
	int getCharger0StateNumber() const;
	unsigned int getCharger0Time() const;

	float getCharger1Bat1temp() const;
	float getCharger1Bat2temp() const;
	float getCharger1BatteryVoltage() const;
	float getCharger1ChargingCurrent() const;
	const QString& getCharger1State() const;
	int getCharger1StateNumber() const ;
	unsigned int getCharger1Time() const ;

	float getCharger2Bat1temp() const ;
	float getCharger2Bat2temp() const ;
	float getCharger2BatteryVoltage() const ;
	float getCharger2ChargingCurrent() const ;
	const QString& getCharger2State() const ;
	int getCharger2StateNumber() const ;
	unsigned int getCharger2Time() const ;

    int getCharger0MajorVer() const ;
    int getCharger0MinorVer() const ;
    int getCharger0PatchVer() const ;
    int getCharger1MajorVer() const ;
    int getCharger1MinorVer() const ;
    int getCharger1PatchVer() const ;
    int getCharger2MajorVer() const ;
    int getCharger2MinorVer() const ;
    int getCharger2PatchVer() const ;

    QVector<bool> getDigitalInput() const ;
    QVector<float> getAnalogInput() const ;
    QVector<bool> getDigitalOutput() const ;
    QVector<bool> getRelay() const ;

	public Q_SLOTS:
	void on_distance_sensors_changed( const QVector< float >& );
	void on_bumper_changed( bool );
	void on_battery_changed( float, float, bool , int , const QString& , bool , int   );
	//void on_odometry_changed( double x, double y, double phi, float vx, float vy, float omega, unsigned int sequence );
	void on_parameters_changed( const QMap< QString, QVariant >& );

	void on_charger0_info_changed( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state );
	void on_charger1_info_changed( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state );
	void on_charger2_info_changed( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state );

    void on_charger0_version_info_changed( int major, int minor, int patch);
    void on_charger1_version_info_changed( int major, int minor, int patch);
    void on_charger2_version_info_changed( int major, int minor, int patch);

    void on_digitalinput_changed( const QVector< bool >& );
    void on_analoginput_changed( const QVector< float >& );
    void on_digitaloutput_changed( const QVector< bool >& );
    void on_relay_changed( const QVector< bool >& );

	private:
	QVector< float > _distances;
	float _battery_voltage;
	float _system_current;
//	double _odoX;
//	double _odoY;
//	double _odoPhi;

	bool _ext_power;
	int _num_chargers;
	QString _batteryType;
	bool _batteryLow;
	int _batteryLowShutdownCounter;

	unsigned int _charger0_time;
	float _charger0_batteryVoltage;
	float _charger0_chargingCurrent;
	float _charger0_bat1temp;
	float _charger0_bat2temp;
	int _charger0_state_number;
	QString _charger0_state;

	unsigned int _charger1_time;
	float _charger1_batteryVoltage;
	float _charger1_chargingCurrent;
	float _charger1_bat1temp;
	float _charger1_bat2temp;
	int _charger1_state_number;
	QString _charger1_state;

	unsigned int _charger2_time;
	float _charger2_batteryVoltage;
	float _charger2_chargingCurrent;
	float _charger2_bat1temp;
	float _charger2_bat2temp;
	int _charger2_state_number;
	QString _charger2_state;

    int _charger0_major;
    int _charger0_minor;
    int _charger0_patch;
    int _charger1_major;
    int _charger1_minor;
    int _charger1_patch;
    int _charger2_major;
    int _charger2_minor;
    int _charger2_patch;

    QVector< bool > _digital_input;
    QVector< float > _analog_input;
    QVector< bool > _digital_output;
    QVector< bool > _relay;

	mutable QMutex _distlock;
	mutable QMutex _bumplock;
	mutable QMutex _batterylock;
	mutable QMutex _charger0lock;
	mutable QMutex _charger1lock;
	mutable QMutex _charger2lock;
    mutable QMutex _chargerverlock;
    mutable QMutex _ioboardslock;

	QTime _lastBump;
};

#endif //_SENSORDATAHANDLER_
