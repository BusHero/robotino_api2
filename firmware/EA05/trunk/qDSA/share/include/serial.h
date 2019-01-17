#ifndef _SERIAL_H
#define _SERIAL_H

#include <avr/io.h>
#include <stdint.h>

#define serial_read UDR

/**Initialisierung*/
void serial_init( void );

/**Der Lesepuffers*/
uint8_t* serial_readbuffer( void );

/**Sendet c*/
void serial_write( uint8_t c );

void serial_write_uint( uint8_t i );

void serial_write_string( uint8_t* string );

void serial_write_buffer( uint8_t* buffer, uint8_t length );

uint8_t serial_write_buffer_sfc( uint8_t* buffer, uint8_t length, uint8_t* start, uint8_t start_length, uint8_t* stop, uint8_t stop_length );

void serial_wait_for_receive( void );

uint8_t serial_rts_count( void );

#endif
