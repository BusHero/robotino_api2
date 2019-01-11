#include "Handler.h"

QTextStream Handler::out( stdout );
QTextStream Handler::err( stderr );

const QString Handler::_texts[20] =
{
	QString( "This is some Text!! " ),
	QString( "his is some Text!! T" ),
	QString( "is is some Text!! Th" ),
	QString( "s is some Text!! Thi" ),
	QString( " is some Text!! This" ),
	QString( "is some Text!! This " ),
	QString( "s some Text!! This i" ),
	QString( " some Text!! This is" ),
	QString( "some Text!! This is " ),
	QString( "ome Text!! This is s" ),
	QString( "me Text!! This is so" ),
	QString( "e Text!! This is som" ),
	QString( " Text!! This is some" ),
	QString( "Text!! This is some " ),
	QString( "ext!! This is some T" ),
	QString( "xt!! This is some Te" ),
	QString( "t!! This is some Tex" ),
	QString( "!! This is some Text" ),
	QString( "! This is some Text!" ),
	QString( " This is some Text!!" ),
};

Handler::Handler( const QString& host, bool localIPCEnabled, int frequency, bool verbose, bool multiThreadedSerialization )
	: _textIndex( 0 )
	, _textTimer( 0 )
{
	if ( frequency > 0 )
	{
		_textTimer = new QTimer;
		_textTimer->setSingleShot( false );
		_textTimer->setInterval( 1000 / frequency );
	}

	bool ok = true;
	ok &= (bool)connect( &_client, SIGNAL( connected() ), SLOT( on_client_connected() ) );
	ok &= (bool)connect( &_client, SIGNAL( disconnected( rec::rpc::ErrorCode ) ), SLOT( on_client_disconnected() ) );
	if ( verbose )
		ok &= (bool)connect( &_client, SIGNAL( textReceived( const QString& ) ), SLOT( on_client_textReceived( const QString& ) ) );

	if ( _textTimer )
		ok &= (bool)connect( _textTimer, SIGNAL( timeout() ), SLOT( on_textTimer_timeout() ) );
	Q_ASSERT(ok);

	out << "Simple REC RPC client test app version " << rec::rpc::getLibraryVersionString() << endl;

	_client.setMultiThreadedSerializationEnabled( multiThreadedSerialization );
	_client.setLocalIPCEnabled( localIPCEnabled );
	_client.setAddress( host );
	_client.connectToServer( 2000U );
}

Handler::~Handler()
{
	_client.setTopicsEnabled( false );
	out << "Shut down" << endl;
}

void Handler::on_client_connected()
{
	_client.setTopicsEnabled( true );
	out << "Client connected" << endl;
	out << "Server version is " << _client.getServerVersion() << endl;
	out << "Hit Ctrl+C to shut down" << endl;
	if ( _textTimer )
		_textTimer->start();
}

void Handler::on_client_disconnected()
{
	out << "Client disconnected" << endl;
	if ( _textTimer )
		_textTimer->stop();
	qApp->quit();
}

void Handler::on_client_textReceived( const QString& text )
{
	out << "Text received: " << text << endl;
}

void Handler::on_textTimer_timeout()
{
	_client.publishText( _texts[ _textIndex ] );
	++_textIndex;
	_textIndex %= 20;
}
