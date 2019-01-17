#include "dac.h"
#include "spi.h"

#define LOAD_CMD		0xF0
#define SHUTDOWN_CMD	0xE0
#define RESUME_CMD		0xD0

void dac_load( unsigned int value )
{
	spi_chipSelect( SPI_CS_DAC );
	spi_write( LOAD_CMD | ( ( value >> 6 ) & 0x0F ) );
	spi_write( ( value << 2 ) & 0xFC );
	spi_chipSelect( SPI_CS_NONE );
}

void dac_shutdown( void )
{
	spi_chipSelect( SPI_CS_DAC );
	spi_write( SHUTDOWN_CMD );
	spi_write( 0x00 );
	spi_chipSelect( SPI_CS_NONE );
}

void dac_resume( void )
{
	spi_chipSelect( SPI_CS_DAC );
	spi_write( RESUME_CMD );
	spi_write( 0x00 );
	spi_chipSelect( SPI_CS_NONE );
}
