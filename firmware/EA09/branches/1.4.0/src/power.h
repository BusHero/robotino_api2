#ifndef _POWER_H_
#define _POWER_H_

void power_init( void );

unsigned char power_is_pressed( void );

void power_lpc( unsigned char on );

void power_fpga( unsigned char on );

#endif
