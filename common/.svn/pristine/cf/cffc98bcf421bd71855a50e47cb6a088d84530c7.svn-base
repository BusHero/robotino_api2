#include "MainWindow.h"
#include <cassert>

#define MajorVer 1
#define MinorVer 0
#define PatchVer 0

MainWindow::MainWindow()
: _logView( new QTextEdit( this ) )
, _connectButton( new QPushButton( this ) )
, _getInfoButton( new QPushButton( this ) )
, _resetButton( new QPushButton( this ) )
, _enterBootloader( new QCheckBox( this ) )
, _sendSetPressuresButton( new QPushButton( "Send", this ) )
, _compressorsCb( new QCheckBox( "Compressors enabled", this ) )
, _waterDrainValveCb( new QCheckBox( "Water drain valve", this ) )
, _gripperValve1Cb( new QCheckBox( "Gripper valve 1", this ) )
, _gripperValve2Cb( new QCheckBox( "Gripper valve 2", this ) )
, _serial( new rec::robotinoxt::serial::SerialThread( this ) )
, _maxPressure( 1500 )
{
	setWindowTitle( QString( "Robotino%1 XT Debugger %2.%3.%4" ).arg( QChar( 0xAE ) ).arg( MajorVer ).arg( MinorVer ).arg( PatchVer ) );

	_getInfoTimer.setInterval( 100 );
	_getInfoTimer.setSingleShot( false );

	_logView->setDocument( &_log );
	_connectButton->setText( "Connect" );
	_getInfoButton->setText( "Get info" );
	_resetButton->setText( "Reset device" );
	_enterBootloader->setText( "Enter bootloader" );

	QVBoxLayout* layout = new QVBoxLayout;
	setLayout( layout );

	QHBoxLayout* txBytesLayout = new QHBoxLayout;
	txBytesLayout->setSpacing( 0 );
	txBytesLayout->setMargin( 0 );

	for( int i = 0; i < 8; ++i )
	{
		_pressures[i] = new QDoubleSpinBox( this );
		_pressures[i]->setRange( 0, 1.5 );
		_pressures[i]->setSingleStep( 0.1 );
		txBytesLayout->addWidget( _pressures[i] );
	}

	QHBoxLayout* txLayout = new QHBoxLayout;
	txLayout->addLayout( txBytesLayout );
	txLayout->addWidget( _sendSetPressuresButton );
	txLayout->addStretch();

	QGroupBox* txGroup = new QGroupBox( "Pressures" );
	txGroup->setLayout( txLayout );

	QGridLayout* doutLayout = new QGridLayout;

	doutLayout->addWidget( _compressorsCb, 0, 0 );
	doutLayout->addWidget( _waterDrainValveCb, 1, 0 );
	doutLayout->addWidget( _gripperValve1Cb, 0, 1 );
	doutLayout->addWidget( _gripperValve2Cb, 1, 1 );

	QGroupBox* doutGroup = new QGroupBox( "Outputs" );
	doutGroup->setLayout( doutLayout );

	QHBoxLayout* resetLayout = new QHBoxLayout;
	resetLayout->addWidget( _resetButton );
	resetLayout->addWidget( _enterBootloader );

	QHBoxLayout* pbLayout = new QHBoxLayout;
	pbLayout->addWidget( _connectButton );
	pbLayout->addWidget( _getInfoButton );

	layout->addWidget( _logView );
	layout->addWidget( txGroup );
	layout->addWidget( doutGroup );
	layout->addLayout( resetLayout );
	layout->addLayout( pbLayout );

	bool success = true;
	success &= (bool)connect( _connectButton, SIGNAL( clicked() ), SLOT( on_connectButton_clicked() ) );
	success &= (bool)connect(_getInfoButton, SIGNAL(clicked()), SLOT(on_getInfoButton_clicked()));
	success &= (bool)connect(_resetButton, SIGNAL(clicked()), SLOT(on_resetButton_clicked()));
	success &= (bool)connect(_sendSetPressuresButton, SIGNAL(clicked()), SLOT(on_sendSetPressuresButton_clicked()));
	success &= (bool)connect(_compressorsCb, SIGNAL(toggled(bool)), SLOT(on_compressorsCb_toggled(bool)));
	success &= (bool)connect(_waterDrainValveCb, SIGNAL(toggled(bool)), SLOT(on_waterDrainValveCb_toggled(bool)));
	success &= (bool)connect(_gripperValve1Cb, SIGNAL(toggled(bool)), SLOT(on_gripperValve1Cb_toggled(bool)));
	success &= (bool)connect(_gripperValve2Cb, SIGNAL(toggled(bool)), SLOT(on_gripperValve2Cb_toggled(bool)));
	success &= (bool)connect(&_getInfoTimer, SIGNAL(timeout()), SLOT(on_getInfoTimer_timeout()));
	success &= (bool)connect(_serial, SIGNAL(serialPortError(const QString&)), SLOT(on_serial_serialPortError(const QString&)));
	success &= (bool)connect(_serial
		, SIGNAL( statusReceived( const QVector< qint16 >&, bool, const QVector< float >& ) )
		, SLOT( on_serial_statusReceived( const QVector< qint16 >&, bool, const QVector< float >& ) ) );
	success &= (bool)connect(_serial, SIGNAL(infoReceived(int, int, int)), SLOT(on_serial_infoReceived(int, int, int)));
	Q_ASSERT( success );
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_connectButton_clicked()
{
	if( _serial->isOpen() )
	{
		_getInfoTimer.stop();
		_serial->close();
		_connectButton->setText( "Connect" );
		return;
	}

	if( _serial->open() )
	{
		_statusTime.start();
		log( QString( "Connected to %1" ).arg( _serial->port() ) );
		_connectButton->setText( "Disconnect" );
	}
	else
	{
		log( QString( "No Robotino XT found" ) );
	}
}

void MainWindow::on_getInfoButton_clicked()
{
	if ( !_serial->isOpen() )
		log( "Unable to retrieve info (no connection)." );
	else
		_getInfoTimer.start();
}

void MainWindow::on_getInfoTimer_timeout()
{
	_serial->sendGetInfo();
}

void MainWindow::on_resetButton_clicked()
{
	_serial->sendResetDevice( _enterBootloader->isChecked() );
}

void MainWindow::on_sendSetPressuresButton_clicked()
{
	QVector< qint16 > pressures( 8 );
	for( int i = 0; i < 8; ++i )
	{
		int p = static_cast<int>( _pressures[i]->value() * 1000 );
		if( p > _maxPressure )
		{
			p = _maxPressure;
		}
		else if( p < 0 )
		{
			p = 0;
		}

		pressures[i] = p;
	}
	_serial->sendSetPressures( pressures );
}

void MainWindow::on_compressorsCb_toggled( bool checked )
{
	_serial->sendSetCompressorsEnabled( checked );
}

void MainWindow::on_waterDrainValveCb_toggled( bool checked )
{
	_serial->sendSetDOut( 3, checked );
}

void MainWindow::on_gripperValve1Cb_toggled( bool checked )
{
	_serial->sendSetDOut( 4, checked );
}

void MainWindow::on_gripperValve2Cb_toggled( bool checked )
{
	_serial->sendSetDOut( 5, checked );
}

void MainWindow::on_serial_serialPortError( const QString& error )
{
	log( error );
	if ( !_serial->isOpen() )
		_connectButton->setText( "Connect" );
}

void MainWindow::on_serial_infoReceived( int major, int minor, int patch )
{
	_getInfoTimer.stop();
	log( QString( "Robotino XT firmware version %1.%2.%3" ).arg( major ).arg( minor ).arg( patch ) );
}

void MainWindow::on_serial_statusReceived( const QVector< qint16 >& pressures, bool pressureSensor, const QVector< float >& potis )
{
	if ( pressures.isEmpty() )
	{
		log( "Error getting pressure values!" );
		return;
	}

	int time = _statusTime.restart();

	QString logMsg( "%1 Received status. Pressures:" );
	Q_FOREACH( qint16 p, pressures )
	{
		logMsg.append( ' ' + QString::number( 0.001f * float( p ) ) );
	}
	logMsg.append( " Pressure sensor: " + QString::number( int( pressureSensor ) ) );
	logMsg.append( " Poti values:" );
	Q_FOREACH( float v, potis )
	{
		logMsg.append( ' ' + QString::number( v ) );
	}
	log( logMsg.arg( time ) );
}

void MainWindow::log( const QString& message )
{
	QTextCursor cursor( &_log );
	cursor.movePosition( QTextCursor::End );
	cursor.insertBlock();
	cursor.insertText( message );

	_logView->ensureCursorVisible();
}
