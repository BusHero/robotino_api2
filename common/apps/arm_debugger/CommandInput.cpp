#include "CommandInput.h"
#include "DynamixelCommandInput.h"
#include "PositionStorage.h"
#include "PositionTable.h"

CommandInput::CommandInput( QWidget* parent )
: QWidget( parent )
, _positionStorage( new PositionStorage( this ) )
, _getDetectedServos( new QPushButton( this ) )
, _positionTable( new PositionTable( this ) )
, _getAllPositionsClicked( false )
, _enterUSBBootloader( new QCheckBox( "Enter USB bootloader", this ) )
{
	//rec::grappler::serial::USBProtocol::setProtocolVerion(2);

	DynamixelCommandInput* dynamixelCommand = new DynamixelCommandInput( this );
	bool ok = (bool)connect( dynamixelCommand, SIGNAL( sendCommand( const QByteArray& ) ), SLOT( on_dynamixelcommand_sendCommand( const QByteArray& ) ) );
	Q_ASSERT(ok);

	ok = (bool)connect(dynamixelCommand, SIGNAL(error(const QString&)), SLOT(on_dynamixelcommand_error(const QString&)));
	Q_ASSERT(ok);

	_getDetectedServos->setText( "Scan servos" );
	ok = (bool)connect(_getDetectedServos, SIGNAL(clicked()), SLOT(on_getDetectedServos_clicked()));
	Q_ASSERT(ok);

	QCheckBox* rx64power = new QCheckBox( this );
	rx64power->setChecked( true );
	rx64power->setText( "RX64 power" );
	ok = (bool)connect(rx64power, SIGNAL(stateChanged(int)), SLOT(on_rx64power_stateChanged(int)));
	Q_ASSERT(ok);

	QCheckBox* rx28power = new QCheckBox( this );
	rx28power->setChecked( true );
	rx28power->setText( "RX28 power" );
	ok = (bool)connect(rx28power, SIGNAL(stateChanged(int)), SLOT(on_rx28power_stateChanged(int)));
	Q_ASSERT(ok);

	QCheckBox* rx10power = new QCheckBox( this );
	rx10power->setChecked( true );
	rx10power->setText( "RX10 power" );
	ok = (bool)connect(rx10power, SIGNAL(stateChanged(int)), SLOT(on_rx10power_stateChanged(int)));
	Q_ASSERT(ok);

	QPushButton* toggleTorque = new QPushButton( this );
	toggleTorque->setText( "Toggle torque" );
	ok = (bool)connect(toggleTorque, SIGNAL(clicked()), SLOT(on_toggleTorque_clicked()));
	Q_ASSERT(ok);

	QPushButton* cwAngleLimits = new QPushButton( this );
	cwAngleLimits->setText( "Set CW angle limits" );
	ok = (bool)connect(cwAngleLimits, SIGNAL(clicked()), SLOT(on_setCWAngleLimits_clicked()));
	Q_ASSERT(ok);

	QPushButton* ccwAngleLimits = new QPushButton( this );
	ccwAngleLimits->setText( "Set CCW angle limits" );
	ok = (bool)connect(ccwAngleLimits, SIGNAL(clicked()), SLOT(on_setCCWAngleLimits_clicked()));
	Q_ASSERT(ok);

	//QPushButton* getAllPositions = new QPushButton( "Get all positions", this );
	//(bool)connect( getAllPositions, SIGNAL( clicked() ), SLOT( on_getAllPositions_clicked() ) );

	QPushButton* storePosition = new QPushButton( this );
	storePosition->setText( "Store current position" );
	ok = (bool)connect(storePosition, SIGNAL(clicked()), SLOT(on_storePosition_clicked()));
	Q_ASSERT(ok);

	QPushButton* sendAllPositions = new QPushButton( this );
	sendAllPositions->setText( "Send all positions" );
	ok = (bool)connect(sendAllPositions, SIGNAL(clicked()), SLOT(on_sendAllPositions_clicked()));
	Q_ASSERT(ok);

	//QCheckBox* enableAutoUpdateCheckBox = new QCheckBox( this );
	//enableAutoUpdateCheckBox->setText( "Enable auto update" );
	//(bool)connect( enableAutoUpdateCheckBox, SIGNAL( stateChanged( int ) ), SLOT( on_enableAutoUpdateCheckBox_stateChanged( int ) ) );

	QPushButton* resetDevice = new QPushButton( "Reset device", this );
	ok = (bool)connect(resetDevice, SIGNAL(clicked()), SLOT(on_resetDevice_clicked()));
	Q_ASSERT(ok);

	QVBoxLayout* layout = new QVBoxLayout;
	setLayout( layout );

	layout->addWidget( dynamixelCommand );
	layout->addWidget( _getDetectedServos );
	layout->addWidget( rx64power );
	layout->addWidget( rx28power );
	layout->addWidget( rx10power );
	layout->addWidget( toggleTorque );
	layout->addWidget( cwAngleLimits );
	layout->addWidget( ccwAngleLimits );
	//layout->addWidget( getAllPositions );
	//layout->addWidget( enableAutoUpdateCheckBox );
	layout->addWidget( _positionTable );
	layout->addWidget( _positionStorage );
	layout->addWidget( sendAllPositions );
	layout->addWidget( storePosition );

	QHBoxLayout* resetLayout = new QHBoxLayout;
	resetLayout->addWidget( resetDevice );
	resetLayout->addWidget( _enterUSBBootloader );

	layout->addLayout( resetLayout );

	ok = (bool)connect( _positionStorage, SIGNAL( load( const QString& ) ), SLOT( on_positionStorage_load( const QString& ) ) );
	Q_ASSERT(ok);
}

void CommandInput::setServosDetected( const rec::grappler::HardwareInfo& info )
{
	_servosDetectedInfo = info;
	_positionTable->setServosDetected( info );
}

void CommandInput::setAllServoPositions( const rec::grappler::HardwareInfo& info )
{
	_currentInfo = info;
	_positionTable->setAllServoPositions( info, _getAllPositionsClicked );
	_getAllPositionsClicked = false;
}

void CommandInput::storePosition( const rec::grappler::HardwareInfo& info )
{
	QString str;
	
	for( int i=0; i<info.servos.size(); ++i )
	{
		str += QString( "(%1;%2) " ).arg( info.servos[i].currentPosition ).arg( 80 );
	}

	_positionStorage->addItem( str );
}

void CommandInput::setAxesLimits( const rec::grappler::HardwareInfo& info )
{
	_positionTable->setAxesLimits( info );
}

void CommandInput::on_sendAllPositions_clicked()
{
	QVector<quint16> positions = _positionTable->positions();
	QVector<quint16> speeds = _positionTable->speeds();

	QByteArray message = rec::grappler::serial::USBProtocol::encodeSetAllPositions( positions, speeds );

	Q_EMIT sendAllPositions( message );
}

void CommandInput::on_storePosition_clicked()
{
	QString str;

	QVector<quint16> positions = _positionTable->currentPositions();
	QVector<quint16> speeds = _positionTable->speeds();

	for( int i=0; i<positions.size(); ++i )
	{
		str += QString( "(%1;%2) " ).arg( positions[i] ).arg( speeds[i] );
	}

	_positionStorage->addItem( str );
}

void CommandInput::on_positionStorage_load( const QString& str )
{
	QStringList posStrList = str.split( QRegExp( "\\s" ), QString::SkipEmptyParts );

	if( posStrList.size() != 9 )
	{
		return;
	}

	rec::grappler::HardwareInfo info;

	QVector<quint16> positions( 9 );
	QVector<quint16> speeds( 9 );

	for( int i=0; i<positions.size(); ++i )
	{
		QString psoStr = posStrList[i];
		psoStr.remove( "(" );
		psoStr.remove( ")" );

		QStringList valueStrList = psoStr.split( QRegExp( ";" ), QString::SkipEmptyParts );
		if( valueStrList.size() != 2 )
		{
			return;
		}

		positions[i] = valueStrList.first().toUInt();
		speeds[i] = valueStrList.last().toUInt();
	}

	QByteArray message = rec::grappler::serial::USBProtocol::encodeSetAllPositions( positions, speeds );

	Q_EMIT sendAllPositions( message );
}

void CommandInput::on_getDetectedServos_clicked()
{
	Q_EMIT getDetectedServos();
}

void CommandInput::on_getAllPositions_clicked()
{
	_getAllPositionsClicked = true;
	Q_EMIT getAllPositions();
}

void CommandInput::on_dynamixelcommand_sendCommand( const QByteArray& command )
{
	Q_EMIT sendCommand( command );
}

void CommandInput::on_dynamixelcommand_error( const QString& errorMessage )
{
}

void CommandInput::on_enableAutoUpdateCheckBox_stateChanged( int state )
{
	Q_EMIT enableAutoUpdate( state == Qt::Checked );
}

void CommandInput::on_rx64power_stateChanged( int state )
{
	Q_EMIT enablePower( (int)rec::grappler::serial::RX64Channel, state == Qt::Checked );
}

void CommandInput::on_rx28power_stateChanged( int state )
{
	Q_EMIT enablePower( (int)rec::grappler::serial::RX28Channel, state == Qt::Checked );
}

void CommandInput::on_rx10power_stateChanged( int state )
{
	Q_EMIT enablePower( (int)rec::grappler::serial::RX10Channel, state == Qt::Checked );
}

void CommandInput::on_toggleTorque_clicked()
{
	Q_EMIT toggleTorque();
}

void CommandInput::on_setCWAngleLimits_clicked()
{
	if( _servosDetectedInfo.servos.size() != _currentInfo.servos.size() )
	{
		QMessageBox::warning( this, "Error", "Scan for servos first" );
		return;
	}

	for( int i=0; i<_servosDetectedInfo.servos.size(); ++i )
	{
		const rec::grappler::ServoInfo& detectedInfo = _servosDetectedInfo.servos[i];

		if( detectedInfo.found )
		{
			const rec::grappler::ServoInfo& currentServoInfo = _currentInfo.servos[i];
			
			QByteArray ba;

			if (1 == rec::grappler::serial::USBProtocol::protocolVersion())
			{
				ba = QByteArray(3, 0);
				ba[0] = 0x6;
				ba[1] = (currentServoInfo.currentPosition & 0xFF);
				ba[2] = ((currentServoInfo.currentPosition >> 8) & 0xFF);

				qDebug() << "Send command "
					<< "ch:" << detectedInfo.channel
					<< " id:" << detectedInfo.id
					<< " instr:" << 0x3
					<< " params:" << QString("0x%1 0x%2 0x%3").arg((quint16)ba[0], 0, 16).arg((quint16)ba[1], 0, 16).arg((quint16)ba[2], 0, 16);
			}
			else
			{
				ba = QByteArray(5, 0);
				ba[0] = 52;
				ba[1] = (currentServoInfo.currentPosition & 0xFF);
				ba[2] = ((currentServoInfo.currentPosition >> 8) & 0xFF);
				ba[3] = ((currentServoInfo.currentPosition >> 16) & 0xFF);
				ba[4] = ((currentServoInfo.currentPosition >> 24) & 0xFF);

				qDebug() << "Send command "
					<< "ch:" << detectedInfo.channel
					<< " id:" << detectedInfo.id
					<< " instr:" << 0x3
					<< " params:" << QString("0x%1 0x%2 0x%3 0x%4 0x%5").arg((quint16)ba[0], 0, 16).arg((quint16)ba[1], 0, 16).arg((quint16)ba[2], 0, 16).arg((quint16)ba[3], 0, 16).arg((quint16)ba[4], 0, 16);
			}
					
			QByteArray commandData = rec::grappler::serial::USBProtocol::encodeDynamixelCommand( detectedInfo.channel, detectedInfo.id, 0x3, ba );
	
			Q_EMIT sendCommand( commandData );
		}
	}

	Q_EMIT enableAutoUpdate( true );
}

void CommandInput::on_setCCWAngleLimits_clicked()
{
	if( _servosDetectedInfo.servos.size() != _currentInfo.servos.size() )
	{
		QMessageBox::warning( this, "Error", "Scan for servos first" );
		return;
	}

	for( int i=0; i<_servosDetectedInfo.servos.size(); ++i )
	{
		const rec::grappler::ServoInfo& detectedInfo = _servosDetectedInfo.servos[i];

		if( detectedInfo.found )
		{
			const rec::grappler::ServoInfo& currentServoInfo = _currentInfo.servos[i];
			
			QByteArray ba( 3, 0 );
			ba[0] = 0x8;
			ba[1] = ( currentServoInfo.currentPosition & 0xFF );
			ba[2] = ( ( currentServoInfo.currentPosition >> 8 ) & 0xFF );

			qDebug() << "Send command "
				<< "ch:" << detectedInfo.channel
				<< " id:" << detectedInfo.id
				<< " instr:" << 0x3
				<< " params:" << QString("0x%1 0x%2 0x%3").arg( (quint16)ba[0], 0, 16 ).arg( (quint16)ba[1], 0, 16 ).arg( (quint16)ba[2], 0, 16 );
			
			QByteArray commandData = rec::grappler::serial::USBProtocol::encodeDynamixelCommand( detectedInfo.channel, detectedInfo.id, 0x3, ba );
	
			Q_EMIT sendCommand( commandData );
		}
	}

	Q_EMIT enableAutoUpdate( true );
}

void CommandInput::on_resetDevice_clicked()
{
	Q_EMIT resetDevice( _enterUSBBootloader->isChecked() );
}
