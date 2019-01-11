#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QtCore>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

class LogView;
class CANHandler;
class ChargerView;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();

private Q_SLOTS:
	void on_about_triggered();
	void on_help_triggered();
	void on_chargerView_triggered();
	void on_chargerView_closed();

private:
	LogView* _logView;
	CANHandler* _canHandler;

	QMdiArea* _mdiArea;

	void createCharger( int id );
};

#endif //_MAINWINDOW_H_
