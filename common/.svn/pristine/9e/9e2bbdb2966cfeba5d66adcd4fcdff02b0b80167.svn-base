#include "rec/grappler/serial/USBProtocol.h"
#include "rec/grappler/HardwareInfo.h"

using namespace rec::grappler::serial;

const unsigned char USBProtocol::getDetectedServosMessage[] = { 0x3, 0x2, 0xFA };
const int USBProtocol::getDetectedServosMessageSize = 3;

const unsigned char USBProtocol::getAllPositionsMessage[] = { 0x3, 0x5, 0xF7 };
const int USBProtocol::getAllPositionsMessageSize = 3;

const unsigned char USBProtocol::getToggleTorqueMessage[] = { 0x3, 0x7, 0xF5 };
const int USBProtocol::getToggleTorqueMessageSize = 3;

const unsigned char USBProtocol::getAllAxesLimitsMessage[] = { 0x3, 0x8, 0xF4 };
const int USBProtocol::getAllAxesLimitsMessageSize = 3;

const unsigned char USBProtocol::resetDeviceMessage[] = { 0x4, 0x9, 0xF2, 0x0 };
const unsigned char USBProtocol::resetDeviceEnterBootloaderMessage[] = { 0x4, 0x9, 0xF1, 0x1 };
const int USBProtocol::resetDeviceMessageSize = 4;

const unsigned char USBProtocol::getInfoMessage[] = { 0x3, 0xA, 0xF2 };
const int USBProtocol::getInfoMessageSize = 3;

int USBProtocol::_protocolVersion = 1;

#ifdef WIN32
//bool rec::grappler::MX_servos_found = true;
//const float rec::grappler::RX_angle_factor = 3.41f;
//const float rec::grappler::RX_speed_factor = 0.111f;
//const float rec::grappler::MX_angle_factor = 11.375f;
//const float rec::grappler::MX_speed_factor = 0.114f;
#endif

void USBProtocol::setProtocolVerion(int version)
{
	_protocolVersion = version;
}

unsigned short update_crc_protocol2(const unsigned char *data_blk_ptr, unsigned short data_blk_size)
{
	unsigned short crc_accum = 0;
	unsigned short i = 0;
	unsigned short j = 0;
	const unsigned short crc_table[256] = {
		0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
		0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022,
		0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072,
		0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041,
		0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2,
		0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
		0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1,
		0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
		0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192,
		0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1,
		0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1,
		0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
		0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151,
		0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162,
		0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
		0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101,
		0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312,
		0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
		0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371,
		0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342,
		0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1,
		0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
		0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2,
		0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
		0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291,
		0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2,
		0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2,
		0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1,
		0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
		0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
		0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231,
		0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202
	};

	for (j = 0; j < data_blk_size; j++)
	{
		i = ((unsigned short)(crc_accum >> 8) ^ data_blk_ptr[j]) & 0xFF;
		crc_accum = (crc_accum << 8) ^ crc_table[i];
	}

	return crc_accum;
}

QByteArray USBProtocol::encodeDynamixelCommand( Channel channel, unsigned char id, unsigned char instr, const QByteArray& params )
{
	if (1 == _protocolVersion)
	{
		QByteArray data(params.size() + 11, 0);

		if (data.size() > 32)
		{
			return QByteArray();
		}

		data[0] = data.size();
		data[1] = 0;
		data[2] = 0;
		data[3] = (const char)channel;
		data[4] = (const char)0xff;
		data[5] = (const char)0xff;
		data[6] = (const char)id;
		data[7] = params.size() + 2;
		data[8] = (const char)instr;

		for (int i = 0; i < params.size(); ++i)
		{
			data[9 + i] = params[i];
		}

		data[9 + params.size()] = dynamixelChecksum(data);

		data[2] = (const char)rec_grappler_lpcToPC_checksum(data.constData());

		return data;
	}
	else
	{
		QByteArray data(params.size() + 15, 0);

		if (data.size() > 32)
		{
			return QByteArray();
		}

		data[0] = data.size();
		data[1] = 0;
		data[2] = 0;
		data[3] = (const char)channel;
		data[4] = (const char)0xff;
		data[5] = (const char)0xff;
		data[6] = (const char)0xfd;
		data[7] = (const char)0x00;
		data[8] = (const char)id;
		data[9] = (const char)params.size()+3;
		data[10] = (const char)0;
		data[11] = (const char)instr;
		for (int i = 0; i < params.size(); ++i)
		{
			data[12 + i] = params[i];
		}

		unsigned short crc = update_crc_protocol2((const unsigned char*)data.constData() + 4, params.size() + 8);

		data[13 + params.size()] = 0xFF & crc;
		data[14 + params.size()] = (crc >> 8);

		data[2] = (const char)rec_grappler_lpcToPC_checksum(data.constData());

		return data;
	}

	return QByteArray();
}

QByteArray USBProtocol::encodeSetAllPositions( const QVector<quint16>& positions, const QVector<quint16>& speeds )
{
	QByteArray data( 39, 0 );

	if( positions.size() != 9 || speeds.size() != 9 )
	{
		return data;
	}

	unsigned char* p = reinterpret_cast<unsigned char*>( data.data() );

	*p++ = 39;
	*p++ = 1;
	unsigned char* checksum = p++;
	*checksum = 0;

	for( int i=0; i<9; ++i )
	{
		const quint16& pos = positions[i];
		const quint16& speed = speeds[i];

		*p++ = ( pos & 0xFF );
		*p++ = ( pos >> 8 );
		*p++ = ( speed & 0xFF );
		*p++ = ( speed >> 8 );
	}

	*checksum = rec_grappler_lpcToPC_checksum( data.constData() );

	return data;
}

bool USBProtocol::decodeSetAllPositions( const QByteArray& data, QVector<quint16>* positions, QVector<quint16>* speeds )
{
	if( 39 != data.size() )
	{
		return false;
	}

	if( 0 == rec_grappler_isMessageCorrect( data.constData() ) )
	{
		return false;
	}

	positions->resize( 9 );
	speeds->resize( 9 );

	const unsigned char* p = reinterpret_cast<const unsigned char*>( data.constData() ) + 3;

	for( int i=0; i<9; ++i )
	{
		quint16 pos = (*p++);
		pos |= ( (*p++) << 8 );

		quint16 speed = (*p++);
		speed |= ( (*p++) << 8 );

		(*positions)[i] = pos;
		(*speeds)[i] = speed;
	}

	return true;
}

QByteArray USBProtocol::encodeEnableAutoUpdate( bool enable )
{
	QByteArray data( 4, 0 );

	unsigned char* p = reinterpret_cast<unsigned char*>( data.data() );

	*p++ = 4;
	*p++ = 3;
	*p++ = 0;
	*p++ = ( enable ? 1 : 0 );

	p = reinterpret_cast<unsigned char*>( data.data() + 2 );
	*p = rec_grappler_lpcToPC_checksum( data.constData() );

	return data;
}

QByteArray USBProtocol::encodeEnablePower( int channel, bool enable )
{
	QByteArray data( 5, 0 );

	unsigned char* p = reinterpret_cast<unsigned char*>( data.data() );

	*p++ = 5;
	*p++ = 6;
	*p++ = 0;
	*p++ = (unsigned char)channel;
	*p++ = ( enable ? 1 : 0 );

	p = reinterpret_cast<unsigned char*>( data.data() + 2 );
	*p = rec_grappler_lpcToPC_checksum( data.constData() );

	return data;
}

unsigned char USBProtocol::dynamixelChecksum( const QByteArray& data )
{
	unsigned char i;
	unsigned short sum = 0;

	for( i=6; i<data.size(); i++ )
	{
	  sum += data[i];
	}

	return ~( sum & 0xFF );
}

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

	if( rec_grappler_lpcToPC_checksum( data ) != (unsigned char)data[2] )
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

	if( REC_GRAPPLER_KeepAlive != (unsigned char)data[2] )
	{
		return false;
	}

	return true;
}

bool USBProtocol::decodeStatusMessage( const char* data, unsigned int dataSize, int* id, int* error, QByteArray* statusData )
{
	*id = (int)(*(data+5));
	*error = (int)(*(data+7));

	int numParams = (unsigned char)(*(data+6)) - 2;

	if( numParams < 0 )
	{
		return false;
	}

	for( int i=0; i<numParams; ++i )
	{
		statusData->append( *(data+8+i) );
	}

	return true;
}

bool USBProtocol::decodeEnableMotorsMessage( const char* data, unsigned int dataSize, bool* enable )
{
	*enable = ( *(data+3) > 0 );
	return true;
}

rec::grappler::HardwareInfo USBProtocol::decodeDetectedServos( const char* data )
{
	const unsigned char* p = reinterpret_cast<const unsigned char*>( data+3 );

	rec::grappler::HardwareInfo info;
	
	for( int i=0; i<info.servos.size(); ++i )
	{
		info.servos[i].found = ( *p++ > 0 );

		if( info.servos[i].found )
		{
			++info.num_servos_found;
		}
	}

	return info;
}

QByteArray USBProtocol::encodeAllServoPositions( const QVector<quint16>& positions, const QVector<quint16>& speeds )
{
	if( positions.size() != speeds.size() )
	{
		return QByteArray();
	}

	QByteArray data( 48, 0 );

	unsigned char* p = reinterpret_cast<unsigned char*>( data.data() );

	*p++ = 48;
	*p++ = 6;
	unsigned char* checksum = p++;
	*checksum = 0;

	for( int i=0; i<positions.size(); ++i )
	{
		const quint16& pos = positions[i];
		const quint16& speed = speeds[i];

		*p++ = ( pos & 0xFF );
		*p++ = ( pos >> 8 );
		*p++ = ( speed & 0xFF );
		*p++ = ( speed >> 8 );
		p++;
	}

	*checksum = rec_grappler_lpcToPC_checksum( data.constData() );

	return data;
}

rec::grappler::HardwareInfo USBProtocol::decodeAllServoPositions( const char* data )
{
	const unsigned char* p = reinterpret_cast<const unsigned char*>( data+3 );

	rec::grappler::HardwareInfo info;

	for( int i=0; i<info.servos.size(); ++i )
	{
		quint16 pos = *p++;
		pos |= ( *p++ << 8 );

		qint16 speed = *p++;
		speed |= ( *p++ << 8 );

		if( speed > 0x3FF )
		{
			speed -= 0x3FF;
			speed = -speed;
		}

		info.servos[i].currentPosition = pos;
		info.servos[i].currentSpeed = speed;
		info.servos[i].error |= ( *p++ >0 );
	}

	return info;
}

rec::grappler::HardwareInfo USBProtocol::decodeStorePosition( const char* data )
{
	const unsigned char* p = reinterpret_cast<const unsigned char*>( data+3 );

	rec::grappler::HardwareInfo info;

	for( int i=0; i<info.servos.size(); ++i )
	{
		quint16 pos = *p++;
		pos |= ( *p++ << 8 );

		info.servos[i].currentPosition = pos;
		info.servos[i].currentSpeed = 0;
	}

	return info;
}

QByteArray USBProtocol::encodeAllAxesLimits( const QVector<quint16>& cwLimits, const QVector<quint16>& ccwLimits )
{
	if( cwLimits.size() != ccwLimits.size() )
	{
		return QByteArray();
	}

	QByteArray data( 39, 0 );

	unsigned char* p = reinterpret_cast<unsigned char*>( data.data() );

	*p++ = 39;
	*p++ = 8;
	unsigned char* checksum = p++;
	*checksum = 0;

	for( int i=0; i<cwLimits.size(); ++i )
	{
		const quint16& cw = cwLimits[i];
		const quint16& ccw = ccwLimits[i];

		*p++ = ( cw & 0xFF );
		*p++ = ( cw >> 8 );
		*p++ = ( ccw & 0xFF );
		*p++ = ( ccw >> 8 );
	}

	*checksum = rec_grappler_lpcToPC_checksum( data.constData() );

	return data;
}

rec::grappler::HardwareInfo USBProtocol::decodeAllAxesLimits( const char* data )
{
	const unsigned char* p = reinterpret_cast<const unsigned char*>( data+3 );

	rec::grappler::HardwareInfo info;

	for( int i=0; i<info.servos.size(); ++i )
	{
		quint16 cwLimit = *p++;
		cwLimit |= ( *p++ << 8 );

		qint16 ccwLimit = *p++;
		ccwLimit |= ( *p++ << 8 );

		info.servos[i].cwAxisLimit = cwLimit;
		info.servos[i].ccwAxisLimit = ccwLimit;
	}

	return info;
}

bool USBProtocol::decodeInfoMessage( const char* data, unsigned int dataSize, int* majorVer, int* minorVer, int* patchVer )
{
	memcpy( majorVer, data + 3, sizeof( int ) );
	memcpy( minorVer, data + 3 + sizeof( int ), sizeof( int ) );
	memcpy( patchVer, data + 3 + 2 * sizeof( int ), sizeof( int ) );

	return true;
}

QString rec::grappler::serial::channelFriendlyName( Channel channel )
{
	switch( channel )
	{
	case RX64Channel:
		return "RX64";

	case RX28Channel:
		return "RX28";

	case RX10Channel:
		return "RX10";

	default:
		return "Unkown channel";
	}
}
