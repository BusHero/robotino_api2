#include "rec/robotinoxt/serial/SerialThread.h"
#include "rec/robotinoxt/serial/USBProtocol.h"

#ifdef WIN32
#include <windows.h>
#include <setupapi.h>
#include <winioctl.h>
#include <vector>
#include <cassert>

#define ID "USB\\VID_1E29&PID_040B"
#endif

using namespace rec::robotinoxt::serial;

SerialThread::SerialThread( QObject* parent )
: QThread( parent )
, _run( false )
, _serialWriteThread( new SerialWriteThread( this, &_serialPort ) )
{
	qRegisterMetaType< QVector< qint16 > >();
	qRegisterMetaType< QVector< float > >();
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
		_serialPort.open( _portStr.toStdString(), 115200, 200 );
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

void SerialThread::sendResetDevice( bool enterUSBBootloader, bool queued )
{
	if ( queued )
	{
		_serialWriteThread->sendResetDevice( enterUSBBootloader );
		return;
	}

	if ( enterUSBBootloader )
	{
		_serialPort.write( rec::robotinoxt::serial::USBProtocol::resetDeviceEnterBootloaderMessage,
			rec::robotinoxt::serial::USBProtocol::resetDeviceEnterBootloaderMessage[0] );
	}
	else
	{
		_serialPort.write( rec::robotinoxt::serial::USBProtocol::resetDeviceMessage,
			rec::robotinoxt::serial::USBProtocol::resetDeviceMessage[0] );
	}
}

void SerialThread::sendGetInfo()
{
	_serialWriteThread->sendGetInfo();
}

void SerialThread::sendSetPressures( const QVector< qint16 >& pressures )
{
	if ( pressures.size() != 8 )
		return;

	_serialWriteThread->sendSetPressures( pressures );
}

void SerialThread::sendSetDOut( unsigned int number, bool enabled )
{
	_serialWriteThread->sendSetDOut( number, enabled );
}

void SerialThread::sendSetCompressorsEnabled( bool enabled )
{
	_serialWriteThread->sendSetCompressorsEnabled( enabled );
}

bool SerialThread::isOpen() const
{
	return _serialPort.isOpen();
}

void SerialThread::run()
{
	unsigned char buffer[256];
	memset( buffer, 0, 256 );
	while( _run )
	{
		int bytesRead = _serialPort.read( buffer, 1 );

		if( bytesRead > 0 )
		{
			const unsigned char* pdata = buffer;

			unsigned char messageLength = *pdata;

			if( messageLength < 3 || messageLength > 60 )
			{
				qDebug() << "Invalid message length";
				continue;
			}

			while( bytesRead < messageLength )
			{
				bytesRead += _serialPort.read( buffer + 1, messageLength - bytesRead );
				if ( false == _run )
					break;
			}
			
			//qDebug() << "bytes read " << bytesRead;

			int numBytesProcessed = 0;

			while( numBytesProcessed < bytesRead )
			{
				//qDebug() << numBytesProcessed;

				const char* messageData = reinterpret_cast< const char* >( buffer ) + numBytesProcessed;
				const int maxMessageLength = bytesRead - numBytesProcessed;

				if( rec::robotinoxt::serial::USBProtocol::isMessageValid( messageData, maxMessageLength ) )
				{
					bool ok = true;
					unsigned char type = static_cast< unsigned char >( *( messageData + 1 ) );
					
					switch( type )
					{
					//case REC_ROBOTINOXT_KeepAlive:
					//	if( 3 == messageData[0] )
					//	{
					//		//qDebug() << "Keep alive received";
					//	}
					//	else
					//	{
					//		Q_EMIT serialPortError( QString( "Invalid keep alive message length %1 received" ).arg((unsigned int)messageData[0] ) );
					//		ok = false;
					//	}	
					//	break;

					case REC_ROBOTINOXT_Error:
						Q_EMIT serialPortError( QString( "Error %1 received" ).arg( type ) );
						ok = false;
						break;

					case REC_ROBOTINOXT_Info:
						{
							int majorVer, minorVer, patchVer;
							if ( rec::robotinoxt::serial::USBProtocol::decodeInfoMessage( messageData, maxMessageLength, &majorVer, &minorVer, &patchVer ) )
							{
								Q_EMIT infoReceived( majorVer, minorVer, patchVer );
							}
							else
							{
								Q_EMIT serialPortError( QString( "Invalid InfoMessage message received" ) );
								ok = false;
							}
						}
						break;

					case REC_ROBOTINOXT_Status:
						{
							QVector< qint16 > pressures;
							QVector< qint16 > potis;
							bool pressureSensor;
							if ( rec::robotinoxt::serial::USBProtocol::decodeStatusMessage( messageData, maxMessageLength, &pressures, &pressureSensor, &potis ) )
							{
								QVector< float > extPotis;
								if ( potis.size() == 7 )
								{	// Value = V / Vref (Vref = 5V)
									extPotis.resize( 7 );
									for( int i = 0; i < 7; ++i )
										extPotis[i] = float( potis[i] ) * 5 / float( 0x3FF );
								}
								// No additional error info.
								// Pressure vector empty means error retrieving pressures via CAN.
								// Potis vector empty means error reading IOs (potis and pressure sensor).
								Q_EMIT statusReceived( pressures, pressureSensor, extPotis );
							}
							else
							{
								Q_EMIT serialPortError( QString( "Invalid pressures message received" ) );
								ok = false;
							}
						}
						break;

					default:
						Q_EMIT serialPortError( QString( "Invalid message type %1 received" ).arg( type ) );
						ok = false;
						break;
					}

					if( ok )
					{
						numBytesProcessed += buffer[ numBytesProcessed ];
					}
					else
					{
						break;
					}
				}
				else
				{
					//const unsigned char* p = (const unsigned char*)buffer.constData();
					//for( int i=0; i<buffer.size(); ++i )
					//{
					//	unsigned int value = *(p++);
					//	qDebug() << value;
					//}
					Q_EMIT serialPortError( "Invalid data received" );
					break;
				}
			}
		}
		else
		{
			_serialPort.close();
			Q_EMIT serialPortError( "Keep alive timeout" );
			return;
		}	
	}
}

SerialWriteThread::SerialWriteThread( QObject* parent, rec::serialport::SerialPort* serialPort )
: QThread( parent )
, _run( true )
, _serialPort( serialPort )
, _messages( REC_ROBOTINOXT_NUMMESSAGES_PC2LPC, rec::robotinoxt::serial::MessageBuffer() )
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

void SerialWriteThread::sendResetDevice( bool enterUSBBootloader )
{
	QMutexLocker lk( &_messages[REC_ROBOTINOXT_ResetDevice].mutex );

	_messages[REC_ROBOTINOXT_ResetDevice].messages.clear();
	if ( enterUSBBootloader )
	{
		_messages[REC_ROBOTINOXT_ResetDevice].messages.enqueue( 
			QByteArray(
			(const char*)rec::robotinoxt::serial::USBProtocol::resetDeviceEnterBootloaderMessage,
			rec::robotinoxt::serial::USBProtocol::resetDeviceEnterBootloaderMessage[0] ) );
	}
	else
	{
		_messages[REC_ROBOTINOXT_ResetDevice].messages.enqueue( 
			QByteArray(
			(const char*)rec::robotinoxt::serial::USBProtocol::resetDeviceMessage,
			rec::robotinoxt::serial::USBProtocol::resetDeviceMessage[0] ) );
	}

	_sendSemaphore.release();
}

void SerialWriteThread::sendGetInfo()
{
	QMutexLocker lk( &_messages[REC_ROBOTINOXT_GetInfo].mutex );

	_messages[REC_ROBOTINOXT_GetInfo].messages.clear();
	_messages[REC_ROBOTINOXT_GetInfo].messages.enqueue( QByteArray(
		reinterpret_cast< const char* >( rec::robotinoxt::serial::USBProtocol::getInfoMessage ),
		rec::robotinoxt::serial::USBProtocol::getInfoMessage[0] ) );

	_sendSemaphore.release();
}

void SerialWriteThread::sendSetPressures( const QVector< qint16 >& pressures )
{
	QMutexLocker lk( &_messages[REC_ROBOTINOXT_SetPressures].mutex );
	_messages[REC_ROBOTINOXT_SetPressures].messages.clear();
	_messages[REC_ROBOTINOXT_SetPressures].messages.enqueue( rec::robotinoxt::serial::USBProtocol::encodeSetPressuresMessage( pressures ) );
	_sendSemaphore.release();
}

void SerialWriteThread::sendSetDOut( unsigned int number, bool enabled )
{
	QMutexLocker lk( &_messages[REC_ROBOTINOXT_SetDOut].mutex );
	_messages[REC_ROBOTINOXT_SetDOut].messages.enqueue( rec::robotinoxt::serial::USBProtocol::encodeSetDOutMessage( number, enabled ) );
	_sendSemaphore.release();
}

void SerialWriteThread::sendSetCompressorsEnabled( bool enabled )
{
	QMutexLocker lk( &_messages[REC_ROBOTINOXT_SetCompressorsEnabled].mutex );

	const unsigned char* msg = enabled
		? rec::robotinoxt::serial::USBProtocol::setCompressorsEnabledMessage
		: rec::robotinoxt::serial::USBProtocol::setCompressorsDisabledMessage;

	_messages[REC_ROBOTINOXT_SetCompressorsEnabled].messages.clear();
	_messages[REC_ROBOTINOXT_SetCompressorsEnabled].messages.enqueue( QByteArray(
		reinterpret_cast< const char* >( msg ), msg[0] ) );

	_sendSemaphore.release();
}

void SerialWriteThread::run()
{
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
			while( false == _messages[i].messages.isEmpty() )
			{
				//for( int j=0; j< _messages[i].message[0]; ++j )
				//{
				//	qDebug() << (int)_messages[i].message[j];
				//}

				QByteArray msg = _messages[i].messages.dequeue();
				if ( !msg.isEmpty() )
				{
					QMutexLocker lk( &_serialPortMutex );
					_serialPort->write( (const unsigned char*)msg.constData(), msg.size() );
				}
			}
		}

		msleep( 50 );
	}
}
