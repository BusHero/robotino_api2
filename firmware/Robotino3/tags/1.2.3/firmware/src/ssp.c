#include "ssp.h"
#include "timer.h"
#include "util.h"
#include "clk.h"
#include <stdio.h>
#include <LPC23XX.H>
#include "robotino3io.h"

#define GYRO_CS_LOW  setBit( MPU6000_CS_CLR, MPU6000_CS_BIT );
#define GYRO_CS_HIGH setBit( MPU6000_CS_SET, MPU6000_CS_BIT );

#define SPI_WRITEREG( x )	x
#define SPI_READREG( x )	( 0x80 | x )

extern void wait_micro( unsigned int microsec );

static unsigned char ssp0_send8_blocking( unsigned int ch, const unsigned char data )
{
	unsigned char result;

	SSP0DR = data;

	while( bitIsSet( SSP0SR, 4 ) )
	{
	}
	while( bitIsClear( SSP0SR, 2 ) )
	{
	}

	result = ( SSP0DR & 0xFF );

	return result;
}

static unsigned char ssp1_send8_blocking( unsigned int ch, const unsigned char data )
{
	unsigned char result;

	SSP1DR = data;

	while( bitIsSet( SSP1SR, 4 ) )
	{
	}
	while( bitIsClear( SSP1SR, 2 ) )
	{
	}

	result = ( SSP1DR & 0xFF );

	return result;
}

SSP_Status ssp_init( unsigned int ch, SSP_DSS dss, SSP_FRF frf, SSP_CPOL cpol, SSP_CPHA cpha, unsigned int frequency )
{
#ifndef MCB2300
	setBit( MPU6000_CS_DIR, MPU6000_CS_BIT );

	GYRO_CS_HIGH;

	if ( dss < SSP_DSS_4Bit )
		dss = SSP_DSS_4Bit;
	if ( dss > SSP_DSS_16Bit )
		dss = SSP_DSS_16Bit;

	switch( ch )
	{
		case 0:
		{
			//SPI clock is 48MHz / 8 = 6MHz
			SSP0CPSR = 8;

			//P0.15 is SCK
			clearBit( PINSEL0, 30 );
			setBit  ( PINSEL0, 31 );

			//P0.17 is MISO
			clearBit( PINSEL1, 2 );
			setBit  ( PINSEL1, 3 );

			//P0.18 is MOSI
			clearBit( PINSEL1, 4 );
			setBit  ( PINSEL1, 5 );

			/*
			SSP0CR0
			Bit  Description
			3:0  The SPI controller sends and receives 8 bits.
			5:4  Frame format
			6    CPOL
			7    CPHA
			15:8 Serial Clock Rate
			*/

			SSP0CR0 = dss | frf | cpol | cpha | ( ( SSP0_CLK / 8 / frequency - 1 ) << 8 );

			/*
			SSP0CR1
			Bit   Value  Description
			0     0	   Normal operation.
			1     1    SSP enabled.
			2     0	   Master mode.
			3     0    No Slave Output Disable.
			*/

			setBit( SSP0CR1, 1 );
			return SSP_OK;
		}
		case 1:
		{
			//SPI clock is 48MHz / 8 = 6MHz
			SSP1CPSR = 8;

			//P0.7 is SCK
			clearBit( PINSEL0, 14 );
			setBit  ( PINSEL0, 15 );

			//P0.8 is MISO
			clearBit( PINSEL0, 16 );
			setBit  ( PINSEL0, 17 );

			//P0.9 is MOSI
			clearBit( PINSEL0, 18 );
			setBit  ( PINSEL0, 19 );

			/*
			SSP1CR0
			Bit  Description
			3:0  The SPI controller sends and receives 8 bits.
			5:4  Frame format
			6    CPOL
			7    CPHA
			15:8 Serial Clock Rate
			*/

			SSP1CR0 = dss | frf | cpol | cpha | ( ( SSP1_CLK / 8 / frequency - 1 ) << 8 );

			/*
			SSP1CR1
			Bit   Value  Description
			0     0	   Normal operation.
			1     1    SSP enabled.
			2     0	   Master mode.
			3     0    No Slave Output Disable.
			*/

			setBit( SSP1CR1, 1 );
			return SSP_OK;
		}
	}
#endif
	return SSP_UNSUPPORTED_CHANNEL;
}

SSP_Status ssp_done( unsigned int ch )
{
	switch( ch )
	{
		case 0:
		{
			clearBit( SSP0CR1, 1 );
			return SSP_OK;
		}
		case 1:
		{
			clearBit( SSP1CR1, 1 );
			return SSP_OK;
		}
	}
	return SSP_UNSUPPORTED_CHANNEL;
}

SSP_Status ssp_readBuf( unsigned int ch, unsigned char reg, unsigned char* buffer, unsigned int length )
{
	switch( ch )
	{
		case 0:
		{
			ssp0_send8_blocking( ch, SPI_READREG( reg ) );
			while( length-- )
			{
				*buffer++ = ssp0_send8_blocking( ch, 0 );
			}
			return SSP_OK;
		}
		case 1:
		{
			ssp1_send8_blocking( ch, SPI_READREG( reg ) );
			while( length-- )
			{
				*buffer++ = ssp1_send8_blocking( ch, 0 );
			}
			return SSP_OK;
		}
	}
	return SSP_UNSUPPORTED_CHANNEL;
}

SSP_Status ssp_writeBuf( unsigned int ch, unsigned char reg, const unsigned char* buffer, unsigned int length )
{
	switch( ch )
	{
		case 0:
		{
			ssp0_send8_blocking( ch, SPI_WRITEREG( reg ) );
			while( length-- )
			{
				ssp0_send8_blocking( ch, *buffer++ );
			}
			return SSP_OK;
		}
		case 1:
		{
			ssp1_send8_blocking( ch, SPI_WRITEREG( reg ) );
			while( length-- )
			{
				ssp1_send8_blocking( ch, *buffer++ );
			}
			return SSP_OK;
		}
	}
	return SSP_UNSUPPORTED_CHANNEL;
}

void ssp_select_gyro( void )
{
	GYRO_CS_LOW;
}

void ssp_deselect_gyro( void )
{
	GYRO_CS_HIGH;
	usleep( 10 );
}
