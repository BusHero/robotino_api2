 #ifndef _EEPROM_H_
 #define _EEPROM_H_

#include <RTL.h>

void eeprom_init( void );

void eeprom_write8( U16 address, U8 data );

U8 eeprom_read8( U16 address );

void eeprom_write16( U16 address, U16 data );

U16 eeprom_read16( U16 address );

void eeprom_write32( U16 address, U32 data );

U32 eeprom_read32( U16 address );

U8 eeprom_read_statusRegsiter( void );

void eeprom_write_statusRegister( U8 data );

void eeprom_write_ip_address( U8 ip1, U8 ip2, U8 ip3, U8 ip4 );

void eeprom_read_ip_address( U8* ip1, U8* ip2, U8* ip3, U8* ip4 );

extern unsigned char eeprom_error;
 	  
 #endif //_EEPROM_H_
