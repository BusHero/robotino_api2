#ifndef _FPGA_H_
#define _FPGA_H_

void fpga_init( void );

void fpga_configure( void );

void fpga_reset( void );

void fpga_power_on( void );

void fpga_power_off( void );

void fpga_led_on( unsigned char led );

void fpga_led_off( unsigned char led );

void fpga_toggle_led( unsigned char led );

void fpga_motor_update( void );

unsigned char fpga_io_din( void );

/**
Set I/O connector REL
@param num Range is [0;1]
@param value Range is [0;1]
*/
void fpga_set_io_rel( unsigned char num, unsigned char value );

unsigned char fpga_io_rel_isOn( unsigned char relay );

/**
Set the port direction of ioa extender
*/
void fpga_set_ioa_dir( unsigned int value );

/**
Set the ioa extender
*/
void fpga_set_ioa( unsigned int value );

/**
Set the port direction of iob extender
*/
void fpga_set_iob_dir( unsigned int value );

/**
Set the iob extender
*/
void fpga_set_iob( unsigned int value );

unsigned char fpga_done( void );

#ifndef _FPGA_NO_EXTERN_
extern signed int fpga_motor_position[4];
extern signed short fpga_motor_velocity[4];
#endif

#endif //_FPGA_H_
