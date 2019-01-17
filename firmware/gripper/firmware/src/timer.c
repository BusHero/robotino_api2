#include "timer.h"
#include "util.h"
#include "clk.h"

#include <string.h>
#include <stdint.h>

#define TCLK0  1000000                  /* Timer Clock rate 10^6/s     */
#define TCNT0 (PCLK/TCLK0)              /* Timer Counts                */

#define TIMER_ELAPSED( NUM, TYPE ) \
	uint32_t currentTime = current_##TYPE(); \
	if( currentTime >= timer_ref[ ( NUM ) ] ) \
	{ \
		return currentTime - timer_ref[ ( NUM )]; \
	} \
	else \
	{ \
		return 0xFFFFFFFF - ( timer_ref[ ( NUM ) ] - currentTime ) + 1; \
	}

#define DEFINE_TIMER_FUNCTIONS( PREFIX, TYPE, STORAGECLASS ) \
	STORAGECLASS void PREFIX##_start( void ) \
	{ \
		TYPE##Timer_start( TYPE##_##PREFIX ); \
	} \
	STORAGECLASS unsigned int PREFIX##_elapsed( void ) \
	{ \
		return TYPE##Timer_elapsed( TYPE##_##PREFIX ); \
	}

typedef enum milli_timer_t_
{	//milli timer
	milli_milliWaitTimer,
	milli_mainTimer,
	milli_ledTimer,
	milli_afterWriteTimer,
	
	NUM_MILLITIMER
} milli_timer_t;

typedef enum micro_timer_t_
{	//microseconds timer
	micro_microWaitTimer,
	
	NUM_MICROTIMER
} micro_timer_t;

static uint32_t timer_ref[NUM_MILLITIMER + NUM_MICROTIMER];

static volatile uint32_t millicounter = 0;
static volatile uint32_t microcounter = 0;

uint32_t current_milli() { return millicounter + ( T0TC / 1000 ); }
uint32_t current_micro() { return microcounter +   T0TC         ; }

void milliTimer_start( milli_timer_t timer ) { timer_ref[timer                 ] = current_milli(); }
void microTimer_start( micro_timer_t timer ) { timer_ref[timer + NUM_MILLITIMER] = current_micro(); }

uint32_t milliTimer_elapsed( milli_timer_t timer )
{
	TIMER_ELAPSED( timer, milli )
}

uint32_t microTimer_elapsed( micro_timer_t timer )
{
	TIMER_ELAPSED( timer + NUM_MILLITIMER, micro )
}

DEFINE_TIMER_FUNCTIONS( milliWaitTimer, milli, )
DEFINE_TIMER_FUNCTIONS( microWaitTimer, micro, )

static void tc0_isr (void) __irq
{	// Interrupt every 1.000.000.000 microseconds to count up
	millicounter += 1000000;
	microcounter += 1000000000;
	T0IR = 1;                                 /* Clear Interrupt Flag */
	VICVectAddr = 0;                          /* Acknowledge Interrupt */
}

void timer_init( void )
{
	memset( timer_ref, 0, ( NUM_MILLITIMER + NUM_MICROTIMER ) * sizeof( uint32_t ) );
	millicounter = 0;
	microcounter = 0;

	T0TCR = 1;								// Enable timer 0
	T0PR = TCNT0 - 1;						// Prescale so that T0TC is increase every micro second
	T0MR0 = 999999999;						// TC0 Match Value.
	T0MCR = 3;								// TC0 Interrupt and Reset on MR0
	
	// Setup Timer Counter 0 Interrupt
	VICVectAddr1 = (unsigned long)tc0_isr;	// TC0 Interrupt -> Vector 1
	VICVectCntl1 = 0x20 | 4;				// TC0 Interrupt -> IRQ Slot 1
	clearBit( VICIntSelect, 4 );			// TC0 interrupt is normal IRQ not FIQ!
	setBit(   VICIntEnable, 4 );			// Enable TC0 Interrupt
}

void timer_done( void )
{
	setBit( VICIntEnClr, 4 );				// Disable TC0 Interrupt
	VICVectCntl1 = 0;						// IRQ slot 1 disabled
	VICVectAddr1 = 0;						// Free Vector 1
	
	T0MCR = 0;								// No action on match
	T0TCR = 0;								// Disable timer

	millicounter = 0;
	microcounter = 0;
}

unsigned int globalTime( void )
{
	return current_milli();
}

void wait_ms( unsigned int msecs )
{
	milliWaitTimer_start();
	while( milliWaitTimer_elapsed() < msecs ) {}
}

void wait_micro( unsigned int microsec )
{
	microWaitTimer_start();
	while( microWaitTimer_elapsed() < microsec ) {}
}

DEFINE_TIMER_FUNCTIONS( mainTimer,  milli, )
DEFINE_TIMER_FUNCTIONS( ledTimer,  milli, )
DEFINE_TIMER_FUNCTIONS( afterWriteTimer,  milli, )

void timer_afterWrite_start( void )
{
	afterWriteTimer_start();
}

unsigned int timer_afterWrite_elapsed( void )
{
	return afterWriteTimer_elapsed();
}

void timer_wait_start( void )
{
	milliWaitTimer_start();
}

unsigned int timer_wait_elapsed( void )
{
	return milliWaitTimer_elapsed();
}

