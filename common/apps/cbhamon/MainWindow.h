#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QtCore>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "rec/robotinoxt/serial/SerialThread.h"

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
	void on_resetButton_clicked();
	void on_sendSetPressuresButton_clicked();

	void on_compressorsCb_toggled( bool checked );
	void on_waterDrainValveCb_toggled( bool checked );
	void on_gripperValve1Cb_toggled( bool checked );
	void on_gripperValve2Cb_toggled( bool checked );

	void on_serial_serialPortError( const QString& error );
	void on_serial_infoReceived( int major, int minor, int patch );
	void on_serial_statusReceived( const QVector< qint16 >& pressures, bool pressureSensor, const QVector< float >& potis );

	void log( const QString& message );

private:
	QTextDocument _log;
	QTextEdit* _logView;

	QPushButton* _connectButton;
	QPushButton* _getInfoButton;
	QPushButton* _resetButton;
	QCheckBox* _enterBootloader;

	QDoubleSpinBox* _pressures[8];
	QPushButton* _sendSetPressuresButton;

	QCheckBox* _compressorsCb;
	QCheckBox* _waterDrainValveCb;
	QCheckBox* _gripperValve1Cb;
	QCheckBox* _gripperValve2Cb;

	rec::robotinoxt::serial::SerialThread* _serial;

	QTimer _getInfoTimer;

	QTime _statusTime;

	int _maxPressure;
};

#endif //_MAINWINDOW_H_
