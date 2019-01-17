#ifndef __TIMER_H
#define __TIMER_H

#include <stdint.h>

extern void timer_init(void);

extern void mainTimer_start(void);
extern uint32_t mainTimer_elapsed(void);

extern void usleep( uint32_t usecs );
extern void msleep( uint32_t msecs );

#endif  // __TIMER_H
