#ifndef _SENSORVIEW_H_
#define _SENSORVIEW_H_

#include <QtCore>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "Com.h"

class SensorView : public QTableWidget
{
	Q_OBJECT
public:
	SensorView( QWidget* parent, Com* com );

Q_SIGNALS:
	void getHardwareVersion();

public Q_SLOTS:
	void reset();
	void setHardwareVersion( const QString& );
	void setSoftwareVersion( const QString& );
	void setDistanceSensorReadings( const QVector<float>& );
	void setAllMotorSpeeds( const QVector<int>& );
	void setAllMotorPositions( const QVector<int>& );
	void setAllMotorPidParameters( const QVector<float>& kp, const QVector<float>& ki, const QVector<float>& kd );
	void setOdometry( float x, float y, float rot );
	void setAllMotorCurrentReadings( const QVector<float>& );
	void setAllAnalogInputs( const QVector<float>& );
	void setAllDigitalInputs( int value );
	void setBumper( bool on );
	void setPowerButton( bool pressed );
	void setFpgaPower( bool on );
	void setPwrOkState( bool high );
	void setComExpressStates( bool sus_s3, bool sus_s4, bool sus_s5, bool thrm, bool thrmtrip );
	void setAllMotorReadings( const QVector<int>&, const QVector<int>&, const QVector<float>& );
	void setIpAddress( quint32 address, quint32 netmask );
	void setPowerSourceReadings( float battery_voltage, float system_current, bool ext_power, int num_chargers, const QString& batteryType, bool batteryLow, int batteryLowShutdownCounter );
	void setMotorAccelLimits( int motor, float minaccel, float maxaccel );
	void setGyroZAngle( float angle, float velocity );
	void setCanMsg( unsigned short id, const QByteArray& data );
	void setNrst( bool on );
	void setBoot( int val );
	void setChargerInfo( int chargerID, unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state );
	void setChargerVersion( int chargerID, int major, int minor, int patch );
	void setChargerError( int chargerID, unsigned int time, const QString& message );
	void setBatteryMin( float voltage );
	void setGpain( int, float );
	void setVersionBits( bool, bool );
	void setGyroParam( float bias, float scale );
	void setFestoolChargerInfo(QVector<unsigned int> /*time*/, QVector<bool> /*accuLoading*/, QVector<bool> /*accuConnected*/, QVector<bool> /*externalPower*/, QVector<bool> /*chargerConnected*/, QVector<float> /*voltages*/, QVector<int> /*capacities*/, QString /*message*/);
	void setMotorDebug(int motor, int startTime, int deltaTime, QVector<int> speeds, QVector<int> speed_setpoints, QVector<float> currents, QVector<int> control_points);

private Q_SLOTS:
	void on_itemClicked( QTableWidgetItem* item );

private:
	Com* _com;
};

#endif //_SENSORVIEW_H_
