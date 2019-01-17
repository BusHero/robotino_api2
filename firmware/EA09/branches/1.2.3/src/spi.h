#ifndef _SPI_H_
#define _SPI_H_

void spi_init( void );

void spi_select_adc0( void );
void spi_deselect_adc0( void );

void spi_select_adc1( void );
void spi_deselect_adc1( void );

void spi_select_eeprom( void );
void spi_deselect_eeprom( void );

unsigned char spi_send8_blocking( unsigned char data );

unsigned short spi_send16_blocking( unsigned short data );

unsigned int spi_send32_blocking( unsigned int data );

#endif
