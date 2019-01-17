#ifndef __SPI_H
#define __SPI_H

#define SPI_CS_NONE		0
#define SPI_CS_DAC		1
#define SPI_CS_EEPROM	2

extern void spi_init( void );

extern void spi_chipSelect( unsigned int dev );

extern void spi_write( unsigned int value );
extern void spi_read( unsigned int* value );

#endif  // __SPI_H
