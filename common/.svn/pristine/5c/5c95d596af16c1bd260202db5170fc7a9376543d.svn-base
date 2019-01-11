#include "rec/robotinoxt/serial/USBProtocol.h"

using namespace rec::robotinoxt::serial;

const unsigned char USBProtocol::resetDeviceMessage[] = { 0x04, 0x00, 0xFB, 0x00 };
const unsigned char USBProtocol::resetDeviceEnterBootloaderMessage[] = { 0x04, 0x00, 0xFA, 0x01 };

const unsigned char USBProtocol::getInfoMessage[] = { 0x03, 0x01, 0xFB };

const unsigned char USBProtocol::setCompressorsEnabledMessage[]  = { 0x04, 0x04, 0xF6, 0x01 };
const unsigned char USBProtocol::setCompressorsDisabledMessage[] = { 0x04, 0x04, 0xF7, 0x00 };

bool USBProtocol::isMessageValid( const char* data, unsigned int dataSize )
{
	if( dataSize < 3 )
	{
		return false;
	}

	if( (unsigned char)data[0] > dataSize )
	{
		return false;
	}

	if( rec_robotinoxt_lpcToPC_checksum( data ) != (unsigned char)data[2] )
	{
		return false;
	}

	return true;
}

bool USBProtocol::isKeepAliveMessage( const char* data, unsigned int dataSize )
{
	if( 3 != data[0] )
	{
		return false;
	}

	if( REC_ROBOTINOXT_KeepAlive != (unsigned char)data[2] )
	{
		return false;
	}

	return true;
}

bool USBProtocol::decodeInfoMessage( const char* data, unsigned int dataSize, int* majorVer, int* minorVer, int* patchVer )
{
	memcpy( majorVer, data + 3, sizeof( int ) );
	memcpy( minorVer, data + 3 + sizeof( int ), sizeof( int ) );
	memcpy( patchVer, data + 3 + 2 * sizeof( int ), sizeof( int ) );

	return true;
}

QByteArray USBProtocol::encodeSetPressuresMessage( const QVector< qint16 >& pressures )
{
	Q_ASSERT( pressures.size() == 8 );
	const int size = 8 * sizeof( qint16 );
	unsigned int len = size + 3;
	QByteArray msg( len, 0 );
	msg[0] = len;
	msg[1] = REC_ROBOTINOXT_SetPressures;
	memcpy( msg.data() + 3, pressures.constData(),size );
	msg[2] = rec_robotinoxt_lpcToPC_checksum( msg.constData() );
	return msg;
}

bool USBProtocol::decodeStatusMessage( const char* data, unsigned int dataSize, QVector< qint16 >* pressures, bool* pressureSensor, QVector< qint16 >* potis )
{
	if ( 0 == pressures || 0 == pressureSensor || 0 == potis )
		return false;

	const int pressuresSize = sizeof( qint16 ) * 8;
	const int potisSize = sizeof( qint16 ) * 7;
	const int pressureSensorSize = sizeof( unsigned char );
	const int errorSize = sizeof( unsigned char );

	if ( ( dataSize != 3 + pressuresSize + potisSize + pressureSensorSize + errorSize ) && ( data[0] != dataSize ) )
		return false;

	const unsigned char& error = data[3];
	if ( ( error & REC_ROBOTINOXT_STATUS_ErrorReadPressures ) == 0 )
	{
		pressures->resize( 8 );
		memcpy( pressures->data(), data + 4, pressuresSize );
	}
	if ( ( error & REC_ROBOTINOXT_STATUS_ErrorReadIOs ) == 0 )
	{
		*pressureSensor = ( data[20] != 0 );
		potis->resize( 7 );
		memcpy( potis->data(), data + 21, potisSize );
	}
	return true;
}

QByteArray USBProtocol::encodeSetDOutMessage( unsigned int number, bool enabled )
{
	QByteArray msg( 4, 0 );
	msg[0] = 4;
	msg[1] = REC_ROBOTINOXT_SetDOut;
	msg[3] = ( number << 4 ) | static_cast< unsigned char >( enabled );
	msg[2] = rec_robotinoxt_lpcToPC_checksum( msg.constData() );
	return msg;
}
