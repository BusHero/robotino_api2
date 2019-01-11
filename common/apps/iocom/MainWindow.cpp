#include "MainWindow.h"

MainWindow::MainWindow()
	: QDialog( NULL )
	, _logView( new LogView( this ) )
	, _connectButton( new QPushButton( this ) )
	, _deviceSelector( new QComboBox( this ) )
	, _transmitButton( new QPushButton( this ) )
	, _receiveTimer( new QTimer( this ) )
{
	_sensorView = new SensorView( this, &_iocom );
	_actorView = new ActorView( this, &_iocom );

	_deviceSelector->addItem( "USB" );
	for( int i=0; i<100; ++i )
	{
		_deviceSelector->addItem( QString( "COM%1" ).arg( i ) );
	}
	_deviceSelector->setCurrentIndex( 0 );

	_transmitButton->setText( "Transmit" );

	QVBoxLayout* vl = new QVBoxLayout;
	setLayout( vl );

	QHBoxLayout* hlConnect = new QHBoxLayout;
	vl->addLayout( hlConnect );
	hlConnect->addWidget( _connectButton );
	hlConnect->addWidget( _deviceSelector );

	QHBoxLayout* hlButton = new QHBoxLayout;
	vl->addLayout( hlButton );
	hlButton->addWidget( _transmitButton );

	QHBoxLayout* hl = new QHBoxLayout;
	vl->addLayout( hl );
	hl->addWidget( _sensorView );
	hl->addWidget( _actorView );

	vl->addWidget( _logView );

	updateConnectButton();

	bool ok;
	ok = connect( _connectButton, SIGNAL( clicked() ), SLOT( on_connectButton_clicked() ) );
	Q_ASSERT( ok );

	ok = connect( _transmitButton, SIGNAL( clicked() ), SLOT( on_transmitButton_clicked() ) );
	Q_ASSERT( ok );

	ok = connect( &_iocom, SIGNAL( info( const QString& ) ), _logView, SLOT( info( const QString& ) ) );
	Q_ASSERT( ok );

	ok = connect( &_iocom, SIGNAL( warning( const QString& ) ), _logView, SLOT( warning( const QString& ) ) );
	Q_ASSERT( ok );

	ok = connect( &_iocom, SIGNAL( error( const QString& ) ), _logView, SLOT( error( const QString& ) ) );
	Q_ASSERT( ok );

	ok = connect( &_iocom, SIGNAL( log( const QString& ) ), _logView, SLOT( log( const QString& ) ) );
	Q_ASSERT( ok );

	ok = connect( _receiveTimer, SIGNAL( timeout() ), SLOT( on_receiveTimer_timeout() ) );
	Q_ASSERT( ok );

	_receiveTimer->setInterval( 50 );
	_receiveTimer->setSingleShot( false );
	_receiveTimer->start();
}

void MainWindow::on_connectButton_clicked()
{
	if( _iocom.isOpen() )
	{
		_iocom.close();
	}
	else
	{
		if( _deviceSelector->currentText() == "USB" )
		{
			_iocom.open();
		}
		else
		{
			_iocom.open( _deviceSelector->currentText().toLatin1().constData() );
		}
	}

	updateConnectButton();
}

void MainWindow::on_transmitButton_clicked()
{
	_iocom.transmit();
}

void MainWindow::on_receiveTimer_timeout()
{
	_iocom.parse();
}

void MainWindow::updateConnectButton()
{
	if( _iocom.isOpen() )
	{
		_connectButton->setText( "Close connection" );
		_transmitButton->setEnabled( true );
	}
	else
	{
		_connectButton->setText( "Open connection" );
		_transmitButton->setEnabled( false );
	}
}