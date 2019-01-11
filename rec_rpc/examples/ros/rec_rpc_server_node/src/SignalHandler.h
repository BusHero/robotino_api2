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

#ifndef _SIGNALHANDLER_H_
#define _SIGNALHANDLER_H_

#include <QtCore>

#ifdef WIN32
#else
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#endif

class SignalHandler : public QThread
{
public:
	SignalHandler()
	{
#ifdef WIN32
#else
		sigemptyset( &set );
		sigaddset( &set, SIGTERM );
		sigaddset( &set, SIGINT );
		sigprocmask( SIG_BLOCK, &set, NULL );

		connect( this, SIGNAL( finished() ), qApp, SLOT( quit() ) );
		start();
#endif
	}

private:
#ifdef WIN32
	void run() {}
#else
	sigset_t set;

	void run()
	{
		qDebug() << "Waiting for SIGTERM or SIGINT";
		int sig_number;
		while( 1 )
		{
			sigwait( &set, &sig_number );
			if( SIGTERM == sig_number || SIGINT == sig_number )
			{
				break;
			}
		}
		qDebug() << "Received SIGTERM or SIGINT";
	}
#endif
};

#endif //_SIGNALHANDLER_H_
