#include "../include/adc.h"

void adc_init( void )
{
  //enable ADC
  ADCSRA |= _BV( ADEN );
  
  //left adjust the output in ADCH and ADCL
  //i.e. ADCH takes bits 2-9 and ADCL takes bits 0-1
  ADMUX |= _BV( ADLAR );

  //enable ADC interrupt
  ADCSRA |= _BV( ADIE );

  if( F_CPU > 8 )
  {
    //devide by 128
    //with 14.7456MHz crystal conversion takes 128/(14.7456MHz)*13 = 0.113ms
    ADCSRA |= _BV( ADPS2 );
    ADCSRA |= _BV( ADPS1 );
    ADCSRA &= ~_BV( ADPS0 );
  }
  else
  {
    //devide by 64
    //with 8MHz crystal conversion takes 64/(8MHz)*13 = 0.104ms
    ADCSRA |= _BV( ADPS2 );
    ADCSRA &= ~_BV( ADPS1 );
    ADCSRA |= _BV( ADPS0 );
  }

  adc_start();
}

void adc_start( void )
{
  ADCSRA |= _BV( ADSC );
}
