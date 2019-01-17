#ifndef _TIMER_H_
#define _TIMER_H_

#define msleep wait_ms
#define usleep wait_micro

extern void timer_init( void );
extern void timer_done( void );

extern unsigned int globalTime( void );

extern void wait_ms( unsigned int msecs );
extern void wait_micro( unsigned int microsec );

extern void mainTimer_start( void );
extern unsigned int mainTimer_elapsed( void );

extern void ledTimer_start( void );
extern unsigned int ledTimer_elapsed( void );

/* For USB bootloader only */
extern void timer_afterWrite_start( void );
extern unsigned int timer_afterWrite_elapsed( void );
extern void timer_wait_start( void );
extern unsigned int timer_wait_elapsed( void );

#endif //_TIMER_H_
