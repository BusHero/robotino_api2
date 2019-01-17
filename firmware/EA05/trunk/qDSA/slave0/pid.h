#ifndef _PID_H_
#define _PID_H_

#ifdef TEST
#include <xstdint.h>
#else
#include <stdint.h>
#endif

void pid_init(void);
void pid_setDV( uint8_t dv, uint8_t dir );
void pid_setDP( uint32_t dp );
void pid_setKP( uint8_t kp );
void pid_setKD( uint8_t kd );
void pid_setKI( uint8_t ki );
void pid_setMode( uint8_t mode );
int16_t pid_step( int16_t actual, int16_t desired );

#endif