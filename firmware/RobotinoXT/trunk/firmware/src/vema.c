#include "vema.h"
#include "can.h"

#include <stdlib.h>
#include <string.h>

extern void timeoutTimer_start( void );
extern unsigned int timeoutTimer_elapsed( void );

enum VEMA_Parameter
{
	VEMA_Channel1		= 0x1,
	VEMA_Channel2,
	VEMA_Channel3,
	VEMA_Channel4,
	VEMA_Channel5,
	VEMA_Channel6,
	VEMA_Channel7,
	VEMA_Channel8,
	VEMA_ChannelGroup1	= 0xD,
	VEMA_ChannelGroup2	= 0xE,
	VEMA_AllChannels	= 0xF,
};

enum VEMA_Command
{
	VEMA_CMD_StartClosedLoop	= 0x10,
	VEMA_CMD_StopClosedLoop		= 0x20,
	VEMA_CMD_SetPressure		= 0x30,
	VEMA_CMD_GetPressure		= 0x40,
	VEMA_CMD_GetStatus			= 0x50,
	VEMA_CMD_GetError			= 0x60,
	VEMA_CMD_Pressurise			= 0x70,
	VEMA_CMD_Exhaust			= 0x80,
	VEMA_CMD_CloseValve			= 0x90,
	VEMA_CMD_SetDigitalOutput	= 0xA0,
	VEMA_CMD_ResetDigitalOutput	= 0xB0,
	VEMA_CMD_StartCalibration	= 0xC0,
	VEMA_CMD_SetCalibration		= 0xD0,
	VEMA_CMD_ResetError			= 0xE0,
};

const unsigned char simpleCmdLength = 2;

const unsigned char startClosedLoopData[simpleCmdLength] =
{
	VEMA_CMD_StartClosedLoop | VEMA_AllChannels,
	~( VEMA_CMD_StartClosedLoop | VEMA_AllChannels )
};

const unsigned char stopClosedLoopData[simpleCmdLength] =
{
	VEMA_CMD_StopClosedLoop | VEMA_AllChannels,
	~( VEMA_CMD_StopClosedLoop | VEMA_AllChannels )
};

const unsigned char getPressuresGroup1[simpleCmdLength] =
{
	VEMA_CMD_GetPressure | VEMA_ChannelGroup1,
	~( VEMA_CMD_GetPressure | VEMA_ChannelGroup1 )
};

const unsigned char getPressuresGroup2[simpleCmdLength] =
{
	VEMA_CMD_GetPressure | VEMA_ChannelGroup2,
	~( VEMA_CMD_GetPressure | VEMA_ChannelGroup2 )
};

unsigned int waitForRxReady( unsigned int timeout )
{
	timeoutTimer_start();
	while( CAN_RxRdy[0] == 0 )
	{
		if ( timeoutTimer_elapsed() > timeout )
			return 0; // RX Timeout --> error
	}
	CAN_RxRdy[0] = 0;
	return 1;
}

unsigned char checksum( unsigned char* data, unsigned char len )
{
	unsigned char sum = 0;
	while( len-- > 0 )
		sum += data[len];
	return ~sum;
}

void encodeSetPressuresMessage( enum VEMA_Parameter channelGroup, const short* pressures )
{
	short tmp = 0;

	CAN_TxMsg[0].len = 8;
	CAN_TxMsg[0].data[0] = VEMA_CMD_SetPressure | channelGroup;
	
	tmp = abs( pressures[0] );
	CAN_TxMsg[0].data[1] = ( unsigned char )( tmp >> 4 );
	if ( pressures[0] < 0 )
		CAN_TxMsg[0].data[1] |= 0x80;
	CAN_TxMsg[0].data[2] = ( tmp & 0x0F ) << 4;

	tmp = abs( pressures[1] );
	CAN_TxMsg[0].data[2] |= ( tmp >> 8 ) & 0x0F;
	if ( pressures[1] < 0 )
		CAN_TxMsg[0].data[2] |= 0x08;
	CAN_TxMsg[0].data[3] = tmp & 0xFF;

	tmp = abs( pressures[2] );
	CAN_TxMsg[0].data[4] = ( unsigned char )( tmp >> 4 );
	if ( pressures[2] < 0 )
		CAN_TxMsg[0].data[4] |= 0x80;
	CAN_TxMsg[0].data[5] = ( tmp & 0x0F ) << 4;

	tmp = abs( pressures[3] );
	CAN_TxMsg[0].data[5] |= ( tmp >> 8 ) & 0x0F;
	if ( pressures[3] < 0 )
		CAN_TxMsg[0].data[5] |= 0x08;
	CAN_TxMsg[0].data[6] = tmp & 0xFF;
	
	CAN_TxMsg[0].data[7] = checksum( CAN_TxMsg[0].data, 7 );
}

unsigned int decodeGetPressureResponse( enum VEMA_Parameter channelGroup, short* pressures )
{
	short tmp = 0;
	
	if ( CAN_RxMsg[0].len != 8 )
		return 0;
	if ( CAN_RxMsg[0].data[7] != checksum( CAN_RxMsg[0].data, 7 ) )
		return 0;
	if ( CAN_RxMsg[0].data[0] != ( VEMA_CMD_GetPressure | channelGroup ) )
		return 0;
	
	tmp = ( short )( CAN_RxMsg[0].data[1] & 0x7F ) << 4;
	tmp += ( short )( CAN_RxMsg[0].data[2] & 0xF0 ) >> 4;
	if ( ( CAN_RxMsg[0].data[1] & 0x80 ) != 0 )
		tmp = -tmp;
	pressures[0] = tmp;

	tmp = ( short )( CAN_RxMsg[0].data[2] & 0x07 ) << 8;
	tmp += ( short )CAN_RxMsg[0].data[3];
	if ( ( CAN_RxMsg[0].data[2] & 0x08 ) != 0 )
		tmp = -tmp;
	pressures[1] = tmp;

	tmp = ( short )( CAN_RxMsg[0].data[4] & 0x7F ) << 4;
	tmp += ( short )( CAN_RxMsg[0].data[5] & 0xF0 ) >> 4;
	if ( ( CAN_RxMsg[0].data[4] & 0x80 ) != 0 )
		tmp = -tmp;
	pressures[2] = tmp;

	tmp = ( short )( CAN_RxMsg[0].data[5] & 0x07 ) << 8;
	tmp += ( short )CAN_RxMsg[0].data[6];
	if ( ( CAN_RxMsg[0].data[5] & 0x08 ) != 0 )
		tmp = -tmp;
	pressures[3] = tmp;
	
	return 1;
}

unsigned int decodeSetPressureResponse( enum VEMA_Parameter channelGroup )
{
	if ( CAN_RxMsg[0].len != 8 )
		return 0;
	if ( CAN_RxMsg[0].data[7] != checksum( CAN_RxMsg[0].data, 7 ) )
		return 0;
	if ( CAN_RxMsg[0].data[0] != ( VEMA_CMD_SetPressure | channelGroup ) )
		return 0;
	return 1;
}

unsigned int vema_startClosedLoop( void )
{
	while( CAN_TxRdy[0] == 0 );
	CAN_TxRdy[0] = 0;
	CAN_TxMsg[0].len = simpleCmdLength;
	memcpy( CAN_TxMsg[0].data, startClosedLoopData, simpleCmdLength );
	CAN_wrMsg( 1, &CAN_TxMsg[0] );

	if ( waitForRxReady( 500 ) == 0 )
		return 0;

	if ( CAN_RxMsg[0].len != simpleCmdLength ||
		 memcmp( CAN_RxMsg[0].data, startClosedLoopData, simpleCmdLength ) != 0 )
		return 0; // mismatch --> error

	return 1;
}

unsigned int vema_stopClosedLoop( void )
{
	while( CAN_TxRdy[0] == 0 );
	CAN_TxRdy[0] = 0;
	CAN_TxMsg[0].len = simpleCmdLength;
	memcpy( CAN_TxMsg[0].data, startClosedLoopData, simpleCmdLength );
	CAN_wrMsg( 1, &CAN_TxMsg[0] );

	if ( waitForRxReady( 500 ) == 0 )
		return 0;

	if ( CAN_RxMsg[0].len != simpleCmdLength ||
		 memcmp( CAN_RxMsg[0].data, stopClosedLoopData, simpleCmdLength ) != 0 )
		return 0;

	return 1;
}

unsigned int vema_getPressures( short* pressures )
{
	while( CAN_TxRdy[0] == 0 );
	CAN_TxRdy[0] = 0;
	CAN_TxMsg[0].len = simpleCmdLength;
	memcpy( CAN_TxMsg[0].data, getPressuresGroup1, simpleCmdLength );
	CAN_wrMsg( 1, &CAN_TxMsg[0] );
	
	if ( waitForRxReady( 50 ) == 0 )
		return 0;

	if ( decodeGetPressureResponse( VEMA_ChannelGroup1, pressures ) == 0 )
		return 0;

	while( CAN_TxRdy[0] == 0 );
	CAN_TxRdy[0] = 0;
	memcpy( CAN_TxMsg[0].data, getPressuresGroup2, simpleCmdLength );
	CAN_wrMsg( 1, &CAN_TxMsg[0] );

	if ( waitForRxReady( 50 ) == 0 )
		return 0;

	if ( decodeGetPressureResponse( VEMA_ChannelGroup2, pressures + 4 ) == 0 )
		return 0;

	return 1;
}

unsigned int vema_setPressures( const short* pressures )
{
	while( CAN_TxRdy[0] == 0 );
	CAN_TxRdy[0] = 0;
	encodeSetPressuresMessage( VEMA_ChannelGroup1, pressures );
	CAN_wrMsg( 1, &CAN_TxMsg[0] );

	if ( waitForRxReady( 50 ) == 0 )
		return 0;

	if ( decodeSetPressureResponse( VEMA_ChannelGroup1 ) == 0 )
		return 0;

	while( CAN_TxRdy[0] == 0 );
	CAN_TxRdy[0] = 0;
	encodeSetPressuresMessage( VEMA_ChannelGroup2, pressures + 4 );
	CAN_wrMsg( 1, &CAN_TxMsg[0] );

	if ( waitForRxReady( 50 ) == 0 )
		return 0;

	if ( decodeSetPressureResponse( VEMA_ChannelGroup2 ) == 0 )
		return 0;

	return 1;
}
