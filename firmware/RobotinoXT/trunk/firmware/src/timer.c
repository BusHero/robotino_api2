#include "timer.h"

#define MCLK  48000000                        /* Master Clock 48 MHz         */
#define TCLK0  1000000                        /* Timer Clock rate 1000/s     */
#define TCNT0 (MCLK/TCLK0/4)                  /* Timer Counts                */
#define TCLK1     1000                        /* Timer Clock rate 1000/s     */
#define TCNT1 (MCLK/TCLK1/4)                  /* Timer Counts                */

#define MAINTIMER 0
#define GETSTATUS_TIMER 1
#define TIMEOUT_TIMER 2
#define CLOSEDLOOP_TIMER 3
#define COMPRESSOR_TIMER 4

unsigned int milliTimer_ref[5] = { 0, 0, 0, 0, 0 };
unsigned int microTimer_ref[5] = { 0, 0, 0, 0, 0 };

#define MSTIMER_START( num ) (milliTimer_ref[num]=T1TC)
#define MICROTIMER_START( num ) (microTimer_ref[num]=T0TC)

#define MSTIMER_ELASPED( num )\
unsigned int currentTime = T1TC;\
if( currentTime >= milliTimer_ref[num] )\
{\
	return currentTime - milliTimer_ref[num];\
}\
else\
{\
	return 0xFFFFFFFF - ( milliTimer_ref[num] - currentTime ) + 1;\
}

#define MICROTIMER_ELASPED( num )\
unsigned int currentTime = T0TC;\
if( currentTime >= microTimer_ref[num] )\
{\
	return currentTime - microTimer_ref[num];\
}\
else\
{\
	return 0xFFFFFFFF - ( microTimer_ref[num] - currentTime ) + 1;\
}

void timer_init( void )
{
	T0TCR = 1; //enable timer1
	T0PR = TCNT0 - 1; //prescale so that T1TC is increase every micro second

	T1TCR = 1; //enable timer1
	T1PR = TCNT1 - 1; //prescale so that T1TC is increase every milli second
}

void mainTimer_start( void )
{
	MSTIMER_START( MAINTIMER );
}

unsigned int mainTimer_elapsed( void )
{
	MSTIMER_ELASPED( MAINTIMER );
}

void getStatusTimer_start( void )
{
	MSTIMER_START( GETSTATUS_TIMER );
}

unsigned int getStatusTimer_elapsed( void )
{
	MSTIMER_ELASPED( GETSTATUS_TIMER );
}

void timeoutTimer_start( void )
{
	MSTIMER_START( TIMEOUT_TIMER );
}

unsigned int timeoutTimer_elapsed( void )
{
	MSTIMER_ELASPED( TIMEOUT_TIMER );
}

void closedLoopTimer_start( void )
{
	MSTIMER_START( CLOSEDLOOP_TIMER );
}

unsigned int closedLoopTimer_elapsed( void )
{
	MSTIMER_ELASPED( CLOSEDLOOP_TIMER );
}

void compressorTimer_start( void )
{
	MSTIMER_START( COMPRESSOR_TIMER );
}

unsigned int compressorTimer_elapsed( void )
{
	MSTIMER_ELASPED( COMPRESSOR_TIMER );
}
