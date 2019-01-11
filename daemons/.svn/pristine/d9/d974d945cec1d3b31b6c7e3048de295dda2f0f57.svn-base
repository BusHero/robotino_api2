#include "WatchDog.h"

WatchDog::WatchDog()
: QObject( NULL )
, _process( new QProcess( this ) )
, _maxCPULoad( 100.0 )
, _watchDogDelay( 10 )
{
	_process = new QProcess( this );
	bool connected = true;
	connected = connect( _process, SIGNAL( readyRead() ), SLOT( on_process_readyRead() ) );
	Q_ASSERT( connected );

	connected = connect( _process, SIGNAL( finished( int, QProcess::ExitStatus ) ), SLOT( on_process_finished( int, QProcess::ExitStatus ) ) );
	Q_ASSERT( connected );
	
	connected = connect( _process, SIGNAL( error( QProcess::ProcessError ) ), SLOT( on_process_error( QProcess::ProcessError ) ) );
	Q_ASSERT( connected );

	_process->start( "top", QStringList() << "-b" << "-p" << QString("%1").arg( qApp->applicationPid() ) );
}

WatchDog::~WatchDog()
{
	if( _process )
	{
		_process->kill();
		_process->waitForFinished( 10000 );
	}

	delete _process;
	_process = NULL;
}

void WatchDog::setMaxCPULoad( int maxCPULoad )
{
	_maxCPULoad = maxCPULoad;
}

void WatchDog::setWatchDogDelay( int watchDogDelay )
{
	_watchDogDelay = watchDogDelay;
}

void WatchDog::on_process_readyRead()
{
	QByteArray ba = _process->readAll();
	QBuffer buffer( &ba );
	buffer.open( QBuffer::ReadOnly );

	QByteArray line = buffer.readLine();
	while( false == line.isEmpty() )
	{
		QString lineStr( line );
		//Q_EMIT log( lineStr, 0 );

		QStringList l = lineStr.split( QRegExp("\\s"), QString::SkipEmptyParts );

		//Q_FOREACH( const QString& str, l )
		//{
		//	Q_EMIT log( str, 0 );
		//}

		if( 12 == l.size() )
		{
			int pid = l.at(0).toInt();
			float pcpu = l.at(8).toFloat();

			if( pid == qApp->applicationPid() )
			{
				Q_EMIT log( QString("CPU load is %1").arg( pcpu ), 2 );

				if( pcpu > _maxCPULoad )
				{
					Q_EMIT log( QString("CPU load higher than max CPU load of %1\%").arg( _maxCPULoad ), 0 );

					if( _watchDogTime.isNull() )
					{
						_watchDogTime.start();
					}
					else if( _watchDogTime.elapsed() > 1000*_watchDogDelay )
					{
						Q_EMIT log( QString("Watchdog bit after %1 seconds").arg( _watchDogTime.elapsed()/1000 ), 0 );
						_watchDogTime = QTime();
						Q_EMIT bit();
					}
				}
				else
				{
					_watchDogTime = QTime();
				}

				break;
			}
		}

		line = buffer.readLine();
	}
}

void WatchDog::on_process_finished( int exitCode, QProcess::ExitStatus exitStatus )
{
}

void WatchDog::on_process_error( QProcess::ProcessError error )
{
}
