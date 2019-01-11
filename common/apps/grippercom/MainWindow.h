#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QtCore>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "LogView.h"
#include "Com.h"

class MainWindow : public QDialog
{
	Q_OBJECT
public:
	MainWindow();

private Q_SLOTS:
	void on_connectButton_clicked();
	void on_receiveTimer_timeout();
	void on_sensorReadings( float i1, float i2, float ad3, float ad4, float ad6 );
	void on_getSensorReadings_clicked();
	void on_servo1Slider_valueChanged( int value );
	void on_servo2Slider_valueChanged( int value );
	void on_led1_stateChanged( int state );
	void on_led2_stateChanged( int state );

private:
	void updateConnectButton();

	LogView* _logView;
	QPushButton* _connectButton;
	QComboBox* _deviceSelector;
	QPushButton* _getSensorReadings;
	QSlider* _servo1Slider;
	QSlider* _servo2Slider;
	QLabel* _sensorLabel;
	QCheckBox* _led1;
	QCheckBox* _led2;

	Com _com;
	QTimer* _receiveTimer;
};

#endif //_MAINWINDOW_H_
