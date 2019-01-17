#ifndef _PWM_H_
#define _PWM_H_

void pwm_init( void );

/**
@param channel valid values are 1,2,3,4,5,6
@param ratio The on/off ratio
             0->constant low
			 255->constant high
*/
void pwm_set( unsigned char channel, unsigned char ratio );

/*
Get ration of channel 1,2,3,4,5,6
@return ratio
*/
unsigned char pwm_get( unsigned char channel );

#endif //_PWM_H_
