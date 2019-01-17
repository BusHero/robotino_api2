#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "spimaster.h"
#include <serial.h>
#include <qdsaio.h>
#include <adc.h>
#include <ebdmacros.h>

#include <qdsa_protocol.h>

#define SPI_DELAY 70
#define SPI_START_DELAY 1000
#define POWER_OFF_TIME 20 // sec/1440*256

uint8_t g_rx_full_buffer[NB_START + NUM_BYTES_P2Q + NB_STOP];
uint8_t* g_rx_buffer = g_rx_full_buffer + NB_START;
uint8_t g_rx_full_buffer_ptr = 0;

uint8_t g_tx_buffer[NUM_BYTES_Q2P];

uint8_t g_adc_channel = 0;
uint8_t g_adc_bit2to9[8] = {0,0,0,0,0,0,0,0};
uint8_t g_adc_bit0to1[8] = {0,0,0,0,0,0,0,0};

uint8_t g_start_sequence[NB_START] = {START0, START1, START2};
uint8_t g_stop_sequence[NB_STOP] = {STOP0, STOP1, STOP2};

uint8_t g_time = 0;
uint8_t g_newdata = 0;

void pidclock_init( void )
{ 
  //prescaling 64 result in an 900Hz PWN Signal with a 14.7456MHz clock
  TCCR2 |= _BV( CS22 );
  TCCR2 &= ~_BV( CS21 );
  TCCR2 &= ~_BV( CS20 );

  //fast pwm mode
  TCCR2 |= _BV( WGM21 );
  TCCR2 |= _BV( WGM20 );

  //OC2 is set on compare match and clear on TOP, if OCR2=TOP -> OC2=low
  TCCR2 |= _BV( COM21 );
  TCCR2 |= _BV( COM20 );
  
  //configure OC2 as output
  M_PIDCLOCK_DDR |= _BV( M_PIDCLOCK_BIT );

  OCR2 = 0x10;
}

void power_off_timer_start( void )
{
  OCR1AH = 0xff;
  OCR1AL = 0;
  TIMSK |= _BV( OCIE1A );

  //clk = 14.7456MHz / 1024 = 1440Hz
  TCCR1B |= _BV( CS12 );
  TCCR1B |= _BV( CS10 ); 
}

void power_off_timer_stop( void )
{
  TCCR1B &= ~_BV( CS12 );
  TCCR1B &= ~_BV( CS11 );
  TCCR1B &= ~_BV( CS10 ); 

  TCNT1L = 0;
  TCNT1H = 0;
}

void power_off( void )
{
  cli();
  
  do_on( M_POWERLED );
  do_off( M_POWER );
  while( 1 )
  {
    do_toggle( M_LED0 );
    do_toggle( M_LED1 );
    _delay_loop_2( 0xFFFF );
    _delay_loop_2( 0xFFFF );
  }
}

ISR( TIMER1_COMPA_vect )
{
  power_off();
}

void power_on( void )
{
  uint8_t i;
  
  do_activate( M_POWER );
  do_on( M_POWER );
  do_off( M_POWERLED );

  for( i=0; i<20; i++ )
  {
    do_toggle( M_LED1 );
    _delay_loop_2( 0xFFFF );
  }
  do_off( M_LED1 );
}

void decode( void )
{
  if( bit_is_set( g_rx_buffer[M_DO], POWER ) )
  {
    power_off();
  }
  if( bit_is_set( g_rx_buffer[M_DO], M_LED1 ) )
  {
    do_off( M_LED1 );
  }
  else
  {
    do_on( M_LED1 );
  }
}

void encode( void )
{
  uint8_t i;

  cli();

  for( i=0; i<8; i++ )
  {
    g_tx_buffer[M_AD0_H+i] = g_adc_bit2to9[i];
    if( i<4 )
    {
      g_tx_buffer[M_AD_L0] = ( g_adc_bit0to1[i] << (2*i) );
    }
    else
    {
      g_tx_buffer[M_AD_L1] = ( g_adc_bit0to1[i] << (2*(i-4)) );
    }
  }

  g_tx_buffer[M_TIME] = g_time;

  //firmware version is 1
  setBit( g_tx_buffer[ NUM_BYTES_M2P + S_MISCIN ], FIRMWAREVERSION0, 1 );

  sei();
}

void restartSlave()
{
  uint8_t j;

  for( j=0; j<NB_START+NUM_BYTES_P2S+NB_STOP; j++ )
  {
    spimaster_send( RESTART_CHAR );
    _delay_loop_2( SPI_DELAY );
  }
}

void update( void )
{
  uint8_t i,j,data;

  for( i=0; i<NUM_SLAVES; i++ )
  {
    spimaster_select( i );

    for( j=0; j<NB_START; j++ )
    {
      data = spimaster_send( g_start_sequence[j] );
      if( data != g_start_sequence[j] )
      {
        g_tx_buffer[NUM_BYTES_M2P + (i*NUM_BYTES_S2P) + S_MISCIN] |= _BV( SPIERROR0 );
        restartSlave();
        goto NEXT_SLAVE;
      }
      if( j == 0 )
      {
        _delay_loop_2( SPI_START_DELAY );
      }
      else
      {
        _delay_loop_2( SPI_DELAY );
      }
    }

    for( j=0; j<NUM_BYTES_S2P; j++ )
    {
      if( j < NUM_BYTES_P2S )
      {
        data = spimaster_send( g_rx_buffer[NUM_BYTES_P2M + (i*NUM_BYTES_P2S) + j] );
      }
      else
      {
        data = spimaster_send( 0 );
      }

      g_tx_buffer[NUM_BYTES_M2P + (i*NUM_BYTES_S2P) + j] = data;
      _delay_loop_2( SPI_DELAY );
    }

    for( j=0; j<NB_STOP; j++ )
    {
      data = spimaster_send( g_stop_sequence[j] );
      if( data != g_stop_sequence[j] )
      {
        g_tx_buffer[NUM_BYTES_M2P + (i*NUM_BYTES_S2P) + S_MISCIN] |= _BV( SPIERROR1 );
        restartSlave();
        goto NEXT_SLAVE;
      }
      _delay_loop_2( SPI_DELAY );
    }

NEXT_SLAVE:
    continue;
  }
}

uint8_t rx_check( void )
{
  if( g_rx_full_buffer_ptr < NB_START )
  {
    if( g_rx_full_buffer[g_rx_full_buffer_ptr] != g_start_sequence[g_rx_full_buffer_ptr] )
    {
      g_rx_full_buffer_ptr = 0;
      return 0;
    }
  }
  else if ( g_rx_full_buffer_ptr >= ( NB_START + NUM_BYTES_P2Q ) )
  {
    if( g_rx_full_buffer[g_rx_full_buffer_ptr] != g_stop_sequence[g_rx_full_buffer_ptr - NB_START - NUM_BYTES_P2Q] )
    {
      g_rx_full_buffer_ptr = 0;
      return 0;
    }
  }

  return 1;
}

int main(int argc, char **argv) 
{ 
  uint8_t i;
  uint8_t poweroff_pressed = 0;

  adc_init();
  sei();

  do_activate( M_LED0 );
  do_on( M_LED0 );

  do_activate( M_LED1 );
  do_on( M_LED1 );

  do_activate( M_POWERLED );
  do_on( M_POWERLED );

  for( i=0; i<5; i++ )
  {
    do_toggle( M_LED0 );
    _delay_loop_2( 0xFFFF );
  }
  do_off( M_LED0 );

  if( g_adc_bit2to9[7] > 100 )
  {
    power_on();
  }
  else
  {
    power_off();
  }

  for( i=0; i<NB_START + NUM_BYTES_P2Q + NB_STOP; i++ )
  {
    g_rx_full_buffer[i]=0;
  }

  cli();

  serial_init();
  spimaster_init();

  for( i=0; i<100; i++ )
  {
    update();
    do_toggle( M_LED0 );
    _delay_loop_2( 0xFFFF );
  }
  for( i=0; i<NUM_SLAVES; i++ )
  {
    if( g_tx_buffer[NUM_BYTES_M2P + (i*NUM_BYTES_S2P) + S_MISCIN] & _BV( SPIERROR0 ) )
    {
      //error in spi communication
     do_off( M_POWER ); 
      for( ;; )
      {
        do_off( M_POWERLED );
        _delay_loop_2( 0xFFFF );
        _delay_loop_2( 0xFFFF );
        do_on( M_POWERLED );
        _delay_loop_2( 0xFFFF );
        _delay_loop_2( 0xFFFF );
      }

    }
  }
  
  sei(); //enable interrupt

  pidclock_init();

  while( 1 )
  {
    if( g_adc_bit2to9[7] > 100 )
    {
      do_toggle( M_LED0 );
      if( poweroff_pressed == 0 )
      {
        power_off_timer_start();
        poweroff_pressed = 1;
      }
    }
    else
    {
      power_off_timer_stop();
      poweroff_pressed = 0;
    }
    if( g_newdata == 1 )
    {
      g_newdata = 0;

      for( i=0; i<NUM_BYTES_Q2P; i++ )
      {
        g_tx_buffer[i] = 0;
      }

      decode();
      update();
      encode();

      serial_write_buffer( g_start_sequence, NB_START );
      serial_write_buffer( g_tx_buffer, NUM_BYTES_Q2P );
      serial_write_buffer( g_stop_sequence, NB_STOP );
      //serial_write_buffer_sfc( g_tx_buffer, NUM_BYTES_Q2P, g_start_sequence, NB_START, g_stop_sequence, NB_STOP );

      g_time++;
    }
  }
}

ISR( USART_RX_vect )
//SIGNAL( SIG_UART_RECV )
{
  /*if( g_rx_full_buffer_ptr >= ( NB_START + NUM_BYTES_P2Q + NB_STOP ) )
  {
    g_rx_full_buffer_ptr = 0;
  }*/
  g_rx_full_buffer[ g_rx_full_buffer_ptr ] = serial_read;

  if( rx_check() == 1 )
  {
    g_rx_full_buffer_ptr++;
  }

  if( g_rx_full_buffer_ptr == NB_P2Q_FULL )
  {
    g_newdata = 1;
    g_rx_full_buffer_ptr = 0;
  }
}


void ADC_vect(void) __attribute__((interrupt));
void ADC_vect(void)
//INTERRUPT( SIG_ADC )
{
  uint8_t i;

  cli();
  g_adc_bit0to1[g_adc_channel] = ADCL;
  g_adc_bit2to9[g_adc_channel] = ADCH;
  sei();

  g_adc_channel++;
  if( g_adc_channel > 7 )
  {
    g_adc_channel = 0;
  }

  for( i=0; i<3; i++ )
  {
    if( bit_is_set( g_adc_channel, i ) )
    {
      ADMUX |= _BV( i );
    }
    else
    {
      ADMUX &= ~_BV( i );
    }
  }
  _delay_loop_1( 10 );
  adc_start();
}
