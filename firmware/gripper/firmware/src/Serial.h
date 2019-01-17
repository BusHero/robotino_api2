#ifndef _SERIAL_H_
#define _SERIAL_H_

extern void serial_init( void );
extern int serial_sendChar( int ch );
extern void serial_sendBuffer( const unsigned char* buf, unsigned int length );
extern void serial_sendString( const char* str );
extern int serial_getKey( void );

#endif
