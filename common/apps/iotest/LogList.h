#ifndef _LOGLIST_H_
#define _LOGLIST_H_

#include <QtGui>
#include <QtWidgets>

class LogEntry
{
public:
	typedef enum { FINE, INFO, WARNING, FATAL, RUNNING } Type;

	LogEntry( Type t, const QString& str )
		: type( t )
		, message( str )
	{
	}

	Type type;
	const QString message;
};

class LogList : public QListWidget
{
	Q_OBJECT
public:
	LogList(QWidget* parent = 0);
	~LogList();

	QSize sizeHint() const;

	void addLogEntry(const LogEntry& entry);

	void append( const QString& str );

	QString currentText() const;

	private Q_SLOTS:
		void on_runningTimer_timeout();

private:
	static QIcon iconFine;
	static QIcon iconInfo;
	static QIcon iconWarning;
	static QIcon iconError;
	static QIcon iconRunning[2];

	void startRunningTimer();
	void stopRunningTimer();

	QTimer* _runningTimer;
	int _runningIndex;

	QListWidgetItem* _currentLogItem;
};

inline QString LogList::currentText() const
{
	if( _currentLogItem )
	{
		return _currentLogItem->text();
	}
	else
	{
		return QString::null;
	}
}

#endif	// _LOGLIST_H_
