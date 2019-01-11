#include "Handler.h"
#include <iostream>
#include "Client.h"
#include "KinectImpl.h"

extern Client* client;

Handler::Handler( unsigned int device, unsigned int channel )
: QThread()
, _device( device )
, _channel( channel )
, _watchDog( new QTimer( this ) )
{
	bool connected = true;

	connected &= connect( _watchDog, SIGNAL( timeout() ), SLOT( on_watchDog_timeout() ) );
	Q_ASSERT( connected );

	_watchDog->setInterval( client->getParameter( "~/watchdog", 500 ).toInt() );
	_watchDog->setSingleShot( true );
}

Handler::~Handler()
{
	delete _watchDog;
	_watchDog = NULL;

	Q_EMIT log( "kinectd stopped", 1 );
}

void Handler::init()
{
	Q_EMIT log( "kinectd started", 1 );

	qApp->postEvent( this, new StartEvent );
}

void Handler::on_error()
{
	qApp->exit( 1 );
}

void Handler::on_watchDog_timeout()
{
	qApp->exit( 1 );
}

void Handler::customEvent( QEvent* e )
{
	switch( e->type() )
	{
	case StartEventId:
		{
			start( QThread::TimeCriticalPriority );
			_watchDog->start();
		}
		break;
	}
}


void Handler::run()
{
	KinectImpl impl( _device, _channel );

	bool connected = true;
	
	connected &= connect( &impl, SIGNAL( error() ), SLOT( on_error() ) );
	Q_ASSERT( connected );

	connected &= connect( &impl, SIGNAL( success() ), _watchDog, SLOT( start() ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= connect( &impl, SIGNAL( log( const QString&, int ) ), SIGNAL( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	if( false == impl.start() )
	{
		qApp->exit( 1 );
		return;
	}

	exec();
}