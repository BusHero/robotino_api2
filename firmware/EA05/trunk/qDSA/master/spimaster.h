#ifndef _SPIMASTER_H_
#define _SPIMASTER_H_

#include <stdint.h>

void spimaster_init( void ); 
void spimaster_select( uint8_t slave ); 
uint8_t spimaster_send( uint8_t data );
void spimaster_deselect( void );

#endif

