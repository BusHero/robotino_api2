#ifndef _InstallFirmwarePage_H_
#define _InstallFirmwarePage_H_

#include <QtGui>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QtWidgets>

class LogList;

class InstallFirmwarePage : public QWizardPage
{
	Q_OBJECT
public:
	InstallFirmwarePage();

private Q_SLOTS:
	void on_installFirmwareButton_clicked();
	void on_firmwareProgrammingProcess_finished( int exitCode, QProcess::ExitStatus exitStatus );
	void on_firmwareProgrammingProcess_error( QProcess::ProcessError error );

	void on_wizard_currentIdChanged(int);

	void on_serialPort_readyRead();

	void on_serialReadTimer_timeout();

private:
	bool _firmwareFound;

	QPushButton* _installFirmwareButton;
	QProcess* _firmwareProgrammingProcess;

	QSerialPort* _serialPort;
	LogList* _logList;

	void initializePage();
	bool isComplete() const;

	QTimer* _serialReadTimer;
	QString _currentLine;

	bool _phyConfigured;
};

#endif //_InstallFirmwarePage_H_
