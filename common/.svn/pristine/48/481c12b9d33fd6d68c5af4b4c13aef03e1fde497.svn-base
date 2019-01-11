#ifndef _ACTIONS_H_
#define _ACTIONS_H_

#include <QtCore>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

class Actions : public QObject
{
	Q_OBJECT
public:
	static Actions& singleton();

	QAction* chargerView[3];
	QAction* log;

	QAction closeActiveWindow;
	QAction closeAllWindows;
	QAction tileWindows;
	QAction cascadeWindows;
	QAction nextWindow;
	QAction previousWindow;

	QAction about;

	QAction help;

private:
	Actions();

private Q_SLOTS:
	void translate();
};

#endif //_ACTIONS_H_
