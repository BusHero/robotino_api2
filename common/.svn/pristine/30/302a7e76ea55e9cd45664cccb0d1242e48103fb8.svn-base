#ifndef _COMMANDINPUT_H_
#define _COMMANDINPUT_H_

#include <QtGui>
#include <QtWidgets>

#include "rec/grappler/HardwareInfo.h"

class PositionStorage;
class PositionTable;

class CommandInput : public QWidget
{
	Q_OBJECT
public:
	CommandInput( QWidget* parent );

	void setServosDetected( const rec::grappler::HardwareInfo& info );
	void setAllServoPositions( const rec::grappler::HardwareInfo& info );
	void storePosition( const rec::grappler::HardwareInfo& info );
	void setAxesLimits( const rec::grappler::HardwareInfo& info );

Q_SIGNALS:
	void sendCommand( const QByteArray& /*data*/ );
	void error( const QString& /*message*/ );
	void getDetectedServos();
	void sendAllPositions( const QByteArray& /*message*/ );
	void getAllPositions();
	void enableAutoUpdate( bool /*enable*/ );
	void enablePower( int /*channel*/, bool /*enable*/ );
	void toggleTorque();
	void resetDevice( bool );

private Q_SLOTS:
	void on_positionStorage_load( const QString& );
	void on_getDetectedServos_clicked();
	void on_getAllPositions_clicked();
	void on_sendAllPositions_clicked();
	void on_dynamixelcommand_sendCommand( const QByteArray& command );
	void on_dynamixelcommand_error( const QString& errorMessage );
	void on_enableAutoUpdateCheckBox_stateChanged( int );
	void on_rx64power_stateChanged( int );
	void on_rx28power_stateChanged( int );
	void on_rx10power_stateChanged( int );
	void on_storePosition_clicked();
	void on_toggleTorque_clicked();
	void on_setCWAngleLimits_clicked();
	void on_setCCWAngleLimits_clicked();
	void on_resetDevice_clicked();

private:
	QPushButton* _getDetectedServos;
	PositionTable* _positionTable;
	PositionStorage* _positionStorage;
	bool _getAllPositionsClicked;

	QCheckBox* _enterUSBBootloader;

	rec::grappler::HardwareInfo _currentInfo;
	rec::grappler::HardwareInfo _servosDetectedInfo;
};

#endif //_COMMANDINPUT_H_
