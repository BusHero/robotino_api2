#ifndef _GyroscopeProgramPage_H_
#define _GyroscopeProgramPage_H_

#include <QtGui>
#include <QtWidgets>

class LogList;

class GyroscopeProgramPage : public QWizardPage
{
	Q_OBJECT
public:
	GyroscopeProgramPage();

private Q_SLOTS:
	void on_startProgramButton_clicked();
	void on_startScanButton_clicked();

	void on_programmingProcess_finished(int exitCode, QProcess::ExitStatus exitStatus);
	void on_programmingProcess_error(QProcess::ProcessError error);
	void on_programmingProcess_readyReadStandardOutput();
	void on_programmingProcess_readyReadStandardError();

	void on_scanProcess_finished(int exitCode, QProcess::ExitStatus exitStatus);
	void on_scanProcess_error(QProcess::ProcessError error);
	void on_scanProcess_readyReadStandardOutput();
	void on_scanProcess_readyReadStandardError();

	void on_wizard_currentIdChanged(int);

private:
	bool _programmingOk;
	bool _scanOk;

	QPushButton* _startProgramButton;
	QPushButton* _startScanButton;
	QProcess* _programmingProcess;
	QProcess* _scanProcess;

	LogList* _logList;

	void initializePage();
	bool isComplete() const;
};

#endif //_GyroscopeProgramPage_H_
