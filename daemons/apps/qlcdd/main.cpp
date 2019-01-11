#include <QtGui>

#include "qlcdd_version.h"
#include "MainWindow.h"
#include "rec/robotino/rpc/Client.h"
#include <iostream>

rec::robotino::rpc::Client* client = NULL;

void printHelp()
{
	QString text;
	QTextStream s( &text );
	s << "qlcdd version " << BuildVerStr << '\n';
	s << "qlcdd [OPTIONS]" << '\n';
	s << "options:" << '\n';
	s << "-help : Print this help message\n";
	s << "/? : Print this help message\n";
	s << "-address=ip:port : Set IP address and port of RPC server (default 127.0.0.1:8000)\n";
	s << "-disablelocalipc : Disable local IPC connections";
	QMessageBox::information( NULL, "qlcdd options", text );
}

int main( int argc, char** argv )
{
	QApplication app( argc, argv );
	app.setApplicationName( "qlcdd" );

	QString address = "127.0.0.1:12080";
	bool disablelocalipc = false;

	QStringList args = app.arguments();
	args.removeFirst();
	Q_FOREACH( const QString& str, args )
	{
		if( str.startsWith( "-address=" ) )
		{
			address = str;
			address.remove( "-address=" );
		}
		else if( "-disablelocalipc" == str )
		{
			disablelocalipc = true;
		}
		else
		{
			printHelp();
			return 1;
		}
	}

	client = new rec::robotino::rpc::Client;

	MainWindow mw;

	client->setAddress( address );
	client->setAutoReconnectEnabled( true );
	client->setLocalIPCEnabled( !disablelocalipc );
	client->set_rec_robotino_rpc_display_text_enabled( true );
	client->set_rec_robotino_rpc_display_backlight_enabled( true );
	client->set_rec_robotino_rpc_display_vbar_enabled( true );
	client->set_rec_robotino_rpc_display_hbar_enabled( true );
	client->set_rec_robotino_rpc_display_progress_enabled( true );
	client->set_rec_robotino_rpc_display_clear_enabled( true );
	client->connectToServer();

	mw.show();

	app.exec();

	delete client;
}
