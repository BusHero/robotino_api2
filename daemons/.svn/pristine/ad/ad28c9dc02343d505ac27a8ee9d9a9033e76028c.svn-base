#include "action/Execute.h"
#include "Client.h"
#include "Translator.h"

extern Client* client;

using namespace action;

Execute::Execute( Action* parent )
: Action( parent )
, _process( NULL )
, _timer( new QTimer( this ) )
, _step( 0 )
, _abortWithKeyAllowd( true )
, _displayWidth( 0 )
, _currentItem( 0 )
, _maxProcessOutputSize( 100 )
{
	_timer->setSingleShot( false );
	_timer->setInterval( 100 );

	bool connected = true;
	connected &= connect( _timer, SIGNAL( timeout() ), SLOT( on_timer_timeout() ) );
	Q_ASSERT( connected );
}

Execute::~Execute()
{
}

void Execute::customActivate( Action* before )
{
	client->set_controller_active_enabled( false );
	_displayWidth = client->getParameter( "lcdd2/width", 16 ).toInt();
	_maxProcessOutputSize = client->getLocalParameter( "execute/maxProcessOutputSize", 100 ).toInt();
	_currentItem = 0;
	_processOutput.clear();
	_step = 0;

	displayClear();

	if( _attributes.contains( "actiontitle" ) )
	{
		_currentTitle = TT( attribute( "actiontitle" ) );
	}
	else
	{
		_currentTitle = TT( title() );
	}

	setDisplayText( _currentTitle, 0, 0, true, true );

	if( "false" == attribute( "abortwithkey" ) )
	{
		_abortWithKeyAllowd = false;
	}
	else
	{
		_abortWithKeyAllowd = true;
	}

	Q_ASSERT( NULL == _process );
	_process = new QProcess( this );
	bool connected = true;
	connected &= connect( _process, SIGNAL( readyRead() ), SLOT( on_process_readyRead() ) );
	connected &= connect( _process, SIGNAL( finished( int, QProcess::ExitStatus ) ), SLOT( on_process_finished( int, QProcess::ExitStatus ) ) );
	connected &= connect( _process, SIGNAL( error( QProcess::ProcessError ) ), SLOT( on_process_error( QProcess::ProcessError ) ) );
	Q_ASSERT( connected );
	if( _attributes.contains( "wd" ) )
	{
		_process->setWorkingDirectory( attribute( "wd" ) );
	}
	QString command;
	if( _attributes.contains( "command" ) )
	{
		command = attribute( "command" );
	}
	else
	{
		command = attribute( "script" );
	}

	QStringList params = parameters();

	QString logStr = _process->workingDirectory() + "/" + command + " ";

	Q_FOREACH( const QString& param, params )
	{
		logStr +=  param + " ";
	}

	log( logStr, 1 );

	_timer->start();
	_process->start( command, params );
}

void Execute::customLeave( Action* next )
{
	_timer->stop();

	if( _process )
	{
		_process->kill();
	}

	delete _process;
	_process = NULL;

	client->set_controller_active_enabled( true );
}

void Execute::on_process_readyRead()
{
	QByteArray ba = _process->readAll();
	QString str( ba );

	if( str.isEmpty() )
	{
		return;
	}

	log( str, 1 );

	QStringList l = str.split( "\n", QString::SkipEmptyParts );

	_processOutput += l;

	while( _processOutput.size() > _maxProcessOutputSize )
	{
		_processOutput.removeFirst();
	}

	_currentItem = _processOutput.size() - 1;
	drawProcessOutput();
}

void Execute::on_process_finished( int exitCode, QProcess::ExitStatus exitStatus )
{
	if( 0 == exitCode )
	{
		log( QString( "Process finished normally - Code :%1" ).arg( exitCode ), 1 );
		_processOutput.append( TT( "erfolgreich" ) );
	}
	else
	{
		log( QString( "Process failed - Code :%1" ).arg( exitCode ), 1 );
		_processOutput.append( TT( "fehlgeschlagen" ) );
	}

	_processOutput.append( TT( "Weiter m. Taste" ) );

	_currentItem = _processOutput.size() - 1;

	drawProcessOutput();

	_timer->stop();

	activateFinishedAction();
}

void Execute::on_process_error( QProcess::ProcessError error )
{
	switch( error )
	{
	case QProcess::FailedToStart:
		_processOutput.append( TT( "failed to start" ) );
		log( QString( "Process failed to start" ), 1 );
		break;

	case QProcess::Crashed:
		_processOutput.append( TT( "crashed" ) );
		log( QString( "Process crashed" ), 1 );
		break;

	case QProcess::Timedout:
		_processOutput.append( TT( "timed out" ) );
		log( QString( "Process timed out" ), 1 );
		break;

	case QProcess::WriteError:
		_processOutput.append( TT( "write error" ) );
		log( QString( "Process write error" ), 1 );
		break;

	case QProcess::ReadError:
		_processOutput.append( TT( "read error" ) );
		log( QString( "Process read error" ), 1 );
		break;

	default:
		_processOutput.append( TT( "unknown error" ) );
		log( QString( "Process unknown error" ), 1 );
		break;
	}

	_processOutput.append( TT( "Weiter m. Taste" ) );

	_currentItem = _processOutput.size() - 1;

	drawProcessOutput();

	_timer->stop();

	activateFinishedAction();
}

void Execute::on_timer_timeout()
{
	_step = _step++ % 10;
	
	setDisplayText( QString::number( _step ), 0, _displayWidth-1, false, true );
}

void Execute::back()
{
	if( _abortWithKeyAllowd || ( _process && QProcess::NotRunning == _process->state() ) )
	{
		activate( _parent );
	}
}

void Execute::enter()
{
	if( _abortWithKeyAllowd || ( _process && QProcess::NotRunning == _process->state() ) )
	{
		activate( _parent );
	}
}

void Execute::up()
{
	if( _currentItem > 2 )
	{
		--_currentItem;
		drawProcessOutput();
	}
}

void Execute::down()
{
	if( _currentItem < _processOutput.size()-1 )
	{
		++_currentItem;
		drawProcessOutput();
	}
}

void Execute::drawProcessOutput()
{
	setDisplayBacklight( true );

	if( _processOutput.isEmpty() )
	{
		return;
	}

	if( _currentItem >= _processOutput.size() )
	{
		_currentItem = _processOutput.size() - 1;
	}

	int j=3;
	for( int i=_currentItem; i>=0 && j>0; --i, --j )
	{
		setDisplayText( _processOutput[i], j, 0, true, true );
	}
}

void Execute::activateFinishedAction()
{
	if( false == attribute( "finished-action" ).isEmpty() )
	{
		QString title = attribute( "finished-action" );
		setAttribute( "finished-action", QString::null );
		Action* a = find( title );
		if( a )
		{
			activate( a );
		}
	}
}