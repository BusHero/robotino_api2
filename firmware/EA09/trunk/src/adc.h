#ifndef _ADC_H_
#define	_ADC_H_

void adc_init( void );

void adc_update( void );

/**
10 bit resolution
*/
unsigned short adc_motor_current( unsigned char motor );

/**
Read distance sensors
10 bit resolution
@param sensor Range is [0;8]
*/
unsigned short adc_distance( unsigned char sensor );

/**
10 bit resolution
*/
unsigned short adc_current( void );

/**
10 bit resolution EA05 compatible
*/
unsigned short adc_current_ea05( void );

/**
10 bit resolution
*/
unsigned short adc_battery( void );

#ifndef _ADC_NO_EXTERN_
extern unsigned short adc0_value[16];
extern unsigned short adc1_value[16];
#endif

#endif //_ADC_H_
