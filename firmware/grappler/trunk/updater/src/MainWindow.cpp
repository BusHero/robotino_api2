#include "MainWindow.h"
#include "DeviceInfo.h"

#include "../../firmware/src/version.h"
#include "../../usb_bootloader/src/usbcfg.h"

#include <cassert>

MainWindow::MainWindow()
: _logView( new QTextEdit( this ) )
, _currentVersionLabel( new QLabel( "Current firmware version:", this ) )
, _updateLabel( new QLabel( this ) )
, _startUpdatePb( new QPushButton( "Upload firmware" ) )
, _serialThread( new rec::grappler::serial::SerialThread( this ) )
, _bootloaderWaitCounter( 0 )
, _updating( false )
, _manualUpdate( false )
{
	setWindowTitle( QString( "Robotino%1 Grappler firmware %2.%3.%4 update" ).arg( QChar( 0xAE ) ).arg( MAJOR ).arg( MINOR ).arg( PATCH ) );

	_connectTimer.setSingleShot( false );
	_connectTimer.setInterval( 1000 );
	_getInfoTimer.setSingleShot( false );
	_getInfoTimer.setInterval( 200 );
	_bootloaderWaitTimer.setSingleShot( false );
	_bootloaderWaitTimer.setInterval( 1000 );
	_passiveBootloaderWaitTimer.setSingleShot( false );
	_passiveBootloaderWaitTimer.setInterval( 2000 );

	_logView->setDocument( &_log );

	_startUpdatePb->setEnabled( false );

	QVBoxLayout* lLayout = new QVBoxLayout;
	lLayout->addWidget( new QLabel( QString( "New firmware version: %1.%2.%3 " ).arg( MAJOR ).arg( MINOR ).arg( PATCH ) ) );
	lLayout->addWidget( _currentVersionLabel );
	lLayout->addWidget( _updateLabel );

	QVBoxLayout* rLayout = new QVBoxLayout;
	rLayout->addWidget( _startUpdatePb, 0, Qt::AlignVCenter );

	QHBoxLayout* vLayout = new QHBoxLayout;
	vLayout->addLayout( lLayout );
	vLayout->addStretch();
	vLayout->addLayout( rLayout );

	QVBoxLayout* layout = new QVBoxLayout;
	setLayout( layout );

	layout->addLayout( vLayout );
	layout->addWidget( _logView );

	bool success = true;

	success &= connect( &_connectTimer, SIGNAL( timeout() ), SLOT( on_connectTimer_timeout() ) );
	success &= connect( &_getInfoTimer, SIGNAL( timeout() ), SLOT( on_getInfoTimer_timeout() ) );
	success &= connect( &_bootloaderWaitTimer, SIGNAL( timeout() ), SLOT( on_bootloaderWaitTimer_timeout() ) );
	success &= connect( &_passiveBootloaderWaitTimer, SIGNAL( timeout() ), SLOT( on_passiveBootloaderWaitTimer_timeout() ) );
	success &= connect( _startUpdatePb, SIGNAL( clicked() ), SLOT( on_startUpdatePb_clicked() ) );
	success &= connect( _serialThread, SIGNAL( serialPortError( const QString& ) ), SLOT( on_serialThread_serialPortError( const QString& ) ) );
	success &= connect( _serialThread,
		SIGNAL( infoReceived( int, int, int ) ), SLOT( on_serialThread_infoReceived( int, int, int ) ) );

	assert( success );

	_connectTimer.start();
	_passiveBootloaderWaitTimer.start();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setUpdating( bool enable )
{
	_updating = enable;

	HMENU systemMenu = GetSystemMenu( static_cast< HWND >( internalWinId() ), FALSE );
	if ( enable )
		EnableMenuItem( systemMenu, SC_CLOSE, MF_BYCOMMAND | MF_GRAYED );
	else
		EnableMenuItem( systemMenu, SC_CLOSE, MF_BYCOMMAND | MF_ENABLED );
}

void MainWindow::closeEvent( QCloseEvent* ev )
{
	if ( _updating )
	{
		ev->ignore();
		return;
	}
	if ( _serialThread->isOpen() )
		_serialThread->close();
	ev->accept();
}

void MainWindow::on_connectTimer_timeout()
{
	if( _serialThread->open() )
	{
		log( QString( "Connected to %1. Click \"Upload firmware\" to start the update." ).arg( _serialThread->port() ) );
		_connectTimer.stop();
		_serialThread->sendGetDetectedServos();
		_getInfoTimer.start();
	}
	else
	{
		log( QString( "No Grappler found" ) );
	}
}

void MainWindow::on_startUpdatePb_clicked()
{
	_startUpdatePb->setEnabled( false );
	setUpdating( true );
	if ( _manualUpdate )
	{
		_serialThread->close();
		uploadFirmware();
		uploadFinished();
		return;
	}
	else
	{
		log( "Resetting device and waiting for the bootloader to come up." );
		_bootloaderWaitCounter = 0;
		_serialThread->sendResetDevice( true );
		_serialThread->close();
		_bootloaderWaitTimer.start();
	}
}

void MainWindow::on_getInfoTimer_timeout()
{
	if ( _serialThread->isOpen() )
		_serialThread->sendGetInfo();
	else
		_getInfoTimer.stop();
}

void MainWindow::on_serialThread_serialPortError( const QString& error )
{
	if ( error == "Keep alive timeout" )
	{
		log( error );
		_startUpdatePb->setEnabled( false );
		_serialThread->close();
		if ( false == _updating )
		{
			_connectTimer.start();
			_passiveBootloaderWaitTimer.start();
		}
	}
}

void MainWindow::on_serialThread_infoReceived( int major, int minor, int patch )
{
	_getInfoTimer.stop();
	_passiveBootloaderWaitTimer.stop();
	_currentVersionLabel->setText( QString( "Current firmware version: %1.%2.%3" ).arg( major ).arg( minor ).arg( patch ) );
	unsigned int thisV = ( ( major & 0xff ) << 16 ) + ( ( minor & 0xff ) << 8 ) + ( patch & 0xff );
	unsigned int newV = ( ( MAJOR & 0xff ) << 16 ) + ( ( MINOR & 0xff ) << 8 ) + ( PATCH & 0xff );
	_manualUpdate = false;
	_startUpdatePb->setEnabled( true );
	if ( newV > thisV )
	{
		_updateLabel->setText( "Update is recommended." );
	}
	else
	{
		_updateLabel->setText( "No update is required." );
	}
}

void MainWindow::log( const QString& message )
{
	QTextCursor cursor( &_log );
	cursor.movePosition( QTextCursor::End );
	cursor.insertBlock();
	cursor.insertText( message );

	_logView->ensureCursorVisible();
}

void MainWindow::on_bootloaderWaitTimer_timeout()
{
	++_bootloaderWaitCounter;
	QChar l = DeviceInfo::getDriveLetterFromHardwareId( VENDOR_ID, PRODUCT_ID );
	if ( l.isNull() )
	{
		log( "Bootloader is not ready." );
		if ( _bootloaderWaitCounter < 60 )
			return;
		log( "Update failed." );
		_bootloaderWaitTimer.stop();
	}
	else
	{
		_bootloaderWaitTimer.stop();
		_driveLetter = l;
		uploadFirmware();
	}
	_bootloaderWaitCounter = 0;
	uploadFinished();
}

void MainWindow::on_passiveBootloaderWaitTimer_timeout()
{
	QChar l = DeviceInfo::getDriveLetterFromHardwareId( VENDOR_ID, PRODUCT_ID );
	if ( l.isNull() )
	{
		_manualUpdate = false;
		_startUpdatePb->setEnabled( false );
		_connectTimer.start();
	}
	else
	{
		_manualUpdate = true;
		_connectTimer.stop();
		_startUpdatePb->setEnabled( true );
		if ( l != _driveLetter )
			log( "Bootloader is ready for manual update. Click \"Upload firmware\" to proceed." );
	}
	_driveLetter = l;
}

void MainWindow::uploadFirmware()
{
	log( QString( "Using drive %1:" ).arg( _driveLetter ) );
	qApp->processEvents();
	QDir dir( QString( "%1:/" ).arg( _driveLetter ) );
	QString filename = QString( "%1:/firmware.bin" ).arg( _driveLetter );
	if ( dir.exists( "firmware.bin" ) )
	{
		log( "Found firmware.bin. Deleting it." );
		qApp->processEvents();
		if ( QFile::remove( filename ) )
		{
			log( "Copying new firmware.bin to the device." );
			qApp->processEvents();
			QFile firmware_bin( ":/firmware.bin" );
			if ( !firmware_bin.copy( filename ) )
			{
				log( "firmware.bin could not be copied to the device. Cancelling." );
			}
		}
		else
		{
			log( "Existing firmware.bin could not be deleted. Cancelling." );
		}
	}
	else
	{
		log( "firmware.bin not found. Cancelling." );
	}
	log( "Dismounting and ejecting volume." );
	DeviceInfo::ejectDrive( _driveLetter );
}

void MainWindow::uploadFinished()
{
	log( "Waiting for the serial interface to come up and reconnecting." );
	setUpdating( false );
	_currentVersionLabel->setText( "Current firmware version:" );
	_updateLabel->clear();
	_connectTimer.start();
}
