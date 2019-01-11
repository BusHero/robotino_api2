#ifndef _SCRIPTRUNNER_H_
#define _SCRIPTRUNNER_H_

#include <QtCore>
#include <QTime>
#include <QStringList>

/**
 * For executing a program as a process.
 */
class ScriptRunner: public QObject
{
	Q_OBJECT
public:
	static ScriptRunner* singleton()
	{
		Q_ASSERT( _impl );
		return _impl;
	}

	static void init();
	static void done();

	~ScriptRunner();

	void runScript( const QString& path );
	void runScript( const QString& path, const QStringList& arguments  );
	
	void killTree( const QString& path );
	void killtreeRecur(Q_PID pid);

	QString getProcessOutput() const;

	bool hasProcess( const QString& path ) const;
	int getProcessState( const QString& path ) const;
	int getProcessExitCode( const QString& path ) const;
	int getProcessExitStatus( const QString& path ) const;
	/* QString getProcessOutputByPath(const QString& path) const; */

private Q_SLOTS:
	void on_process_readyRead();
	void on_process_finished( int exitCode, QProcess::ExitStatus exitStatus );
	void on_process_error( QProcess::ProcessError );
	void on_process_started();

private:
	ScriptRunner();
	
	void customEvent( QEvent* e );
	
	void runScript_i( const QString& path, const QStringList& arguments );
	void killTree( QProcess* process );//kill the whole process tree
	void startProcess( QProcess* process, const QString& path, const QStringList& arguments );

	static ScriptRunner* _impl;

	QMap< QString, QProcess* > _processMap;
	QMap< QString, Q_PID > _pidMap;
	
	/* QMap<QString, QString> _processOutputMap; */
	mutable QMutex _outputMutex;

	mutable QString _processOutput; 

	enum
	{
		RunScriptNoWaitEventId = QEvent::User,
		KillTreeEventId
	};

	class RunScriptNoWaitEvent : public QEvent
	{
	public:
		RunScriptNoWaitEvent( const QString& path_, const QStringList& arguments_ )
			: QEvent( (QEvent::Type)RunScriptNoWaitEventId )
			, path( path_ )
			, arguments( arguments_ )
		{
		}

		QString path;
		QStringList arguments;
	};

	class KillTreeEvent : public QEvent
	{
	public:
		KillTreeEvent( const QString& path_ )
			: QEvent( (QEvent::Type)KillTreeEventId )
			, path( path_ )
		{
		}

		QString path;
	};
};


#endif
