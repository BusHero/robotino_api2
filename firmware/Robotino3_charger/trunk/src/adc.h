#ifndef __ADC_H
#define __ADC_H

#define ADC_CH_IVINMON	0
#define ADC_CH_ISMON	1
#define ADC_CH_U_AKKU	2
#define ADC_CH_NTC1		3
#define ADC_CH_NTC2		4
#define ADC_CH_VAREF	5

extern void adc_init( void );
extern void adc_update( void );
extern float adc_get_value( unsigned int channel );
extern unsigned short adc_get_raw_value( unsigned int channel );

#endif  // __ADC_H
