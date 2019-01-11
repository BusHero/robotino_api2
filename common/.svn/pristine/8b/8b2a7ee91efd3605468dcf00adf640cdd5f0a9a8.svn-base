#include "MainWindow.h"
//#include "debugger_version.h"
#include <cassert>

#define MajorVer 1
#define MinorVer 0
#define PatchVer 0

MainWindow::MainWindow()
: _logView( new QTextEdit( this ) )
, _connectButton( new QPushButton( this ) )
, _getInfoButton( new QPushButton( this ) )
, _commandInput( new CommandInput( this ) )
, _serialThread( new rec::grappler::serial::SerialThread( this ) )
{
	setWindowTitle( QString( "Robotino%1 Arm Debugger %2.%3.%4" ).arg( QChar( 0xAE ) ).arg( MajorVer ).arg( MinorVer ).arg( PatchVer ) );

	_getInfoTimer.setInterval( 100 );
	_getInfoTimer.setSingleShot( false );

	_logView->setDocument( &_log );
	_connectButton->setText( "Connect" );
	_getInfoButton->setText( "Get info" );

	QVBoxLayout* layout = new QVBoxLayout;
	setLayout( layout );

	QHBoxLayout* pbLayout = new QHBoxLayout;
	pbLayout->addWidget( _connectButton );
	pbLayout->addWidget( _getInfoButton );

	layout->addWidget( _logView );
	layout->addLayout( pbLayout );
	layout->addWidget( _commandInput );

	bool success = true;

	success &= (bool)connect( _connectButton, SIGNAL( clicked() ), SLOT( on_connectButton_clicked() ) );
	assert( success );

	success &= (bool)connect( _getInfoButton, SIGNAL( clicked() ), SLOT( on_getInfoButton_clicked() ) );
	assert( success );

	success &= (bool)connect( &_getInfoTimer, SIGNAL( timeout() ), SLOT( on_getInfoTimer_timeout() ) );
	assert( success );

	success &= (bool)connect( _commandInput, SIGNAL( sendCommand( const QByteArray& /*data*/ ) ), SLOT( on_commandInput_sendCommand( const QByteArray& /*data*/ ) ) );
	assert( success );

	success &= (bool)connect( _commandInput, SIGNAL( error( const QString& /*message*/ ) ), SLOT( on_commandInput_error( const QString& /*message*/ ) ) );
	assert( success );

	success &= (bool)connect( _commandInput, SIGNAL( getDetectedServos() ), SLOT( on_commandInput_getDetectedServos() ) );
	assert( success );

	success &= (bool)connect( _commandInput, SIGNAL( getAllPositions() ), SLOT( on_commandInput_getAllPositions() ) );
	assert( success );

	success &= (bool)connect( _commandInput, SIGNAL( sendAllPositions( const QByteArray& /*message*/ ) ), SLOT( on_commandInput_sendAllPositions( const QByteArray& /*message*/ ) ) );
	assert( success );

	success &= (bool)connect( _commandInput, SIGNAL( enablePower( int /*channel*/, bool /*enable*/ ) ), SLOT( on_commandInput_enablePower( int /*channel*/, bool /*enable*/ ) ) );
	assert( success );

	success &= (bool)connect( _commandInput, SIGNAL( toggleTorque() ), SLOT( on_commandInput_toggleTorque() ) );
	assert( success );

	success &= (bool)connect( _commandInput, SIGNAL( enableAutoUpdate( bool /*enable*/ ) ), SLOT( on_commandInput_enableAutoUpdate( bool /*enable*/ ) ) );
	assert( success );

	success &= (bool)connect( _commandInput, SIGNAL( resetDevice( bool /*enterUSBBootloader*/ ) ), SLOT( on_commandInput_resetDevice( bool /*enterUSBBootloader*/ ) ) );
	assert( success );


	success &= (bool)connect( _serialThread,
		SIGNAL( serialPortError( const QString& /*error*/ ) ),
		SLOT( on_serialThread_serialPortError( const QString& /*error*/ ) ) );
	assert( success );

	success &= (bool)connect( _serialThread,
		SIGNAL( statusReceived( int /*id*/, int /*error*/, const QByteArray& /*data*/ ) ),
		SLOT( on_serialThread_statusReceived( int /*id*/, int /*error*/, const QByteArray& /*data*/ ) ) );
	assert( success );

	success &= (bool)connect( _serialThread,
		SIGNAL( servosDetected( const rec::grappler::HardwareInfo& /*info*/ ) ),
		SLOT( on_serialThread_servosDetected( const rec::grappler::HardwareInfo& /*info*/ ) ) );
	assert( success );

	success &= (bool)connect( _serialThread,
		SIGNAL( allServoPositions( const rec::grappler::HardwareInfo& /*info*/ ) ),
		SLOT( on_serialThread_allServoPositions( const rec::grappler::HardwareInfo& /*info*/ ) ) );
	assert( success );

	success &= (bool)connect( _serialThread,
		SIGNAL( storePosition( const rec::grappler::HardwareInfo& /*info*/ ) ),
		SLOT( on_serialThread_storePosition( const rec::grappler::HardwareInfo& /*info*/ ) ) );
	assert( success );

	success &= (bool)connect( _serialThread,
		SIGNAL( allAxesLimits( const rec::grappler::HardwareInfo& /*info*/ ) ),
		SLOT( on_serialThread_allAxesLimits( const rec::grappler::HardwareInfo& /*info*/ ) ) );

	success &= (bool)connect( _serialThread,
		SIGNAL( infoReceived( int, int, int ) ), SLOT( on_serialThread_infoReceived( int, int, int ) ) );
		
	success &= (bool)connect( _serialThread,
		SIGNAL( communicationReady() ),
		SLOT( on_serialThread_communicationReady() ) );

	assert( success );
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_connectButton_clicked()
{
	if( _serialThread->isOpen() )
	{
		_getInfoTimer.stop();
		_serialThread->close();
		_connectButton->setText( "Connect" );
		return;
	}

	if( _serialThread->open() )
	{
		log( QString( "Connected to %1" ).arg( _serialThread->port() ) );
		_connectButton->setText( "Disconnect" );
	}
	else
	{
		log( QString( "No Robotino Arm found" ) );
	}
}

void MainWindow::on_getInfoButton_clicked()
{
	if ( !_serialThread->isOpen() )
		log( "Unable to retrieve info (no connection)." );
	else
		_getInfoTimer.start();
}

void MainWindow::on_getInfoTimer_timeout()
{
	_serialThread->sendGetInfo();
}

void MainWindow::on_commandInput_sendCommand( const QByteArray& data )
{
	QString message;
		
	for( int i=0; i<data.size(); ++i )
	{
		unsigned int num = (unsigned char)data[i];
		message += QString("%1 ").arg( num, 2, 16, QChar( '0' ) );
	}

	//log( QString("Sending %1").arg( message ) );

	_serialThread->sendDynamixelCommand( data );
}

void MainWindow::on_commandInput_error( const QString& message )
{
	log( message );
}

void MainWindow::on_commandInput_getDetectedServos()
{
	_serialThread->sendGetDetectedServos();
}

void MainWindow::on_commandInput_getAllPositions()
{
	_serialThread->sendGetAllPositions();
}

void MainWindow::on_commandInput_sendAllPositions( const QByteArray& message )
{
	_serialThread->sendAllPositions( message );
}

void MainWindow::on_commandInput_enableAutoUpdate( bool enable )
{
	_serialThread->setAutoUpdateEnabled( enable );
}

void MainWindow::on_commandInput_enablePower( int channel, bool enable )
{
	_serialThread->sendEnablePower( channel, enable );
}

void MainWindow::on_commandInput_toggleTorque()
{
	_serialThread->sendToggleTorque();
}

void MainWindow::on_commandInput_resetDevice( bool enterUSBBootloader )
{
	_serialThread->sendResetDevice( enterUSBBootloader );
}

void MainWindow::on_serialThread_serialPortError( const QString& error )
{
	qDebug() << "on_serialThread_serialPortError: " << error;
	log( error );
	if ( !_serialThread->isOpen() )
		_connectButton->setText( "Connect" );
}

void MainWindow::on_serialThread_statusReceived( int id, int error, const QByteArray& data )
{
	qDebug() << "on_serialThread_statusReceived";

	QString message = QString( "Status - id %1 error %2 data " ).arg( id ).arg( error );
		
	for( int i=0; i<data.size(); ++i )
	{
		unsigned int num = (unsigned char)data[i];
		message += QString("%1 ").arg( num, 2, 16, QChar( '0' ) );
	}

	log( message ) ;
}

void MainWindow::on_serialThread_servosDetected( const rec::grappler::HardwareInfo& info )
{
	qDebug() << "on_serialThread_servosDetected";

	_commandInput->setServosDetected( info );
	_serialThread->sendGetAllAxesLimits();
}

void MainWindow::on_serialThread_allServoPositions( const rec::grappler::HardwareInfo& info )
{
	static rec::grappler::HardwareInfo infoHist;

	if (info != infoHist)
	{
		infoHist = info;

		qDebug() << "on_serialThread_allServoPositions";
		_commandInput->setAllServoPositions(info);
	}
}

void MainWindow::on_serialThread_storePosition( const rec::grappler::HardwareInfo& info )
{
	qDebug() << "on_serialThread_storePosition";
	_commandInput->storePosition(info);
}

void MainWindow::on_serialThread_allAxesLimits( const rec::grappler::HardwareInfo& info )
{
	qDebug() << "on_serialThread_allAxesLimits";
	_commandInput->setAxesLimits(info);
}

void MainWindow::on_serialThread_infoReceived( int major, int minor, int patch )
{
	qDebug() << "on_serialThread_infoReceived";
	_getInfoTimer.stop();
	log( QString( "Robotino arm firmware version %1.%2.%3" ).arg( major ).arg( minor ).arg( patch ) );
}

void MainWindow::on_serialThread_communicationReady()
{
	_serialThread->setAutoUpdateEnabled( true );
}

void MainWindow::log( const QString& message )
{
	QTextCursor cursor( &_log );
	cursor.movePosition( QTextCursor::End );
	cursor.insertBlock();
	cursor.insertText( message );

	_logView->ensureCursorVisible();
}
