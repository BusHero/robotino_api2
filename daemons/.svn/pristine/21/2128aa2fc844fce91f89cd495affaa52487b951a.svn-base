#include "ScriptRunner.h"
#include <QtConcurrentRun>
#include "ConfigReader.h"
#include "configuration.h"

#include "rec/robotino/daemons/Log.h"

ScriptRunner* ScriptRunner::_impl = NULL;

ScriptRunner::ScriptRunner()
: _robviewProgram( currentConfig().value("Robview/path","/usr/bin/robview_interpreter") )
, _robviewOutputMaxSize( currentConfig().value("Robview/outputMaxSize","20").toInt() )
{
}

void ScriptRunner::init()
{
	Q_ASSERT( NULL == _impl );
	_impl = new ScriptRunner;
}

void ScriptRunner::done()
{
	delete _impl;
	_impl = NULL;
}

ScriptRunner::~ScriptRunner()
{
	qDeleteAll( _processMap.values() );
}

void ScriptRunner::runRobotinoView( const QString& programPath )
{
	{
		QMutexLocker lk(&_globalVarsMutex);
		_globalVars.clear();
		_robviewOutput.clear();
	}
	
	QStringList args;
	
	int interval =  currentConfig().value( "Robview/globalVariableOutputInterval", 0 ).toInt();
	if( interval > 0 )
	{
		args << "--global-variable-output-interval";
		args << QString("%1").arg(interval);
	}
	
	args << "-f" << programPath;
	
	runScript(_robviewProgram,args);
}

bool ScriptRunner::isRobotinoViewRunning( const QString& programPath ) const
{
	QProcess* process = _processMap.value( _robviewProgram );
	
	if(process)
	{
		QStringList args = process->arguments();
		if(args.contains(programPath))
		{
			return true;
		}
	}
	
	return false;
}

void ScriptRunner::stopRobotinoView()
{
	QProcess* process = _processMap.value( _robviewProgram );
	
	if(process)
	{
		process->kill();
	}
}

QMap<QString,QString> ScriptRunner::globalVars() const
{
	QMutexLocker lk(&_globalVarsMutex);
	return _globalVars;
}

QStringList ScriptRunner::robviewOutput() const
{
	QMutexLocker lk(&_globalVarsMutex);
	return _robviewOutput;
}

void ScriptRunner::runScript( const QString& path )
{
	runScript( path, QStringList() );
}

void ScriptRunner::runScript( const QString& path, const QStringList& arguments )
{
	qApp->postEvent( this, new RunScriptNoWaitEvent( path, arguments ) );
}

void ScriptRunner::killTree( const QString& path )
{
	qApp->postEvent( this, new KillTreeEvent( path ) );
}

void ScriptRunner::killtreeRecur(Q_PID pid)
{
#ifndef WIN32
	QStringList params;
	QProcess killer;
	params << "--ppid";
	params << QString::number(pid);
	params << "-o";
	params << "pid";
	params << "--noheaders";
	killer.start("/bin/ps",params,QIODevice::ReadOnly);

	rec::robotino::daemons::Log::singleton()->log( QString("killtree:"+QString::number(pid))  ,0);
	if(killer.waitForStarted(-1))
	{
		if(killer.waitForFinished(-1))
		{
			QByteArray temp=killer.readAllStandardOutput();
			QString str=QString::fromLocal8Bit(temp);
			QStringList list=str.split("\n");

			for(int i=0;i<list.size();i++)
			{
				if(!list.at(i).isEmpty())
				{
					killtreeRecur(list.at(i).toInt());
				}
			}
			QString tc = QString("pkill -TERM -P "+QString::number(pid));
			rec::robotino::daemons::Log::singleton()->log( QString("!kill in killtree: %1" ).arg( pid ), 0 );
			QProcess::execute("sudo bash -c \""+ tc + "\"");
		}
	}
#endif
}

void ScriptRunner::killTree( QProcess* process )
{
	if( NULL == process )
	{
		QProcess::execute("sudo killall robview_interpreter");
		return;
	}

	//QString path = Client::singleton()->getParameter( "~/Scripts/killgroup_script", "/usr/local/bin/killgroup.sh" ).toString();
	//QString pid = QString( "%1" ).arg( process->pid() );
	//QStringList arguments = QStringList() << pid;

	//process = new QProcess( this );
	//_processMap[ path ] = process;

	//startProcess( process, path , arguments );
	killtreeRecur(process->pid());
}

void ScriptRunner::runScript_i( const QString& path, const QStringList& arguments )
{
	QProcess* process = _processMap.value( path );
	if( NULL != process )
	{
		killTree( process );
		delete process;
	}

	process = new QProcess( this );
	_processMap[ path ] = process;

	startProcess( process, path , arguments );
}

void ScriptRunner::startProcess( QProcess* process, const QString& path, const QStringList& arguments )
{
	rec::robotino::daemons::Log::singleton()->log( QString( " ScriptRunner::startProcess %1 %2" ).arg( path ).arg( arguments.join( " " ) ) , 2 );

	process->setProcessChannelMode( QProcess::MergedChannels );

	bool success = connect( process, SIGNAL( readyReadStandardError() ), SLOT( on_process_readyRead() ) );
	Q_ASSERT(success);
	
	success = connect( process, SIGNAL( readyReadStandardOutput() ), SLOT( on_process_readyRead() ) );
	Q_ASSERT(success);
	
	success = (bool)connect( process, SIGNAL(started()), SLOT(on_process_started()) );
	Q_ASSERT(success);
	
	success = (bool)connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(on_process_finished(int, QProcess::ExitStatus)));
	Q_ASSERT(success);
	
	success = (bool)connect(process, SIGNAL(error(QProcess::ProcessError)), SLOT(on_process_error(QProcess::ProcessError)));
	Q_ASSERT(success);

	process->start( path , arguments );
}

QString ScriptRunner::getProcessOutput() const
{
	QMutexLocker lk( &_outputMutex );
	QString str = _processOutput;
	_processOutput.clear();
	return str;
}


bool ScriptRunner::hasProcess( const QString& path ) const
{
	QProcess* process = _processMap.value( path );
	return NULL != process;
}

int ScriptRunner::getProcessState( const QString& path ) const
{
	QProcess* process = _processMap.value( path );
	if( NULL == process )
	{
		return -1;
	}
	else
	{
		return process->state();
	}
}

int ScriptRunner::getProcessExitCode( const QString& path ) const
{
	QProcess* process = _processMap.value( path );
	if( NULL == process )
	{
		return -1;
	}
	else
	{
		return process->exitCode();
	}
}

int ScriptRunner::getProcessExitStatus( const QString& path ) const
{
	QProcess* process = _processMap.value( path );
	if( NULL == process )
	{
		return -1;
	}
	else
	{
		return process->exitStatus();
	}
}

void ScriptRunner::parseRobotinoViewOutput(const QByteArray& data)
{
	QMutexLocker lk(&_globalVarsMutex);
		
	QTextStream in(data,QIODevice::ReadOnly);
	QString line = in.readLine();
	while(false == line.isNull() )
	{
		if(line.startsWith("##") && line.endsWith("##"))
		{
			line = line.mid(3,line.length()-6);
			QStringList l = line.split(";;");
			
			if(2 == l.size())
			{
				_globalVars[l.first()] = l.last();
			}
		}
		else
		{
			if(false == line.isEmpty())
			{
				_robviewOutput.append(line);
				if(_robviewOutput.size()>_robviewOutputMaxSize)
				{
					_robviewOutput.removeFirst();
				}				
			}
		}
		
		line = in.readLine();
	}
}

void ScriptRunner::on_process_readyRead()
{
	QMutexLocker lk( &_outputMutex );

	QProcess* process = reinterpret_cast< QProcess* >( sender() );
	QByteArray ba = process->readAll();
	
	if(process->program() == _robviewProgram )
	{
		parseRobotinoViewOutput(ba);
	}

	// QString poutstr = _processOutputMap.value( path );
	// if( NULL != poutstr )
	// {
	// 	poutstr = "";
	// }

	// _processOutputMap[ path ] += str;

	if(rec::robotino::daemons::Log::singleton()->verbosity() > 1 )
	{
		rec::robotino::daemons::Log::singleton()->log( QString(ba), 2 );
	}
}

void ScriptRunner::on_process_started()
{
	QProcess* process = reinterpret_cast< QProcess* >( sender() );

	QString path = _processMap.key( process );

	rec::robotino::daemons::Log::singleton()->log( path + " : started", 2 );
}

void ScriptRunner::on_process_finished( int exitCode, QProcess::ExitStatus exitStatus )
{
	QProcess* process = reinterpret_cast< QProcess* >( sender() );

	QString path = _processMap.key( process );

	rec::robotino::daemons::Log::singleton()->log( QString( "ScriptRunner::on_process_finished %1 : exitcode = %2" ).arg( path ).arg( exitCode ), 2 );
	
	_processMap.remove( path );

	process->deleteLater();
}

void ScriptRunner::on_process_error( QProcess::ProcessError processError)
{
	QProcess* process = reinterpret_cast< QProcess* >( sender() );

	QStringList keys = _processMap.keys( process );
	QString path = keys.join( " " );

	rec::robotino::daemons::Log::singleton()->log( QString( "ScriptRunner::on_process_error %1 : error = %2" ).arg( path ).arg( processError ), 0 );
}

void ScriptRunner::customEvent( QEvent* e )
{
	switch( e->type() )
	{
	case RunScriptNoWaitEventId:
	{
		RunScriptNoWaitEvent* ev = reinterpret_cast<RunScriptNoWaitEvent*>( e );
		runScript_i( ev->path, ev->arguments );
	}
	break;

	case KillTreeEventId:
	{
		KillTreeEvent* ev = reinterpret_cast<KillTreeEvent*>( e );
		killTree( _processMap.value( ev->path ) );
	}
	break;

	default:
		break;
	}
}
