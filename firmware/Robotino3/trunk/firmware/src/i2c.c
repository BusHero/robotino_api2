#include "i2c.h"
#include "clk.h"
#include "util.h"
#include <LPC23XX.H>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define I2CCON_AA		0x04
#define I2CCON_SI		0x08
#define I2CCON_STO		0x10
#define I2CCON_STA		0x20
#define I2CCON_I2EN		0x40

#define I2CSTAT_START		0x08
#define I2CSTAT_RESTART		0x10
#define I2CSTAT_SLA_W_ACK	0x18
#define I2CSTAT_SLA_W_NACK	0x20
#define I2CSTAT_T_DAT_ACK	0x28
#define I2CSTAT_T_DAT_NACK	0x30
#define I2CSTAT_SLA_R_ACK	0x40
#define I2CSTAT_SLA_R_NACK	0x48
#define I2CSTAT_R_DAT_ACK	0x50
#define I2CSTAT_R_DAT_NACK	0x58

#define I2C_READ( adr ) ( ( adr << 1 ) | 1 )
#define I2C_WRITE( adr ) ( adr << 1 )

#define selectSlaveWrite( ch, slaveAddr )	sendByte( ch, I2C_WRITE( slaveAddr ), I2CSTAT_SLA_W_ACK, I2C_SEND_SLA_ADR_FAILED )
#define selectSlaveRead( ch, slaveAddr )	sendByte( ch, I2C_READ( slaveAddr ), I2CSTAT_SLA_R_ACK, I2C_SEND_SLA_ADR_FAILED )
#define write( ch, value )					sendByte( ch, value, I2CSTAT_T_DAT_ACK, I2C_WRITE_FAILED )

#define CHECKED_EXEC( CMD ) \
	ret = CMD; \
	if ( I2C_OK != ret ) \
	{ \
		stop( ch ); \
		return ret; \
	}

static I2C_Status wait_for_SI( unsigned int ch )
{
	unsigned long timeout = 1600000;

	switch( ch )
	{
		case 0:
			I20CONCLR = 8;		//	clear SI starts action
			while( timeout-- )	//	check SI with timeout
			{
				if ( I20CONSET & 8 )
					return I2C_OK;
			}
			break;
		case 1:
			I21CONCLR = 8;		//	clear SI starts action
			while( timeout-- )	//	check SI with timeout
			{
				if ( I21CONSET & 8 )
					return I2C_OK;
			}
			break;
		case 2:
			I22CONCLR = 8;		//	clear SI starts action
			while( timeout-- )	//	check SI with timeout
			{
				if ( I22CONSET & 8 )
					return I2C_OK;
			}
			break;
	}
	printf( "I2C SI timeout\n\r" );
	return I2C_TIMEOUT;
}

static I2C_Status start( unsigned int ch )
{
	I2C_Status ret = I2C_OK;
	switch( ch )
	{
		case 0:
			I20CONCLR = I2CCON_AA;
			I20CONSET = I2CCON_STA | I2CCON_SI;	// Start condition
			ret = wait_for_SI( ch );
			if ( I2C_OK == ret )
			{
				if ( I20STAT == I2CSTAT_START || I20STAT == I2CSTAT_RESTART )
				{
					return I2C_OK;
				}
				ret = I2C_START_FAILED;
				printf( "I20STAT %02x\n\r", I20STAT );
			}
			break;
		case 1:
			I21CONCLR = I2CCON_AA;
			I21CONSET = I2CCON_STA | I2CCON_SI;	// Start condition
			ret = wait_for_SI( ch );
			if ( I2C_OK == ret )
			{
				if ( I21STAT == I2CSTAT_START || I21STAT == I2CSTAT_RESTART )
				{
					return I2C_OK;
				}
				ret = I2C_START_FAILED;
				printf( "I21STAT %02x\n\r", I21STAT );
			}
			break;
		case 2:
			I22CONCLR = I2CCON_AA;
			I22CONSET = I2CCON_STA | I2CCON_SI;	// Start condition
			ret = wait_for_SI( ch );
			if ( I2C_OK == ret )
			{
				if ( I22STAT == I2CSTAT_START || I22STAT == I2CSTAT_RESTART )
				{
					return I2C_OK;
				}
				ret = I2C_START_FAILED;
				printf( "I22STAT %02x\n\r", I22STAT );
			}
			break;
	}
	printf( "I2C start failed\n\r" );
	return ret;
}

static void stop( unsigned int ch )
{
	switch( ch )
	{
		case 0:
			I20CONSET = I2CCON_AA | I2CCON_STO;
			break;
		case 1:
			I21CONSET = I2CCON_AA | I2CCON_STO;
			break;
		case 2:
			I22CONSET = I2CCON_AA | I2CCON_STO;
			break;
	}
}

static I2C_Status sendByte( unsigned int ch, uint8_t byte, uint32_t expectedStatus, I2C_Status errorOnFail )
{
	I2C_Status ret = I2C_OK;
	switch( ch )
	{
		case 0:
			I20DAT = byte;
			I20CONCLR = I2CCON_SI | I2CCON_STA;
			ret = wait_for_SI( ch );
			if ( I2C_OK == ret )
			{
				if ( I20STAT == expectedStatus )
				{
					return I2C_OK;
				}
				ret = errorOnFail;
				printf( "I20STAT %02x\n\r", I20STAT );
			}
			break;
		case 1:
			I21DAT = byte;
			I21CONCLR = I2CCON_SI | I2CCON_STA;
			ret = wait_for_SI( ch );
			if ( I2C_OK == ret )
			{
				if ( I21STAT == expectedStatus )
				{
					return I2C_OK;
				}
				ret = errorOnFail;
				printf( "I21STAT %02x\n\r", I21STAT );
			}
			break;
		case 2:
			I22DAT = byte;
			I22CONCLR = I2CCON_SI | I2CCON_STA;
			ret = wait_for_SI( ch );
			if ( I2C_OK == ret )
			{
				if ( I22STAT == expectedStatus )
				{
					return I2C_OK;
				}
				ret = errorOnFail;
				printf( "I22STAT %02x\n\r", I22STAT );
			}
			break;
	}
	printf( "I2C write failed\n\r" );
	return ret;
}

static I2C_Status read( unsigned int ch, uint8_t* value, bool last )
{
	I2C_Status ret = I2C_OK;
	switch( ch )
	{
		case 0:
			if ( last )
			{
				I20CONCLR = I2CCON_AA;
			}
			else
			{
				I20CONSET = I2CCON_AA;
			}
			I20CONCLR = I2CCON_SI;
			ret = wait_for_SI( ch );
			if ( I2C_OK == ret )
			{
				if ( ( last && ( I20STAT == I2CSTAT_R_DAT_NACK ) ) || ( !last && ( I20STAT == I2CSTAT_R_DAT_ACK ) ) )
				{
					*value = I20DAT;
					return I2C_OK;
				}
				ret = I2C_READ_FAILED;
				printf( "I20STAT %02x\n\r", I20STAT );
			}
			break;
		case 1:
			if ( last )
			{
				I21CONCLR = I2CCON_AA;
			}
			else
			{
				I21CONSET = I2CCON_AA;
			}
			I21CONCLR = I2CCON_SI;
			ret = wait_for_SI( ch );
			if ( I2C_OK == ret )
			{
				if ( ( last && ( I21STAT == I2CSTAT_R_DAT_NACK ) ) || ( !last && ( I21STAT == I2CSTAT_R_DAT_ACK ) ) )
				{
					*value = I21DAT;
					return I2C_OK;
				}
				ret = I2C_READ_FAILED;
				printf( "I21STAT %02x\n\r", I21STAT );
			}
			break;
		case 2:
			if ( last )
			{
				I22CONCLR = I2CCON_AA;
			}
			else
			{
				I22CONSET = I2CCON_AA;
			}
			I22CONCLR = I2CCON_SI;
			ret = wait_for_SI( ch );
			if ( I2C_OK == ret )
			{
				if ( ( last && ( I22STAT == I2CSTAT_R_DAT_NACK ) ) || ( !last && ( I22STAT == I2CSTAT_R_DAT_ACK ) ) )
				{
					*value = I22DAT;
					return I2C_OK;
				}
				ret = I2C_READ_FAILED;
				printf( "I22STAT %02x\n\r", I22STAT );
			}
			break;
	}
	printf( "I2C read failed\n\r" );
	return ret;
}

I2C_Status i2c_init( unsigned int ch, unsigned int freq )
{
	unsigned long cycle;
	if ( freq > 400000 )
		freq = 400000;

	switch( ch )
	{
		case 0:
			PINSEL1 &= 0x03C00000;
			PINSEL1 |= 0x01400000;

			cycle = I2C0_CLK / freq; // Clock setting
			I20SCLL = cycle / 2 + ( cycle % 2 );
			I20SCLH = cycle / 2;

			I20CONCLR = I2CCON_AA | I2CCON_SI | I2CCON_STA | I2CCON_I2EN;
			I20CONSET = I2CCON_I2EN; // Activate I2C
			return I2C_OK;
	
		case 1:
			PINSEL1 |= 0x000003C0;

			cycle = I2C1_CLK / freq; // Clock setting
			I21SCLL = cycle / 2 + ( cycle % 2 );
			I21SCLH = cycle / 2;

			I21CONCLR = I2CCON_AA | I2CCON_SI | I2CCON_STA | I2CCON_I2EN;
			I21CONSET = I2CCON_I2EN; // Activate I2C
			return I2C_OK;

		case 2:
			PINSEL0 &= 0x00F00000;
			PINSEL0 |= 0x00A00000;

			cycle = I2C2_CLK / freq; // Clock setting
			I22SCLL = cycle / 2 + ( cycle % 2 );
			I22SCLH = cycle / 2;

			I22CONCLR = I2CCON_AA | I2CCON_SI | I2CCON_STA | I2CCON_I2EN;
			I22CONSET = I2CCON_I2EN; // Activate I2C
			return I2C_OK;
	}
	return I2C_UNSUPPORTED_CHANNEL;
}

I2C_Status i2c_done( unsigned int ch )
{
	switch( ch )
	{
		case 0:
			I20CONCLR = I2CCON_AA | I2CCON_SI | I2CCON_STA | I2CCON_I2EN;
			return I2C_OK;

		case 1:
			I21CONCLR = I2CCON_AA | I2CCON_SI | I2CCON_STA | I2CCON_I2EN;
			return I2C_OK;

		case 2:
			I22CONCLR = I2CCON_AA | I2CCON_SI | I2CCON_STA | I2CCON_I2EN;
			return I2C_OK;
	}
	return I2C_UNSUPPORTED_CHANNEL;
}

I2C_Status i2c_readBuf( unsigned int ch, unsigned char slaveAddr, unsigned char reg, unsigned char* buffer, unsigned int length )
{
	I2C_Status ret = I2C_OK;
	if ( ch > 2 )
		return I2C_UNSUPPORTED_CHANNEL;

	if ( length == 0 )
		return I2C_OK;
	CHECKED_EXEC( start( ch ) );
	CHECKED_EXEC( selectSlaveWrite( ch, slaveAddr ) );
	CHECKED_EXEC( write( ch, reg ) );
	CHECKED_EXEC( start( ch ) );
	CHECKED_EXEC( selectSlaveRead( ch, slaveAddr ) );
	while( --length )
	{
		CHECKED_EXEC( read( ch, buffer++, false ) );
	}
	CHECKED_EXEC( read( ch, buffer, true ) );
	stop( ch );
	return I2C_OK;
}

I2C_Status i2c_writeBuf( unsigned int ch, unsigned char slaveAddr, unsigned char reg, const unsigned char* buffer, unsigned int length )
{
	I2C_Status ret = I2C_OK;
	if ( ch > 2 )
		return I2C_UNSUPPORTED_CHANNEL;

	if ( length == 0 )
		return I2C_OK;
	CHECKED_EXEC( start( ch ) );
	CHECKED_EXEC( selectSlaveWrite( ch, slaveAddr ) );
	CHECKED_EXEC( write( ch, reg ) );
	while( length-- )
	{
		CHECKED_EXEC( write( ch, *buffer++ ) );
	}
	stop( ch );
	return I2C_OK;
}

I2C_Status i2c_writeByte( unsigned int ch, unsigned char slaveAddr, unsigned char reg, unsigned char value )
{
	I2C_Status ret = I2C_OK;
	if ( ch > 2 )
		return I2C_UNSUPPORTED_CHANNEL;

	CHECKED_EXEC( start( ch ) );
	CHECKED_EXEC( selectSlaveWrite( ch, slaveAddr ) );
	CHECKED_EXEC( write( ch, reg ) );
	CHECKED_EXEC( write( ch, value ) );
	stop( ch );
	return I2C_OK;
}
