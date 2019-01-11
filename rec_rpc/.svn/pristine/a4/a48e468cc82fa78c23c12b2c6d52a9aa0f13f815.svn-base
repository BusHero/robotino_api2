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
#include <limits>

const QString MainWindow::_texts[20] =
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

MainWindow::MainWindow()
: _address( new QComboBox )
, _port( new QSpinBox )
, _connectPb( new QPushButton )
, _localIPCEnabled( new QCheckBox( "local IPC enabled" ) )
, _blocking( new QCheckBox( "blocking" ) )
, _enableTopics( new QCheckBox( "listen to topics" ) )
, _versionLabel( new QLabel )
, _a( new QDoubleSpinBox )
, _b( new QDoubleSpinBox )
, _c( new QDoubleSpinBox )
, _opCb( new QComboBox )
, _equalLabel( new QLabel( "=" ) )
, _calcPb( new QPushButton( "Calculate" ) )
, _textPb( new QPushButton( "Text" ) )
, _imgPath( new QLineEdit )
, _openImgPb( new QPushButton( "..." ) )
, _imgView( new QLabel )
, _logView( new LogView )
, _imageInfoView( new TopicInfoView )
, _imagePath( QDir::homePath() )
, _textIndex( 0 )
, _startStopPb( new QPushButton( "Start" ) )
{
	setWindowTitle( QString( "RPC client (RPC library %1)" ).arg( rec::rpc::getLibraryVersionString() ) );

	_address->setMinimumWidth( 100 );
	_address->setEditable( true );
	_address->addItem( "127.0.0.1" );

	_port->setRange( 0, 0xFFFF );
	_port->setValue( rec::rpc::defaultPort );

	_a->setRange( std::numeric_limits< int >::min(), std::numeric_limits< int >::max() );
	_b->setRange( std::numeric_limits< int >::min(), std::numeric_limits< int >::max() );
	_c->setRange( std::numeric_limits< int >::min(), std::numeric_limits< int >::max() );
	_c->setReadOnly( true );

	_opCb->addItem( "+" );
	_opCb->addItem( "-" );
	_opCb->addItem( "*" );
	_opCb->addItem( "/" );
	_opCb->setCurrentIndex( 0 );

	_imgPath->setReadOnly( true );

	_logView->setAcceptRichText( false );
	_logView->setReadOnly( true );

	_localIPCEnabled->setChecked( _client.isLocalIPCEnabled() );

	_textTimer.setSingleShot( false );
	_textTimer.setInterval( 10 );

	QVBoxLayout* layout = new QVBoxLayout;
	setLayout( layout );

	QHBoxLayout* connLayout = new QHBoxLayout;
	connLayout->addWidget( new QLabel( "Connect to:" ) );
	connLayout->addWidget( _address, 1 );
	connLayout->addWidget( _port );
	connLayout->addWidget( _localIPCEnabled );
	connLayout->addWidget( _connectPb );
	connLayout->addWidget( _blocking );
	connLayout->addWidget( _enableTopics );

	QHBoxLayout* calcLayout = new QHBoxLayout;
	calcLayout->addWidget( _a );
	calcLayout->addWidget( _opCb );
	calcLayout->addWidget( _b );
	calcLayout->addWidget( _equalLabel );
	calcLayout->addWidget( _c );
	calcLayout->addWidget( _calcPb );
	calcLayout->addStretch();

	QHBoxLayout* imgLayout = new QHBoxLayout;
	imgLayout->addWidget( new QLabel( "Image topic:" ) );
	imgLayout->addWidget( _imgPath, 1 );
	imgLayout->addWidget( _openImgPb );

	QGridLayout* bottomLayout = new QGridLayout;
	bottomLayout->addWidget( new QLabel( "Log:" ), 0, 0 );
	bottomLayout->addWidget( _logView, 1, 0 );
	bottomLayout->addWidget( new QLabel( "Clients listening to topic 'image':" ), 0, 1 );
	bottomLayout->addWidget( _imageInfoView, 1, 1 );

	layout->addLayout( connLayout );
	layout->addWidget( _versionLabel );
	layout->addLayout( calcLayout );
	layout->addWidget( _textPb );
	layout->addWidget( _startStopPb );
	layout->addLayout( imgLayout );
	layout->addWidget( _imgView );
	layout->addLayout( bottomLayout );

	bool ok = true;
	ok &= (bool)connect( &_client, SIGNAL( connected() ), SLOT( on_client_connected() ) );
	ok &= (bool)connect(&_client, SIGNAL(disconnected(rec::rpc::ErrorCode)), SLOT(on_client_disconnected(rec::rpc::ErrorCode)));
	ok &= (bool)connect(&_client, SIGNAL(log(const QString&, int)), _logView, SLOT(log(const QString&)));
	ok &= (bool)connect(&_client, SIGNAL(calcResultReceived(float, rec::rpc::ErrorCode)), SLOT(on_client_calcResultReceived(float, rec::rpc::ErrorCode)));
	ok &= (bool)connect(&_client, SIGNAL(imageReceived(const QImage&)), SLOT(on_client_imageReceived(const QImage&)));
	ok &= (bool)connect(&_client, SIGNAL(imageInfoChanged(const rec::rpc::ClientInfoSet&)), _imageInfoView, SLOT(setInfo(const rec::rpc::ClientInfoSet&)));

	ok &= (bool)connect(_connectPb, SIGNAL(clicked()), SLOT(on_connectPb_clicked()));
	ok &= (bool)connect(_blocking, SIGNAL(toggled(bool)), &_client, SLOT(setBlocking(bool)));
	ok &= (bool)connect(_enableTopics, SIGNAL(toggled(bool)), SLOT(on_enableTopics_toggled(bool)));

	ok &= (bool)connect(_calcPb, SIGNAL(clicked()), SLOT(on_calcPb_clicked()));

	ok &= (bool)connect(_openImgPb, SIGNAL(clicked()), SLOT(on_openImgPb_clicked()));
	ok &= (bool)connect(_textPb, SIGNAL(clicked()), SLOT(on_textPb_clicked()));
	ok &= (bool)connect(_startStopPb, SIGNAL(clicked()), SLOT(on_startStopPb_clicked()));
	ok &= (bool)connect(_localIPCEnabled, SIGNAL(toggled(bool)), &_client, SLOT(setLocalIPCEnabled(bool)));
	ok &= (bool)connect(&_textTimer, SIGNAL(timeout()), SLOT(on_textTimer_timeout()));
	assert( ok );

	updateWidgets( false );
}

MainWindow::~MainWindow()
{
	_client.disconnectFromServer();
}

void MainWindow::updateWidgets( bool connected )
{
	if ( connected )
	{
		_connectPb->setText( "Disconnect" );
	}
	else
	{
		_connectPb->setText( "Connect" );
		_startStopPb->setText( "Start" );
	}
	_address->setDisabled( connected );
	_port->setDisabled( connected );
	_localIPCEnabled->setDisabled( connected );
	_blocking->setEnabled( connected );
	_enableTopics->setEnabled( connected );
	_a->setEnabled( connected );
	_b->setEnabled( connected );
	_c->setEnabled( connected );
	_opCb->setEnabled( connected );
	_equalLabel->setEnabled( connected );
	_calcPb->setEnabled( connected );
	_imgPath->setEnabled( connected );
	_openImgPb->setEnabled( connected );
	_textPb->setEnabled( connected );
	_startStopPb->setEnabled( connected );
}

void MainWindow::on_connectPb_clicked()
{
	if ( _address->findText( _address->currentText() ) < 0 )
		_address->addItem( _address->currentText() );
	if ( _client.isConnected() )
		_client.disconnectFromServer();
	else
	{
		if ( _address->currentText().isEmpty() )
		{
			_logView->log( "Could not connect: address is empty." );
		}
		else
		{
			QString addr = _address->currentText();
			addr.append( ':' );
			addr.append( QString::number( _port->value() ) );
			_client.setAddress( addr.toLatin1().constData() );
			try
			{
				_client.connectToServer( rec::rpc::Client::DefaultTimeout );
			}
			catch( const rec::rpc::Exception& e )
			{
				_logView->log( QString( "Could not connect: " ) + e.getMessage() );
			}
		}
	}
}

void MainWindow::on_enableTopics_toggled( bool enabled )
{
	if ( !enabled )
		_imageInfoView->clear();
	_client.setTopicsEnabled( enabled );
}

void MainWindow::on_client_connected()
{
	_logView->log( QString( "Connected to %1." ).arg( _client.address() ) );
	_client.setBlocking( _blocking->isChecked() );
	if ( _enableTopics->isChecked() )
		_client.setTopicsEnabled( true );
	updateWidgets( true );
	try
	{
		_versionLabel->setText( QString( "Server version: %1" ).arg( _client.getServerVersion() ) );
	}
	catch( const rec::rpc::Exception& e )
	{
		_versionLabel->setText( QString( "Server version could not be retrieved. Reason: " ) + e.getMessage() );
	}
	catch( ... )
	{
		_versionLabel->setText( QString( "Server version could not be retrieved." ) );
	}
}

void MainWindow::on_client_disconnected( rec::rpc::ErrorCode errorCode )
{
	_textTimer.stop();
	_versionLabel->clear();
	_imageInfoView->clear();
	if ( errorCode == rec::rpc::NoError )
	{
		_logView->log( "Disconnected regularly." );
	}
	else
	{
		_logView->log( QString( "Disconnected: " ) + rec::rpc::Exception::messageFromErrorCode( errorCode ) );
	}
	updateWidgets( false );
}

void MainWindow::on_calcPb_clicked()
{
	float c = 0;
	QString op;
	try
	{
		switch( _opCb->currentIndex() )
		{
		case 1:
			op = "sub";
			c = _client.sub( _a->value(), _b->value() );
			break;
		case 2:
			op = "mult";
			c = _client.mult( _a->value(), _b->value() );
			break;
		case 3:
			op = "div";
			c = _client.div( _a->value(), _b->value() );
			break;
		default:
			op = "add";
			c = _client.add( _a->value(), _b->value() );
		}
		_logView->log( QString( "Successfully called \"%1()\" (%2) with result %3." ).arg( op ).arg( _client.blocking() ? "blocking" : "non-blocking" ).arg( c ) );
	}
	catch( const rec::rpc::Exception& e )
	{
		QString errorMsg;
		if ( ErrorCode( e.errorCode() ) == DivisionByZero )
			errorMsg = "Division by zero.";
		else
			errorMsg = e.getMessage();
		QMessageBox::critical( this, "Error", errorMsg );
		_logView->log( QString( "Called \"%1()\" (%2) with error: %3" ).arg( op ).arg( _client.blocking() ? "blocking" : "non-blocking" ).arg( errorMsg ) );
	}
	_c->setValue( c );
}

void MainWindow::on_client_calcResultReceived( float v, rec::rpc::ErrorCode e )
{
	QString errorMsg;
	if ( e != rec::rpc::NoError )
	{
		if ( ErrorCode( e ) == DivisionByZero )
			errorMsg = "Division by zero.";
		else
			errorMsg = rec::rpc::Exception::messageFromErrorCode( e );
		QMessageBox::critical( this, "Error", errorMsg );
	}
	if ( errorMsg.isEmpty() )
		_logView->log( QString( "Result received: %1" ).arg( v ) );
	else
		_logView->log( QString( "Error received: %1" ).arg( errorMsg ) );
	_c->setValue( v );
}

void MainWindow::on_client_imageReceived( const QImage& img )
{
	_imgView->setPixmap( QPixmap::fromImage( img ) );
	_imgView->adjustSize();
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
		_client.publishImage( img );
}

void MainWindow::on_textPb_clicked()
{
	_client.publishText( "a" );
	_client.publishText( "b" );
	_client.publishText( "c" );
	_client.publishText( "d" );
}

void MainWindow::on_startStopPb_clicked()
{
	if ( _textTimer.isActive() )
	{
		_textTimer.stop();
		_startStopPb->setText( "Start" );
	}
	else
	{
		_textTimer.start();
		_startStopPb->setText( "Stop" );
	}
}

void MainWindow::on_textTimer_timeout()
{
	_client.publishText( _texts[_textIndex] );
	++_textIndex;
	_textIndex %= 20;
}
