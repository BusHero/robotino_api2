#include "MainWindow.h"

MainWindow::MainWindow()
	: QDialog( NULL )
	, _logView( new LogView( this ) )
	, _connectButton( new QPushButton( this ) )
	, _deviceSelector( new QComboBox( this ) )
	, _receiveTimer( new QTimer( this ) )
	, _getSensorReadings( new QPushButton( this ) )
	, _servo1Slider( new QSlider( this ) )
	, _servo2Slider( new QSlider( this ) )
	, _sensorLabel( new QLabel( this ) )
	, _led1( new QCheckBox( this ) )
	, _led2( new QCheckBox( this ) )
{
	_deviceSelector->addItem( "USB" );
	for( int i=0; i<100; ++i )
	{
		_deviceSelector->addItem( QString( "COM%1" ).arg( i ) );
	}
	_deviceSelector->setCurrentIndex( 0 );

	_getSensorReadings->setText( "Get SensorReadings" );
	_led1->setText( "LED1" );
	_led2->setText( "LED2" );
	
	_servo1Slider->setOrientation( Qt::Horizontal );
	_servo1Slider->setRange( 0, 1000 );

	_servo2Slider->setOrientation( Qt::Horizontal );
	_servo2Slider->setRange( 0, 1000 );

	QVBoxLayout* vl = new QVBoxLayout;
	setLayout( vl );

	QHBoxLayout* hlConnect = new QHBoxLayout;
	vl->addLayout( hlConnect );
	hlConnect->addWidget( _connectButton );
	hlConnect->addWidget( _deviceSelector );

	QFormLayout* formLayout = new QFormLayout;
	vl->addLayout( formLayout );

	formLayout->addRow( _getSensorReadings );
	formLayout->addRow( _led1 );
	formLayout->addRow( _led2 );
	formLayout->addRow( "Sensors", _sensorLabel );
	formLayout->addRow( "PWM1", _servo1Slider );
	formLayout->addRow( "PWM2", _servo2Slider );

	vl->addWidget( _logView );

	updateConnectButton();

	bool ok;
	ok = connect( _connectButton, SIGNAL( clicked() ), SLOT( on_connectButton_clicked() ) );
	Q_ASSERT( ok );

	ok = connect( _servo1Slider, SIGNAL( valueChanged( int ) ), SLOT( on_servo1Slider_valueChanged( int ) ) );
	Q_ASSERT( ok );

	ok = connect( _servo2Slider, SIGNAL( valueChanged( int ) ), SLOT( on_servo2Slider_valueChanged( int ) ) );
	Q_ASSERT( ok );

	ok = connect( _led1, SIGNAL( stateChanged( int ) ), SLOT( on_led1_stateChanged( int ) ) );
	Q_ASSERT( ok );

	ok = connect( _led2, SIGNAL( stateChanged( int ) ), SLOT( on_led2_stateChanged( int ) ) );
	Q_ASSERT( ok );

	ok = connect( &_com, SIGNAL( log( const QString& ) ), _logView, SLOT( log( const QString& ) ) );
	Q_ASSERT( ok );

	ok = connect( &_com, SIGNAL( sensorReadings( float, float, float, float, float ) ), SLOT( on_sensorReadings( float, float, float, float, float ) ) );
	Q_ASSERT( ok );

	ok = connect( _receiveTimer, SIGNAL( timeout() ), SLOT( on_receiveTimer_timeout() ) );
	Q_ASSERT( ok );

	ok = connect( _getSensorReadings, SIGNAL( clicked() ), SLOT( on_getSensorReadings_clicked() ) );
	Q_ASSERT( ok );

	_receiveTimer->setInterval( 50 );
	_receiveTimer->setSingleShot( false );
	_receiveTimer->start();
}

void MainWindow::on_connectButton_clicked()
{
	if( _com.isOpen() )
	{
		_com.close();
	}
	else
	{
		if( _deviceSelector->currentText() == "USB" )
		{
			_com.open();
		}
		else
		{
			_com.open( _deviceSelector->currentText().toLatin1().constData() );
		}
	}

	updateConnectButton();
}

void MainWindow::on_getSensorReadings_clicked()
{
	_com.getSensorReadings();
	_com.transmit();
}

void MainWindow::on_receiveTimer_timeout()
{
	_com.parse();
}

void MainWindow::updateConnectButton()
{
	if( _com.isOpen() )
	{
		_connectButton->setText( "Close connection" );
		_getSensorReadings->setEnabled( true );
		_servo1Slider->setEnabled( true );
		_servo2Slider->setEnabled( true );
	}
	else
	{
		_connectButton->setText( "Open connection" );
		_getSensorReadings->setEnabled( false );
		_servo1Slider->setEnabled( false );
		_servo2Slider->setEnabled( false );
	}
}

void MainWindow::on_sensorReadings( float i1, float i2, float ad3, float ad4, float ad6 )
{
	_sensorLabel->setText( QString("i1:%1A i2:%2A ad3:%3V ad4:%4V ad6:%5V" ).arg( i1, 0, 'g', 3 ).arg( i2, 0, 'g', 3 ).arg( ad3, 0, 'g', 3 ).arg( ad4, 0, 'g', 3 ).arg( ad6, 0, 'g', 3 ) );
}

void MainWindow::on_servo1Slider_valueChanged( int value )
{
	_com.setPwm( 0, value );
	_com.transmit();
}

void MainWindow::on_servo2Slider_valueChanged( int value )
{
	_com.setPwm( 1, value );
	_com.transmit();
}

void MainWindow::on_led1_stateChanged( int state )
{
	_com.setLed( 0, Qt::Checked == state );
	_com.transmit();
}

void MainWindow::on_led2_stateChanged( int state )
{
	_com.setLed( 1, Qt::Checked == state );
	_com.transmit();
}
