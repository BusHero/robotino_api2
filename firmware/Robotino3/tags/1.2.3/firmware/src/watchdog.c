#include <LPC23xx.H>                    /* LPC23xx definitions                */

void watchdog_feed( void )
{
	WDFEED = 0xAA;
	WDFEED = 0x55;
}
