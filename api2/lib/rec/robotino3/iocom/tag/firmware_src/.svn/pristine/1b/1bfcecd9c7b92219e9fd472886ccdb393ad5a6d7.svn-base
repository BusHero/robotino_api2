#include <LPC23xx.H>                    /* LPC23xx definitions                */

void __swi(11) watchdog_feed_swi (void);
void __SWI_11 ( void )
{
	WDFEED = 0xAA;
	WDFEED = 0x55;
}

void watchdog_feed (void)
{
	watchdog_feed_swi();
}
