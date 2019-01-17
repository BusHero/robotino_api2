#ifndef _IO_H_
#define	_IO_H_

extern void io_init( void );
extern void io_update( void );

extern unsigned char io_pressure_sensor_state( void );

extern void io_dout_set_on( unsigned char pin );
extern void io_dout_set_off( unsigned char pin );

extern void io_led_set( unsigned char ledNum, unsigned char on );
extern void io_led_on( unsigned char ledNum );
extern void io_led_off( unsigned char ledNum );
extern void io_led_toggle( unsigned char ledNum );

extern void io_set_compressors_enabled( unsigned char enabled );

#endif
