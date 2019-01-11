#include "timer.h"
#include "util.h"
#include "clk.h"
#include "robotino3io.h"
#include "led.h"
#include <stdio.h>

#define led_pattern_size 100
const unsigned char led_sinus_pattern[led_pattern_size]={49,46,43,40,37,34,31,28,25,23,20,18,15,13,11,9,7,6,4,3,2,1,0,0,0,0,0,0,0,1,2,3,4,6,7,9,11,13,15,18,20,23,25,28,31,34,37,40,43,46,50,53,56,59,62,65,68,71,74,76,79,81,84,86,88,90,92,93,95,96,97,98,99,99,99,100,99,99,99,98,97,96,95,93,92,90,88,86,84,81,79,76,74,71,68,65,62,59,56,53};
const unsigned char led_fast_sinus_pattern[led_pattern_size]={50,56,62,68,74,79,84,88,92,95,97,99,99,99,99,97,95,92,88,84,79,74,68,62,56,49,43,37,31,25,20,15,11,7,4,2,0,0,0,0,2,4,7,11,15,20,25,31,37,43,50,56,62,68,74,79,84,88,92,95,97,99,99,99,99,97,95,92,88,84,79,74,68,62,56,50,43,37,31,25,20,15,11,7,4,2,0,0,0,0,2,4,7,11,15,20,25,31,37,43};
const unsigned char led_up_pattern[led_pattern_size]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100};
const unsigned char led_down_pattern[led_pattern_size]={99,98,97,96,95,94,93,92,91,90,89,88,87,86,85,84,83,82,81,80,79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0};
const unsigned char led_rect_pattern[led_pattern_size]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100};
const unsigned char led_fast_rect_pattern[led_pattern_size]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100};
const unsigned char* _led_current_pattern = 0;

//#define MCLK  48000000                        /* Master Clock 48 MHz         */
//#define TCLK0  1000000                        /* Timer Clock rate 10^6/s     */
//#define TCNT0 (MCLK/TCLK0/4)                  /* Timer Counts                */
//#define TCLK1     1000                        /* Timer Clock rate 1000/s     */
//#define TCNT1 (MCLK/TCLK1/4)                  /* Timer Counts                */
//#define TCLK2  1000000                        /* Timer Clock rate 10^6/s     */
//#define TCNT2 (MCLK/TCLK2/4)                  /* Timer Counts                */

#define TCLK0  1000000                        /* Timer Clock rate 10^6/s     */
#define TCNT0 (TIMER0_CLK/TCLK0)              /* Timer Counts                */
#define TCLK1     1000                        /* Timer Clock rate 1000/s     */
#define TCNT1 (TIMER1_CLK/TCLK1)              /* Timer Counts                */
#define TCLK2  1000000                        /* Timer Clock rate 10^6/s     */
#define TCNT2 (TIMER2_CLK/TCLK2)              /* Timer Counts                */
#define TCLK3     1000                        /* Timer Clock rate 1000/s     */
#define TCNT3 (TIMER3_CLK/TCLK3)              /* Timer Counts                */

//ms timer
#define MAINTIMER 0
#define TICKTIMER 1
#define MSTIMER 2
#define GYROTIMER 3
#define CHARGERTIMER 4

//microseconds timer
#define MICROTIMER0 0
#define MICROTIMER1 1
#define MOTORTIMER 2

extern void fpga_update( void );
extern void fpga_toggle_led( unsigned char led );

extern void motor_update( void );
extern void odometry_update( void );

extern void io_cover_pwr_btn_led_set( unsigned int value );

unsigned int milliTimer_ref[5] = { 0, 0, 0, 0, 0 };
unsigned int microTimer_ref[4] = { 0, 0, 0, 0 };

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

volatile unsigned char motor_needs_update = 0;
unsigned int motorTimerElapsed = 0;

void motorTimer_start( void );
unsigned int motorTimer_elapsed( void );

void motor_timer_irq (void) __irq {
	setBit( PWM5_SET, PWM5_BIT );
	fpga_update();
	motor_update();
	setBit( PWM5_CLR, PWM5_BIT );

	T2IR = 1;
	VICVectAddr = 0; /* Acknowledge Interrupt */
}

void power_button_led_irq (void) __irq {
	static unsigned char index = 0;

	if( index >= led_pattern_size )
	{
		index = 0;
	}

	io_cover_pwr_btn_led_set( _led_current_pattern[index] );

	++index;

	T3IR = 1;
	VICVectAddr = 0; /* Acknowledge Interrupt */
}

void led_set_pattern( unsigned char pattern )
{
	switch( pattern )
	{
	case LED_SINUS:
		_led_current_pattern = led_sinus_pattern;
		break;

	case LED_FAST_SINUS:
		_led_current_pattern = led_fast_sinus_pattern;
		break;

	case LED_UP:
		_led_current_pattern = led_up_pattern;
		break;

	case LED_DOWN:
		_led_current_pattern = led_down_pattern;
		break;

	case LED_RECT:
		_led_current_pattern = led_rect_pattern;
		break;

	case LED_FAST_RECT:
		_led_current_pattern = led_fast_rect_pattern;
		break;

	default:
		break;
	}
}

void timer_init( void )
{
	led_set_pattern( LED_UP );

	setBit( PWM5_DIR, PWM5_BIT );

	T0TCR = 1; //enable timer1
	T0PR = TCNT0 - 1; //prescale so that T0TC is increase every micro second

	T1TCR = 1; //enable timer1
	T1PR = TCNT1 - 1; //prescale so that T1TC is increase every milli second


	//************* Motor timer
	T2PR = TCNT2 - 1; //prescale so that T2TC is increase every micro second

	VICVectAddr26 = (unsigned long)motor_timer_irq;
	VICVectCntl26 = 1;       /*1 is highest priority, use it for Timer2 Interrupt VICVectCntl is VICVectPriority in LPC2378*/
	VICIntEnable = 1 << 26;   /* Enable Timer2 Interrupt */

	T2MR0 = 1000;
	setBit2( T2MCR, 0, 1 );
	//************* Motor timer

	//************* power button led timer
	T3PR = TCNT3 - 1; //prescale so that T3TC is increase every milli second

	VICVectAddr27 = (unsigned long)power_button_led_irq;
	VICIntEnable = 1 << 27;   /* Enable Timer3 Interrupt */

	T3MR0 = 20;
	setBit2( T3MCR, 0, 1 );

	T3TCR = 1; //enable timer1
	//************* power button led timer
}

void timer_start_motor_timer( void )
{
	T2TCR = 1; //enable timer2
}

void timer_stop_motor_timer( void )
{
	T2TCR = 0; //disable timer2
}

void mainTimer_start( void )
{
	MSTIMER_START( MAINTIMER );
}

unsigned int mainTimer_elapsed( void )
{
	MSTIMER_ELASPED( MAINTIMER );
}

void tickTimer_start( void )
{
	MSTIMER_START( TICKTIMER );
}

unsigned int tickTimer_elapsed( void )
{
	MSTIMER_ELASPED( TICKTIMER );
}

void msTimer_start( void )
{
	MSTIMER_START( MSTIMER );
}

unsigned int msTimer_elapsed( void )
{
	MSTIMER_ELASPED( MSTIMER );
}

void wait_ms( unsigned int msecs )
{
	msTimer_start();
	while( msTimer_elapsed() < msecs ) {}
}

void gyroTimer_start( void )
{
	MSTIMER_START( GYROTIMER );
}

unsigned int gyroTimer_elapsed( void )
{
	MSTIMER_ELASPED( GYROTIMER );
}

void chargerTimer_start( void )
{
	MSTIMER_START( CHARGERTIMER );
}

unsigned int chargerTimer_elapsed( void )
{
	MSTIMER_ELASPED( CHARGERTIMER );
}

unsigned int globalTime( void )
{
	return T1TC;
}

unsigned int elapsed(unsigned int since)
{
	unsigned int now = globalTime();
	
	if(now >= since)
	{
		return now-since;
	}
	else
	{
		return 0xFFFFFFFF - ( since - now ) + 1;
	}
}

void microTimer_start( void )
{
	MICROTIMER_START( MICROTIMER0 );
}

unsigned int microTimer_elapsed( void )
{
	MICROTIMER_ELASPED( MICROTIMER0 );
}

void wait_micro( unsigned int microsec )
{
	microTimer_start();
	while( microTimer_elapsed() < microsec ){}
}

void microTimer2_start( void )
{
	MICROTIMER_START( MICROTIMER1 );
}

unsigned int microTimer2_elapsed( void )
{
	MICROTIMER_ELASPED( MICROTIMER1 );
}

void wait_micro2( unsigned int microsec )
{
	microTimer2_start();
	while( microTimer2_elapsed() < microsec ){}
}


