#ifndef _CLK_H_
#define _CLK_H_

#define F_OSC	12000000 // Main oscillator's frequency. In most cases 12MHz. If not, please change!

// Don't change the lines below!

#include <LPC214X.H>

#define PLL_M		( ( PLL0CFG & 0x1F ) + 1 )
#define CCLK		( F_OSC * PLL_M )

#define PCLK		( CCLK / APBDIV_DIVISOR[VPBDIV & 0x03 ] )

extern const unsigned char APBDIV_DIVISOR[4];

#endif
