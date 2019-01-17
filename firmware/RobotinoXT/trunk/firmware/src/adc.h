#ifndef _ADC_H_
#define	_ADC_H_

void adc_init( void );

void adc_done( void );

short adc_channel_value( unsigned char channel );

#endif
