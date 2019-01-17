#include <lpc23xx.h>
//#include "timer.h"

#define MCLK  48000000                        /* Master Clock 48 MHz         */
#define TCLK0  1000                           /* Timer Clock rate 1000/s     */
#define TCNT0 (MCLK/TCLK0/4)                  /* Timer Counts                */

#define MAINTIMER 0
#define AFTERWRITETIMER 1
#define SW_TIMER 2
#define LED_TIMER 3
#define WAIT_TIMER 4
#define NUM_TIMERS 5

unsigned int milliTimer_ref[NUM_TIMERS] = { 0, 0, 0, 0, 0 };

#define MSTIMER_START( num ) (milliTimer_ref[num]=T0TC)

#define MSTIMER_ELASPED( num )\
unsigned int currentTime = T0TC;\
if( currentTime >= milliTimer_ref[num] )\
{\
	return currentTime - milliTimer_ref[num];\
}\
else\
{\
	return 0xFFFFFFFF - ( milliTimer_ref[num] - currentTime ) + 1;\
}

void timer_init( void )
{
	T0TCR = 1; //enable timer1
	T0PR = TCNT0 - 1; //prescale so that T0TC is increase every milli second
}

void timer_done( void )
{
	T0TCR = 0; //disable timer1
}

void mainTimer_start( void )
{
	MSTIMER_START( MAINTIMER );
}

unsigned int mainTimer_elapsed( void )
{
	MSTIMER_ELASPED( MAINTIMER );
}

void timer_afterWrite_start( void )
{
	MSTIMER_START( AFTERWRITETIMER );
}

unsigned int timer_afterWrite_elapsed( void )
{
	MSTIMER_ELASPED( AFTERWRITETIMER );
}

void timer_sw_start( void )
{
	MSTIMER_START( SW_TIMER );
}

unsigned int timer_sw_elapsed( void )
{
	MSTIMER_ELASPED( SW_TIMER );
}

void timer_led_start( void )
{
	MSTIMER_START( LED_TIMER );
}

unsigned int timer_led_elapsed( void )
{
	MSTIMER_ELASPED( LED_TIMER );
}

void timer_wait_start( void )
{
	MSTIMER_START( WAIT_TIMER );
}

unsigned int timer_wait_elapsed( void )
{
	MSTIMER_ELASPED( WAIT_TIMER );
}

void wait( unsigned int ms )
{
   timer_wait_start();
   while(timer_wait_elapsed()<ms){}; 
}
