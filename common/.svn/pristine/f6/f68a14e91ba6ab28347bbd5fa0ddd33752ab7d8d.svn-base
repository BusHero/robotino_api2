#include "rec/grappler/serial/SerialThread.h"
#include "rec/grappler/serial/USBProtocol.h"

#ifdef WIN32
#include <windows.h>
#include <setupapi.h>
#include <winioctl.h>
#include <vector>
#include <cassert>

#define ID "USB\\VID_1E29&PID_0408"
#endif

using namespace rec::grappler::serial;

SerialThread::SerialThread( QObject* parent )
: QThread( parent )
, _run( false )
, _serialWriteThread( new SerialWriteThread( this, &_serialPort ) )
{
	qRegisterMetaType<rec::grappler::HardwareInfo>();
}

SerialThread::~SerialThread()
{
	close();
}

bool SerialThread::open()
{
	QString portStr;

#ifdef WIN32
	HDEVINFO devInfoSet = SetupDiGetClassDevsA( &GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR, ID, NULL, DIGCF_PRESENT );
	if ( devInfoSet == INVALID_HANDLE_VALUE )
		return false;

	SP_DEVINFO_DATA devInfo;
	devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
	if ( !SetupDiEnumDeviceInfo( devInfoSet, 0, &devInfo ) )
		return false;

	HKEY deviceKey = SetupDiOpenDevRegKey( devInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE );
	if ( !deviceKey )
		return false;

	std::string portName( 256, '\0' );
	DWORD size = portName.size();
	DWORD type = 0;
	LSTATUS result = RegQueryValueExA(deviceKey, "PortName", NULL, &type, reinterpret_cast< LPBYTE >( &portName[0] ), &size );
	RegCloseKey( deviceKey );
	if ( ( result != ERROR_SUCCESS ) || ( type != REG_SZ ) )
		return false;

	portStr = QString::fromLatin1( portName.c_str() );

#ifdef _DEBUG
	assert( portStr.startsWith( "COM" ) );
	bool ok = false;
	portStr.right( portStr.length() - 3 ).toInt( &ok );
	assert( ok );
#endif

#else
	portStr = QString( "/dev/ttyACM0" );
#endif

	return open( portStr );
}

bool SerialThread::open( const QString& port )
{
	close();

	_portStr = port;

	try
	{
		_serialPort.open( _portStr.toStdString(), 200 );
		_run = true;
		start();
		_serialWriteThread->start();
	}
	catch( rec::serialport::SerialPortException& e )
	{
		Q_EMIT serialPortError( e.getMessage().c_str() );
		return false;
	}

	return true;
}

void SerialThread::close()
{
	_portStr.clear();
	_run = false;
	wait();
	_serialWriteThread->stop();
	_serialPort.close();
}

void SerialThread::sendDynamixelCommand( const QByteArray& message )
{
	//qDebug() << "Write " << command.size() << " bytes";

	_serialWriteThread->sendDynamixelCommand( message );
	//_serialPort.write( (const unsigned char*)command.constData(), command.size() );
}

void SerialThread::sendGetDetectedServos()
{
	_serialWriteThread->sendGetDetectedServos();
}

void SerialThread::sendAllPositions( const rec::grappler::HardwareInfo& info )
{
	QVector<quint16> positions;
	QVector<quint16> speeds;

	Q_FOREACH( const rec::grappler::ServoInfo& i, info.servos )
	{
		if( i.found )
		{
			positions << i.currentPosition;
			speeds << i.currentSpeed;
		}
		else
		{
			positions << 0;
			speeds << 0;
		}
	}

	QByteArray data = USBProtocol::encodeSetAllPositions( positions, speeds );

	_serialWriteThread->sendAllPositions( data );
}

void SerialThread::sendGetAllPositions()
{
	_serialWriteThread->sendGetAllPositions();
}

void SerialThread::sendGetAllAxesLimits()
{
	_serialWriteThread->sendGetAllAxesLimits();
}

void SerialThread::sendEnablePower( int channel, bool enable )
{
	_serialWriteThread->sendEnablePower( channel, enable );
}
	
void SerialThread::sendToggleTorque()
{
	_serialWriteThread->sendToggleTorque();
}

void SerialThread::sendResetDevice( bool enterUSBBootloader )
{
	_serialWriteThread->sendResetDevice( enterUSBBootloader );
}

void SerialThread::sendGetInfo()
{
	_serialWriteThread->sendGetInfo();
}

void SerialThread::setAutoUpdateEnabled( bool enable )
{
	_serialWriteThread->setAutoUpdateEnabled( enable );
}

bool SerialThread::isOpen() const
{
	return _serialPort.isOpen();
}

QString SerialThread::printBuffer(QByteArray buffer) const
{
	QString bufferStr;
	for(int i=0; i<buffer.size();++i)
	{
		QString dataStr = QString("0x%1").arg((int)(0xFF & buffer.at(i)),2,16,QChar('0'));
		dataStr.toUpper();
		
		bufferStr += dataStr;
		
		if( 0 == (i+1)%16 )
		{
			bufferStr += "\n";
		}
		else
		{
			bufferStr += " ";
		}
	}
	
	return bufferStr;
}

void SerialThread::run()
{
	QByteArray buffer;
	int errorCount = 0;
	bool firstTime=true;
	
	const char keepAliveMessage[] = {0x03,0xFF,0xFD};
	
	while( _run )
	{
		unsigned char ch;
		int bytesRead = _serialPort.read( &ch, 1 );

		if( bytesRead > 0 )
		{	
			errorCount = 0;
			
			buffer.append( (const char)ch );
			
			if(firstTime)
			{
				//qDebug() << "New byte " << QString("%1").arg((int)(0xFF & ch),2,16,QChar('0'));
				
				if(3 == buffer.size())
				{
					if(0 == memcmp(buffer.constData(),keepAliveMessage,3))
					{
						firstTime = false;
						Q_EMIT communicationReady();
					}
					else
					{
						buffer = buffer.mid(1);
						continue;
					}
				}
			}
			
			if(buffer.size() > 64 || (unsigned char)buffer[0] > 64 )
			{
				buffer = buffer.mid(1);
			}
			
			const char* messageData = buffer.constData();
			
			if( rec::grappler::serial::USBProtocol::isMessageValid( messageData, buffer.size() ) )
			{
				bool ok = true;
				unsigned char messageLength = (unsigned char)(*messageData);
				unsigned char type = (unsigned char)(*(messageData+1));
				
				switch( type )
				{
				case REC_GRAPPLER_StatusReceiveTimeout:
					if( 4 == messageLength )
					{
						int channel = messageData[3];

						serialPortError( QString( "Status Receive Timeout channel %1 received" ).arg( channel ) );
					}
					else
					{
						Q_EMIT serialPortError( QString( "Invalid StatusReceiveTimeout message length %1 received\n%2" ).arg((unsigned int)messageLength ).arg(printBuffer(buffer)) );
						ok = false;
					}
					break;

				case REC_GRAPPLER_Status:
					{
						int id = 0;
						int error = 0;
						QByteArray statusData;
						if( rec::grappler::serial::USBProtocol::decodeStatusMessage( messageData, buffer.size(), &id, &error, &statusData ) )
						{
							Q_EMIT statusReceived( id, error, statusData );
						}
						else
						{
							Q_EMIT serialPortError( QString( "Invalid StatusMessage message received\n%1" ).arg(printBuffer(buffer)) );
							ok = false;
						}
					}
					break;

				case REC_GRAPPLER_EnableMotors:
					{
						bool enable;
						if( rec::grappler::serial::USBProtocol::decodeEnableMotorsMessage( messageData, buffer.size(), &enable ) )
						{
							Q_EMIT enableMotors( enable );
						}
						else
						{
							Q_EMIT serialPortError( QString( "Invalid EnableMotors message received\n%1" ).arg(printBuffer(buffer)) );
							ok = false;
						}
					}
					break;

				case REC_GRAPPLER_StorePosition:
					{
						rec::grappler::HardwareInfo info = rec::grappler::serial::USBProtocol::decodeStorePosition( messageData );
						Q_EMIT storePosition( info );
					}
					break;

				case REC_GRAPPLER_KeepAlive:
					if( 3 == messageLength )
					{
						Q_EMIT keepAliveReceived();
					}
					else
					{
						Q_EMIT serialPortError( QString( "Invalid keep alive message length %1 received\n%2" ).arg((unsigned int)messageLength ).arg(printBuffer(buffer)) );
						ok = false;
					}	
					break;

				case REC_GRAPPLER_Error:
					Q_EMIT serialPortError( QString( "Error %1 received" ).arg( type ) );
					ok = false;
					break;

				case REC_GRAPPLER_AllServoPositions:
					{
						rec::grappler::HardwareInfo info = rec::grappler::serial::USBProtocol::decodeAllServoPositions( messageData );
						Q_EMIT allServoPositions( info );
					}
					break;

				case REC_GRAPPLER_DetectedServos:
					{
						rec::grappler::HardwareInfo info = rec::grappler::serial::USBProtocol::decodeDetectedServos( messageData );
						Q_EMIT servosDetected( info );
					}
					break;

				case REC_GRAPPLER_AllAxesLimits:
					{
						rec::grappler::HardwareInfo info = rec::grappler::serial::USBProtocol::decodeAllAxesLimits( messageData );
						Q_EMIT allAxesLimits( info );
					}
					break;

				case REC_GRAPPLER_Info:
					{
						int majorVer, minorVer, patchVer;
						if ( rec::grappler::serial::USBProtocol::decodeInfoMessage( messageData, buffer.size(), &majorVer, &minorVer, &patchVer ) )
						{
							Q_EMIT infoReceived( majorVer, minorVer, patchVer );
						}
						else
						{
							Q_EMIT serialPortError( QString( "Invalid InfoMessage message received\n%1" ).arg(printBuffer(buffer)) );
							ok = false;
						}
					}
					break;

				default:
					//Q_EMIT serialPortError( QString( "Invalid message type %1 received\n%2" ).arg( type ).arg(printBuffer(buffer)) );
					ok = false;
					break;
				}
				
				if(ok)
				{
					buffer = buffer.mid(messageLength);
				}
				else
				{
					buffer = buffer.mid(1);
				}
			}
		}
		else
		{
			++errorCount;
			
			if(errorCount > 5)
			{
				_serialPort.close();
				Q_EMIT serialPortError( "Keep alive timeout" );
				return;
			}
			else
			{
				msleep(100);
			}
		}	
	}
}

SerialWriteThread::SerialWriteThread( QObject* parent, rec::serialport::SerialPort* serialPort )
: QThread( parent )
, _run( true )
, _serialPort( serialPort )
, _messages( REC_GRAPPLER_NUMMESSAGES_PC2LPC, rec::grappler::serial::MessageBuffer() )
{
}

void SerialWriteThread::start()
{
	_run = true;
	QThread::start();
}

void SerialWriteThread::stop()
{
	_run = false;
	_sendSemaphore.release();
	wait();
}

void SerialWriteThread::sendDynamixelCommand( const QByteArray& message )
{
	if( false == _run || message.isEmpty() )
	{
		return;
	}

	QMutexLocker lk( &_serialPortMutex );
	_serialPort->write( (const unsigned char*)message.constData(), message.size() );
}

void SerialWriteThread::sendGetDetectedServos()
{
	QMutexLocker lk( &_messages[REC_GRAPPLER_GetDetectedServos].mutex );

	_messages[REC_GRAPPLER_GetDetectedServos].message =
		QByteArray(
		(const char*)rec::grappler::serial::USBProtocol::getDetectedServosMessage,
		rec::grappler::serial::USBProtocol::getDetectedServosMessageSize );

	_sendSemaphore.release();
}

void SerialWriteThread::sendAllPositions( const QByteArray& message )
{
	QMutexLocker lk( &_messages[REC_GRAPPLER_SetAllPositions].mutex );

	_messages[REC_GRAPPLER_SetAllPositions].message = message;

	_sendSemaphore.release();
}

void SerialWriteThread::sendGetAllPositions()
{
	QMutexLocker lk( &_messages[REC_GRAPPLER_GetAllPositions].mutex );

	_messages[REC_GRAPPLER_GetAllPositions].message =
		QByteArray(
		(const char*)rec::grappler::serial::USBProtocol::getAllPositionsMessage,
		rec::grappler::serial::USBProtocol::getAllPositionsMessageSize );

	_sendSemaphore.release();
}

void SerialWriteThread::sendGetAllAxesLimits()
{
	QMutexLocker lk( &_messages[REC_GRAPPLER_GetAllAxesLimits].mutex );

	_messages[REC_GRAPPLER_GetAllAxesLimits].message =
		QByteArray(
		(const char*)rec::grappler::serial::USBProtocol::getAllAxesLimitsMessage,
		rec::grappler::serial::USBProtocol::getAllAxesLimitsMessageSize );

	_sendSemaphore.release();
}

void SerialWriteThread::sendEnablePower( int channel, bool enable )
{
	QMutexLocker lk( &_messages[REC_GRAPPLER_EnablePower].mutex );

	_messages[REC_GRAPPLER_EnablePower].message = rec::grappler::serial::USBProtocol::encodeEnablePower( channel, enable );

	_sendSemaphore.release();
}
	
void SerialWriteThread::sendToggleTorque()
{
	QMutexLocker lk( &_messages[REC_GRAPPLER_GetAllPositions].mutex );

	_messages[REC_GRAPPLER_GetAllPositions].message =
		QByteArray(
		(const char*)rec::grappler::serial::USBProtocol::getToggleTorqueMessage,
		rec::grappler::serial::USBProtocol::getToggleTorqueMessageSize );

	_sendSemaphore.release();
}

void SerialWriteThread::sendResetDevice( bool enterUSBBootloader )
{
	QMutexLocker lk( &_messages[REC_GRAPPLER_ResetDevice].mutex );

	if ( enterUSBBootloader )
	{
		_messages[REC_GRAPPLER_ResetDevice].message = 
			QByteArray(
			(const char*)rec::grappler::serial::USBProtocol::resetDeviceEnterBootloaderMessage,
			rec::grappler::serial::USBProtocol::resetDeviceMessageSize );
	}
	else
	{
		_messages[REC_GRAPPLER_ResetDevice].message = 
			QByteArray(
			(const char*)rec::grappler::serial::USBProtocol::resetDeviceMessage,
			rec::grappler::serial::USBProtocol::resetDeviceMessageSize );
	}

	_sendSemaphore.release();
}

void SerialWriteThread::sendGetInfo()
{
	QMutexLocker lk( &_messages[REC_GRAPPLER_GetInfo].mutex );

	_messages[REC_GRAPPLER_GetInfo].message =
		QByteArray(
		(const char*)rec::grappler::serial::USBProtocol::getInfoMessage,
		rec::grappler::serial::USBProtocol::getInfoMessageSize );

	_sendSemaphore.release();
}

void SerialWriteThread::setAutoUpdateEnabled( bool enable )
{
	QMutexLocker lk( &_messages[REC_GRAPPLER_EnableAutoUpdate].mutex );

	_messages[REC_GRAPPLER_EnableAutoUpdate].message = rec::grappler::serial::USBProtocol::encodeEnableAutoUpdate( enable );

	_sendSemaphore.release();
}

void SerialWriteThread::run()
{
	setAutoUpdateEnabled( false );
	
	while( _run )
	{
		_sendSemaphore.acquire( std::max<int>( _sendSemaphore.available(), 1 ) );

		if( false == _run )
		{
			return;
		}

		for( int i=0; i<_messages.size(); ++i )
		{
			QMutexLocker lk( &_messages[i].mutex );
			if( false == _messages[i].message.isEmpty() )
			{
				//for( int j=0; j< _messages[i].message[0]; ++j )
				//{
				//	qDebug() << (int)_messages[i].message[j];
				//}

				{
					QMutexLocker lk( &_serialPortMutex );
					_serialPort->write( (const unsigned char*)_messages[i].message.constData(), _messages[i].message.size() );
				}
				_messages[i].message.clear();
				
				msleep(20);
			}
		}

		msleep( 50 );
	}
}
