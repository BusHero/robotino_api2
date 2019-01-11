#ifndef _LOGVIEW_H_
#define _LOGVIEW_H_

#include <QtCore>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

class LogView : public QListWidget
{
	Q_OBJECT
public:
	LogView( QWidget* parent );

public Q_SLOTS:
	void info( const QString& message );
	void warning( const QString& message );
	void error( const QString& message );
	void log( const QString& message );
};

#endif //_LOGVIEW_H_
