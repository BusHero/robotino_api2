#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QtGui>
#include <QtWidgets>

#include "rec/grappler/HardwareInfo.h"
#include "rec/grappler/serial/SerialThread.h"
#include "CommandInput.h"

class MainWindow : public QWidget
{
	Q_OBJECT
public:
	MainWindow();
	~MainWindow();

private Q_SLOTS:
	void on_connectButton_clicked();
	void on_getInfoButton_clicked();
	void on_getInfoTimer_timeout();
	void on_commandInput_sendCommand( const QByteArray& /*data*/ );
	void on_commandInput_error( const QString& /*message*/ );
	void on_commandInput_getDetectedServos();
	void on_commandInput_getAllPositions();
	void on_commandInput_sendAllPositions( const QByteArray& /*message*/ );
	void on_commandInput_enableAutoUpdate( bool /*enable*/ );
	void on_commandInput_enablePower( int /*channel*/, bool /*enable*/ );
	void on_commandInput_toggleTorque();
	void on_commandInput_resetDevice( bool /*enterUSBBootloader*/ );

	void on_serialThread_serialPortError( const QString& /*error*/ );
	void on_serialThread_statusReceived( int /*id*/, int /*error*/, const QByteArray& /*data*/ );
	void on_serialThread_servosDetected( const rec::grappler::HardwareInfo& /*info*/ );
	void on_serialThread_allServoPositions( const rec::grappler::HardwareInfo& /*info*/ );
	void on_serialThread_storePosition( const rec::grappler::HardwareInfo& /*info*/ );
	void on_serialThread_allAxesLimits( const rec::grappler::HardwareInfo& /*info*/ );
	void on_serialThread_infoReceived( int major, int minor, int patch );
	void on_serialThread_communicationReady();

	void log( const QString& message );

private:
	QTextDocument _log;
	QTextEdit* _logView;

	QPushButton* _connectButton;
	QPushButton* _getInfoButton;

	CommandInput* _commandInput;

	rec::grappler::serial::SerialThread* _serialThread;

	QTimer _getInfoTimer;
};

#endif //_MAINWINDOW_H_
