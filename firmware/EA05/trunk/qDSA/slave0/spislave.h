#ifndef _SPISLAVE_H_
#define _SPISLAVE_H_

#include <avr/io.h>

#define spislave_read SPDR
#define spislave_send SPDR

void spislave_init( void ); 

#endif

