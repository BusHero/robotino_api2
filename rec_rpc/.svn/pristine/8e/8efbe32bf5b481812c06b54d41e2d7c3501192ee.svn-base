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

#include "ConnectButton.h"
#include "Client.h"

ConnectButton::ConnectButton( QWidget* parent )
: QToolButton( parent )
, _updateTimer( new QTimer( this ) )
, _currentState( -1 )
{
	setObjectName( "ConnectButton" );

	setFixedSize( 40, 40 );
	setIconSize( QSize( 40, 40 ) );

	for( unsigned int i=0; i<3; i++ )
	{
		_connectingPixmaps[i] = QPixmap( QString( ":/images/wifi_connect%1.png" ).arg( i+1 ) );
	}
	_connectingPixmaps[3] = QPixmap( ":/images/wifi_connect3.png" );
	_connectingPixmaps[-1] = QPixmap( ":/images/wifi_disconnect.png" );

	bool success = true;

	success &= connect( Client::singleton(), SIGNAL( is_connected() ), SLOT( on_client_is_connected() ) );
	success &= connect( Client::singleton(), SIGNAL( is_disconnected() ), SLOT( on_client_is_disconnected() ) );

	success &= connect( _updateTimer, SIGNAL( timeout() ), SLOT( on_updateTimer_timeout() ) );

	Q_ASSERT( success );

	_updateTimer->setSingleShot( false );
	_updateTimer->setInterval( 500 );

	setConnectionState( -1 );
}

void ConnectButton::setConnectionState( int state )
{
	if( state > 3 )
	{
		state = 0;
	}

	_currentState = state;
	setIcon( _connectingPixmaps.value( state ) );
	//updateToolTip( state );
}

void ConnectButton::on_clicked()
{
	if( Client::singleton()->isConnected() )
	{
		Client::singleton()->disconnectFromServer();
	}
	else
	{
		_updateTimer->start();

		try
		{
			Client::singleton()->connectToServer();
		}
		catch( const rec::rpc::Exception& /*e*/ )
		{
			on_client_is_disconnected();
			QMessageBox::critical( NULL, QObject::tr("Connect"), QObject::tr("Failed to connect to %1").arg( Client::singleton()->address() ) );
		}
	}
}

void ConnectButton::on_client_is_connected()
{
	_updateTimer->stop();
	setConnectionState( 3 );
}

void ConnectButton::on_client_is_disconnected()
{
	_updateTimer->stop();
	ConnectButton::setConnectionState( -1 );
}

void ConnectButton::on_updateTimer_timeout()
{
	setConnectionState( _currentState + 1 );
}


