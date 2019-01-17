/*
Access to Robotino's IO connector
*/

#ifndef _IO_H_
#define	_IO_H_

void io_init( void );

void io_update( void );

/*
for io_din use
extern unsigned char io_din;
the value is updated by a call to io_update()
*/

/**
Read I/O connector analog input
10 bit resolution
@param channel Range is [0;7]
*/
unsigned short io_adc( unsigned char channel );

/**
Set I/O connector DOUT
@param port Range is [0;7]
@param value Range is [0;1]
*/
void io_set_dout( unsigned char port, unsigned char value );

void io_set_dout_all( unsigned char value );

/**
Return the state of DOUT
*/
unsigned char io_dout( unsigned char port );

unsigned char io_din( unsigned char port );

unsigned char io_bumper( void );

#ifndef _IO_NO_EXTERN_
extern unsigned char io_din_buf;
#endif

#endif
