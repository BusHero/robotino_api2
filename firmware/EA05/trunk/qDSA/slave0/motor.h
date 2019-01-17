#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <stdint.h>
#include <avr/io.h>

void motor_init( void );
void motor_set_DV( int16_t velocity );
void motor_setDirection( uint8_t dir );
void motor_setSpeed( uint8_t speed );
void motor_setBrake( uint8_t on );
void motor_counter_update( void );
uint8_t motor_actualSpeed( void );
uint8_t motor_actualSpeedDirection( void );
int16_t motor_actualVelocity( void );
int32_t motor_actualPosition( void );
//uint8_t motor_actualPositionDirection( void );
void motor_resetPosition( void );

#endif
