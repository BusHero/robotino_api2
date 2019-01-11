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

#include "MainWindow.h"

#include <cassert>

MainWindow::MainWindow()
: _startStopPb( new QPushButton )
, _port( new QSpinBox )
, _localIPCEnabled( new QCheckBox( "local IPC enabled" ) )
, _enableTopics( new QCheckBox( "listen to topics" ) )
, _handleHttpGetRequests( new QCheckBox( "handle HTTP GET requests" ) )
, _handleCustomRequests( new QCheckBox( "handle custom requests" ) )
, _imgPath( new QLineEdit )
, _openImgPb( new QPushButton( "..." ) )
, _textPb( new QPushButton( "Text" ) )
, _imgView( new QLabel )
, _logView( new LogView )
, _imageInfoView( new TopicInfoView )
, _numClientsLabel( new QLabel( "0" ) )
, _imagePath(QDir::homePath() )
{
	setWindowTitle( QString( "RPC server (RPC library %1)" ).arg( rec::rpc::getLibraryVersionString() ) );

	_port->setRange( 0, 0xFFFF );
	_port->setValue( _server.port() );

	_imgPath->setReadOnly( true );

	_logView->setAcceptRichText( false );
	_logView->setReadOnly( true );

	_localIPCEnabled->setChecked( _server.isLocalIPCEnabled() );

	QVBoxLayout* layout = new QVBoxLayout;
	setLayout( layout );

	QHBoxLayout* mgmLayout = new QHBoxLayout;
	mgmLayout->addWidget( _startStopPb );
	mgmLayout->addWidget( new QLabel( "Port:" ) );
	mgmLayout->addWidget( _port );
	mgmLayout->addWidget( _localIPCEnabled );
	mgmLayout->addWidget( _enableTopics );
	mgmLayout->addWidget( _handleHttpGetRequests );
	mgmLayout->addWidget( _handleCustomRequests );
	mgmLayout->addStretch();

	QHBoxLayout* imgLayout = new QHBoxLayout;
	imgLayout->addWidget( new QLabel( "Image topic:" ) );
	imgLayout->addWidget( _imgPath, 1 );
	imgLayout->addWidget( _openImgPb );

	QGridLayout* bottomLayout = new QGridLayout;
	bottomLayout->addWidget( new QLabel( "Log:" ), 0, 0 );
	bottomLayout->addWidget( _logView, 1, 0 );
	bottomLayout->addWidget( new QLabel( "Clients listening to topic 'image':" ), 0, 1 );
	bottomLayout->addWidget( _imageInfoView, 1, 1 );

	QHBoxLayout* statusLayout = new QHBoxLayout;
	statusLayout->addWidget( new QLabel( "Connected clients:" ) );
	statusLayout->addWidget( _numClientsLabel );
	statusLayout->addStretch();

	layout->addLayout( mgmLayout);
	layout->addWidget( _textPb);
	layout->addLayout( imgLayout );
	layout->addWidget( _imgView );
	layout->addLayout( bottomLayout );
	layout->addLayout( statusLayout );

	bool ok = true;
	ok &= (bool)connect( &_server, SIGNAL( listening() ), SLOT( on_server_listening() ) );
	ok &= (bool)connect( &_server, SIGNAL( closed() ), SLOT( on_server_closed() ) );
	ok &= (bool)connect( &_server, SIGNAL( clientConnected( const rec::rpc::ClientInfo& ) ), SLOT( on_server_clientConnected( const rec::rpc::ClientInfo& ) ) );
	ok &= (bool)connect( &_server, SIGNAL( clientDisconnected( const rec::rpc::ClientInfo& ) ), SLOT( on_server_clientDisconnected( const rec::rpc::ClientInfo& ) ) );
	ok &= (bool)connect( &_server, SIGNAL( numClientsConnectedChanged( int ) ), SLOT( on_server_numClientsConnectedChanged( int ) ) );
	ok &= (bool)connect( &_server, SIGNAL( registeredTopicListener( const QString&, const rec::rpc::ClientInfo& ) ), SLOT( on_server_registeredTopicListener( const QString&, const rec::rpc::ClientInfo& ) ) );
	ok &= (bool)connect( &_server, SIGNAL( unregisteredTopicListener( const QString&, const rec::rpc::ClientInfo& ) ), SLOT( on_server_unregisteredTopicListener( const QString&, const rec::rpc::ClientInfo& ) ) );
	ok &= (bool)connect( &_server, SIGNAL( imageReceived( const QImage& ) ), SLOT( on_server_imageReceived( const QImage& ) ) );
	ok &= (bool)connect( &_server, SIGNAL( imageInfoChanged( const rec::rpc::ClientInfoSet& ) ), _imageInfoView, SLOT( setInfo( const rec::rpc::ClientInfoSet& ) ) );
	ok &= (bool)connect( &_server, SIGNAL( log( const QString&, int ) ), _logView, SLOT( log( const QString& ) ) );

	ok &= (bool)connect( _startStopPb, SIGNAL( clicked() ), SLOT( on_startStopPb_clicked() ) );
	ok &= (bool)connect( _enableTopics, SIGNAL( toggled( bool ) ), SLOT( on_enableTopics_toggled( bool ) ) );
	ok &= (bool)connect( _handleHttpGetRequests, SIGNAL( toggled( bool ) ), &_server, SLOT( setHandleHttpGet( bool ) ) );
	ok &= (bool)connect( _handleCustomRequests, SIGNAL( toggled( bool ) ), &_server, SLOT( setHandleCustomRequests( bool ) ) );
	ok &= (bool)connect( _localIPCEnabled, SIGNAL( toggled( bool ) ), &_server, SLOT( setLocalIPCEnabled( bool ) ) );
	ok &= (bool)connect( _port, SIGNAL( valueChanged( int ) ), &_server, SLOT( setPort( int ) ) );

	ok &= (bool)connect( _openImgPb, SIGNAL( clicked() ), SLOT( on_openImgPb_clicked() ) );
	ok &= (bool)connect( _textPb, SIGNAL( clicked() ), SLOT( on_textPb_clicked() ) );
	assert( ok );

	updateWidgets( false );
}

MainWindow::~MainWindow()
{
	_server.exit();
}

void MainWindow::updateWidgets( bool listening )
{
	_startStopPb->setEnabled( true );
	if ( listening )
		_startStopPb->setText( "Stop server" );
	else
		_startStopPb->setText( "Start server" );
	_localIPCEnabled->setDisabled( listening );
	_textPb->setEnabled( listening );
	_openImgPb->setEnabled( listening );
	_imgPath->setEnabled( listening );
}

void MainWindow::on_server_listening()
{
	_logView->log( QString( "Server is listening on port %1." ).arg( _server.serverPort() ) );
	updateWidgets( true );
}

void MainWindow::on_server_closed()
{
	_logView->log( QString( "Server is closed." ) );
	updateWidgets( false );
}

void MainWindow::on_server_clientConnected( const rec::rpc::ClientInfo& info )
{
	logClientConnectionStateChange( info, true );
}

void MainWindow::on_server_clientDisconnected( const rec::rpc::ClientInfo& info )
{
	logClientConnectionStateChange( info, false );
}

void MainWindow::on_server_registeredTopicListener( const QString& name, const rec::rpc::ClientInfo& info )
{
	if ( info.isLocal() )
	{
		_logView->log( QString( "Local client %1 registered topic '%2'." ).arg( info.port ).arg( name ) );
	}
	else if ( info.port == 0 )
	{
		_logView->log( QString( "Server registered topic '%1'." ).arg( name ) );
	}
	else
	{
		_logView->log( QString( "Remote client %1:%2 registered topic '%3'." ).arg( info.address.toString() ).arg( info.port ).arg( name ) );
	}
}

void MainWindow::on_server_unregisteredTopicListener( const QString& name, const rec::rpc::ClientInfo& info )
{
	if ( info.isLocal() )
	{
		_logView->log( QString( "Local client %1 unregistered topic '%2'." ).arg( info.port ).arg( name ) );
	}
	else if ( info.port == 0 )
	{
		_logView->log( QString( "Server unregistered topic '%1'." ).arg( name ) );
	}
	else
	{
		_logView->log( QString( "Remote client %1:%2 unregistered topic '%3'." ).arg( info.address.toString() ).arg( info.port ).arg( name ) );
	}
}

void MainWindow::on_server_imageReceived( const QImage& img )
{
	_imgView->setPixmap( QPixmap::fromImage( img ) );
	_imgView->adjustSize();
}

void MainWindow::on_startStopPb_clicked()
{
	_startStopPb->setEnabled( false );
	if ( _server.isListening() )
		_server.close();
	else
		_server.listen();
}

void MainWindow::on_enableTopics_toggled( bool enabled )
{
	if ( !enabled )
		_imageInfoView->clear();
	_server.setTopicsEnabled( enabled );
}

void MainWindow::on_openImgPb_clicked()
{
	QString formatString = "Image (";
	Q_FOREACH( const QByteArray& format, QImageReader::supportedImageFormats() )
	{
		formatString.append( "*." );
		formatString.append( format );
		formatString.append( ' ' );
	}
	formatString[ formatString.length() - 1 ] = ')';
	QString filePath = QFileDialog::getOpenFileName( this, "Open image", _imagePath, formatString );
	_imgPath->setText( filePath );
	if ( filePath.isEmpty() )
		return;
	_imagePath = QFileInfo( filePath ).absolutePath();
	QImage img( filePath );
	if ( !img.isNull() )
		_server.publishImage( img );
}

void MainWindow::logClientConnectionStateChange( const rec::rpc::ClientInfo& info, bool connected )
{
	static QString logMsg( "%1 client %2 %3." );

	QString clientDesc = QString::number( info.port );
	if ( !info.isLocal() )
	{
		clientDesc.prepend( info.address.toString() + ":" );
	}
	if ( !info.name.isEmpty() )
	{
		clientDesc.prepend( "\" " + info.name + "\" (" );
		clientDesc.append( ')' );
	}
	_logView->log( logMsg.arg( info.isLocal() ? "Local" : "Remote" ).arg( clientDesc ).arg( connected ? "connected" : "disconnected" ) );
}

void MainWindow::on_server_numClientsConnectedChanged( int num )
{
	_numClientsLabel->setText( QString::number( num ) );
}

void MainWindow::on_textPb_clicked()
{
	_server.publishText( "e" );
	_server.publishText( "f" );
	_server.publishText( "g" );
	_server.publishText( "h" );
}