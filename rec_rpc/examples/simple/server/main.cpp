/*
Copyright (c) 2011, REC Robotics Equipment Corporation GmbH, Planegg, Germany
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
- Neither the name of the REC Robotics Equipment Corporation GmbH nor the names of
  its contributors may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Handler.h"
#include "SignalHandler.h"

void showHelp( int exitCode )
{
	Handler::out << "REC RPC simple server test application" << endl;
	Handler::out << endl;
	Handler::out << "rec_rpc_simple_server_test [OPTIONS]" << endl;
	Handler::out << "options:" << endl;
	Handler::out << "-h, --help                       : Show this message." << endl;
	Handler::out << "-p, --port PORT                  : Listen on port PORT." << endl;
	Handler::out << "-d, --disableLocalIPC            : Allow TCP connections only." << endl;
	Handler::out << "-m, --multiThreadedSerialization : Use multi-threaded serialization." << endl;
	exit( exitCode );
}

QString getValue( const QString& arg, QStringList& args )
{
	if ( arg.startsWith( "--" ) && arg.contains( '=' ) )
	{
		QStringList pair = arg.split( '=' );
		if ( pair.size() == 2 )
		{
			return pair.last();
		}
	}
	else if ( false == args.isEmpty() )
	{
		return args.takeFirst();
	}
	return QString::null;
}

int main(int argc, char* argv[])
{
	QCoreApplication app(argc, argv);

	int port = rec::rpc::defaultPort;
	bool localIPCEnabled = true;
	bool multiThreadedSerialization = false;

	{	// Parse cmd line arguments
		QStringList args = app.arguments();
		if ( args.size() > 1 )
		{
			args.takeFirst();
			while( false == args.isEmpty() )
			{
				QString arg = args.takeFirst();
				if ( arg == "-h" || arg == "--help" )
				{
					showHelp( 0 );
				}
				else if ( arg == "-d" || arg == "--disableLocalIPC" )
				{
					localIPCEnabled = false;
				}
				else if ( arg == "-m" || arg == "--multiThreadedSerialization" )
				{
					multiThreadedSerialization = true;
				}
				else if ( arg == "-p" || arg == "--port" )
				{
					QString v = getValue( arg, args );
					if ( v.isEmpty() )
					{
						Handler::err << "No port value has been specified. Aborting..." << endl;
						showHelp( 1 );
					}
					bool ok = true;
					int p = v.toInt( &ok );
					if ( false == ok )
					{
						Handler::err << "Port value has wrong data format. Aborting..." << endl;
						showHelp( 1 );
					}
					port = p;
				}
				else
				{
					Handler::err << "Unknown command line argument \"" << arg << "\". Aborting..." << endl;
					showHelp( 1 );
				}
			}
		}
	}

	SignalHandler sigHandler;
	Handler handler( port, localIPCEnabled, multiThreadedSerialization );

	return app.exec();
}
