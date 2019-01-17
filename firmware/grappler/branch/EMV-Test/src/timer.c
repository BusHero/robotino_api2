#include "timer.h"

#define MCLK  48000000                        /* Master Clock 48 MHz         */
#define TCLK0  1000000                        /* Timer Clock rate 1000/s     */
#define TCNT0 (MCLK/TCLK0/4)                  /* Timer Counts                */
#define TCLK1     1000                        /* Timer Clock rate 1000/s     */
#define TCNT1 (MCLK/TCLK1/4)                  /* Timer Counts                */

#define MAINTIMER 0
#define COMTIMER 1
#define TELNETTIMER 2
#define RX64TIMER 3
#define RX28TIMER 4
#define RX10TIMER 5
#define DYNAMIXELTIMER 6
#define SW0_TIMER 7
#define SW1_TIMER 8

unsigned int milliTimer_ref[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned int microTimer_ref[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

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

void comTimer_start( void )
{
	MSTIMER_START( COMTIMER );
}

unsigned int comTimer_elapsed( void )
{
	MSTIMER_ELASPED( COMTIMER );
}

void telnetTimer_start( void )
{
	MSTIMER_START( TELNETTIMER );
}

unsigned int telnetTimer_elapsed( void )
{
	MSTIMER_ELASPED( TELNETTIMER );
}

void timer_rx64_start( void )
{
	MSTIMER_START( RX64TIMER );
}

unsigned int timer_rx64_elapsed( void )
{
	MSTIMER_ELASPED( RX64TIMER );
}

void timer_rx28_start( void )
{
	MSTIMER_START( RX28TIMER );
}

unsigned int timer_rx28_elapsed( void )
{
	MSTIMER_ELASPED( RX28TIMER );
}

void timer_rx10_start( void )
{
	MSTIMER_START( RX10TIMER );
}

unsigned int timer_rx10_elapsed( void )
{
	MSTIMER_ELASPED( RX10TIMER );
}

void dynamixelTimer_start( void )
{
	MSTIMER_START( DYNAMIXELTIMER );
}

unsigned int dynamixelTimer_elapsed( void )
{
	MSTIMER_ELASPED( DYNAMIXELTIMER );
}

void timer_sw0_start( void )
{
	MSTIMER_START( SW0_TIMER );
}

unsigned int timer_sw0_elapsed( void )
{
	MSTIMER_ELASPED( SW0_TIMER );
}

void timer_sw1_start( void )
{
	MSTIMER_START( SW1_TIMER );
}

unsigned int timer_sw1_elapsed( void )
{
	MSTIMER_ELASPED( SW1_TIMER );
}