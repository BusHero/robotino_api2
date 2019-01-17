#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QtGui>

#include "rec/grappler/HardwareInfo.h"
#include "rec/grappler/serial/SerialThread.h"

class MainWindow : public QWidget
{
	Q_OBJECT
public:
	MainWindow();
	~MainWindow();

private Q_SLOTS:
	void on_connectTimer_timeout();
	void on_getInfoTimer_timeout();
	void on_bootloaderWaitTimer_timeout();
	void on_passiveBootloaderWaitTimer_timeout();
	void on_startUpdatePb_clicked();

	void on_serialThread_serialPortError( const QString& /*error*/ );
	void on_serialThread_infoReceived( int major, int minor, int patch );

	void log( const QString& message );

private:
	QLabel* _currentVersionLabel;
	QLabel* _updateLabel;
	QPushButton* _startUpdatePb;

	QTextDocument _log;
	QTextEdit* _logView;

	rec::grappler::serial::SerialThread* _serialThread;

	QTimer _connectTimer;
	QTimer _getInfoTimer;
	QTimer _bootloaderWaitTimer;
	QTimer _passiveBootloaderWaitTimer;

	int _bootloaderWaitCounter;
	bool _updating;
	bool _manualUpdate;
	QChar _driveLetter;

	void setUpdating( bool enable );
	void uploadFirmware();
	void uploadFinished();

	void closeEvent( QCloseEvent* ev );
};

#endif //_MAINWINDOW_H_
