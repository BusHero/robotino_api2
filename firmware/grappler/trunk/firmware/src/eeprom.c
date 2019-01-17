#include "eeprom.h"
#include "util.h"
#include "spi.h"


#define WREN 0x6
#define WRDI 0x4
#define RDSR 0x5
#define WRSR 0x1
#define READ 0x3
#define WRITE 0x2

#define IPADDRESS    0    //4 bytes
#define NETMASK      4    //4 bytes
#define MACADDRESS   8    //6 bytes
#define AP_MODE      14   //1 byte

unsigned char eeprom_error = 0;

void eeprom_init( void )
{
	//all initialization done in spi_init()
}

U8 eeprom_readStatusRegister( void )
{
	U8 result;

	spi_select_eeprom();
	spi_send8_blocking( RDSR );
	result = spi_send8_blocking( 0 );
	spi_deselect_eeprom();

	return result;
}

void eeprom_writeEnable( void )
{
	spi_select_eeprom();
	spi_send8_blocking( WREN );
	spi_deselect_eeprom();

	//write enable is reset by
	// sending write disable
	// write status register
	// write complete
}

void eeprom_writeDisable( void )
{
	spi_select_eeprom();
	spi_send8_blocking( WRDI );
	spi_deselect_eeprom();
}

void eeprom_write8( U16 address, U8 data )
{
	eeprom_writeEnable();

	spi_select_eeprom();
	spi_send8_blocking( WRITE );
	spi_send16_blocking( address );
	spi_send8_blocking( data );
	spi_deselect_eeprom();
}

U8 eeprom_read8( U16 address )
{
	U8 result;

	spi_select_eeprom();
	spi_send8_blocking( READ );
	spi_send16_blocking( address );
	result = spi_send8_blocking( 0 );
	spi_deselect_eeprom();

	return result;
}

void eeprom_write16( U16 address, U16 data )
{
	eeprom_writeEnable();

	spi_select_eeprom();
	spi_send8_blocking( WRITE );
	spi_send16_blocking( address );
	spi_send16_blocking( data );
	spi_deselect_eeprom();
}

U16 eeprom_read16( U16 address )
{
	U16 result;

	spi_select_eeprom();
	spi_send8_blocking( READ );
	spi_send16_blocking( address );
	result = spi_send16_blocking( 0 );
	spi_deselect_eeprom();

	return result;
}

void eeprom_write32( U16 address, U32 data )
{
	eeprom_writeEnable();

	spi_select_eeprom();
	spi_send8_blocking( WRITE );
	spi_send16_blocking( address );
	spi_send32_blocking( data );
	spi_deselect_eeprom();
}

U32 eeprom_read32( U16 address )
{
	U32 result;

	spi_select_eeprom();
	spi_send8_blocking( READ );
	spi_send16_blocking( address );
	result = spi_send32_blocking( 0 );
	spi_deselect_eeprom();

	return result;
}

U8 eeprom_read_statusRegsiter( void )
{
	U8 result;

	spi_select_eeprom();
	spi_send8_blocking( RDSR );
	result = spi_send8_blocking( 0 );
	spi_deselect_eeprom();

	return result;
}

void eeprom_write_statusRegister( U8 data )
{
	eeprom_writeEnable();

	spi_select_eeprom();
	spi_send8_blocking( WRSR );
	spi_send8_blocking( data );
	spi_deselect_eeprom();
}

void eeprom_write_ip_address( U8 ip1, U8 ip2, U8 ip3, U8 ip4 )
{
	eeprom_write32( IPADDRESS, (ip1 << 24 ) | (ip2 << 16) | (ip3 << 8) | ip4 );
}

void eeprom_read_ip_address( U8* ip1, U8* ip2, U8* ip3, U8* ip4 )
{
	U32 result = eeprom_read32( IPADDRESS );
	*ip1 = ( (result >> 24) & 0xFF );
	*ip2 = ( (result >> 16) & 0xFF );
	*ip3 = ( (result >> 8) & 0xFF );
	*ip4 = ( result & 0xFF );
}
