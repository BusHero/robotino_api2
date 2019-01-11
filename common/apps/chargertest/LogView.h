#ifndef _LOGVIEW_H_
#define _LOGVIEW_H_

#include <QtCore>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "CANhandler.h"

class LogView : public QDockWidget
{
	Q_OBJECT
public:
	LogView( QWidget* parent );

public Q_SLOTS:
	void info( const QString& message );
	void warning( const QString& message );
	void error( const QString& message );
	void log( const QString& message );
	void canMessage( const CANMessage& );

private Q_SLOTS:
	void on_log_triggered();

private:
	QListWidget* _listWidget;

	void showEvent( QShowEvent* );
	void hideEvent( QHideEvent* );
};

#endif //_LOGVIEW_H_
