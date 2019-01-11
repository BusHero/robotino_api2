#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QtGui>

#include "LogView.h"
#include "Com.h"
#include "SensorView.h"
#include "ActorView.h"

class MainWindow : public QDialog
{
	Q_OBJECT
public:
	MainWindow();

private Q_SLOTS:
	void on_connectButton_clicked();
	void on_transmitButton_clicked();
	void on_receiveTimer_timeout();

private:
	void updateConnectButton();

	LogView* _logView;
	QPushButton* _connectButton;
	QComboBox* _deviceSelector;
	QPushButton* _transmitButton;
	SensorView* _sensorView;
	ActorView* _actorView;

	Com _iocom;
	QTimer* _receiveTimer;
};

#endif //_MAINWINDOW_H_
