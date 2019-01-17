#ifndef __IO_H
#define __IO_H

extern void io_init( void );

extern void io_led_cnt( void );

extern void io_led_on( unsigned char lednum );
extern void io_led_off( unsigned char lednum );

#endif  // __IO_H
