#ifndef _FPGA_H_
#define _FPGA_H_

void fpga_init( void );
unsigned char fpga_done( void );
void fpga_configure( void );
void fpga_reset( void );

void fpga_toggle_led( unsigned char led );
void fpga_led_on( unsigned char led );
void fpga_led_off( unsigned char led );

void fpga_update( void );

void fpga_motor_velocity( unsigned char motor, signed short* value );
void fpga_motor_position( unsigned char motor, signed int* value );
void fpga_io_din( unsigned char* value );
void fpga_power_status( unsigned char* value );

void fpga_power_on( void );
void fpga_power_off( void );
void fpga_motor_set_position( unsigned char motor, signed int pos );
void fpga_set_io_rel( unsigned char num, unsigned char value );

#endif //_FPGA_H_

