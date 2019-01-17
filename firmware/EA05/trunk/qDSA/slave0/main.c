#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <qdsaio.h>

#include "spislave.h"
#include "motor.h"
#include "pid.h"
#include <adc.h>
#include <ebdmacros.h>

#include <qdsa_protocol.h>

#define TIMEOUT 10000

#define KP_DEAFAULT 234
#define KI_DEAFAULT 5
#define KD_DEAFAULT 0

uint8_t g_rx_full_buffer[NB_START + NUM_BYTES_S2P + NB_STOP];
uint8_t* g_rx_buffer = g_rx_full_buffer + NB_START;
uint8_t g_rx_full_buffer_ptr = 0;

uint8_t g_tx_buffer[NUM_BYTES_S2P];

uint8_t g_start_sequence[NB_START] = {START0, START1, START2};
uint8_t g_stop_sequence[NB_STOP] = {STOP0, STOP1, STOP2};

uint8_t g_adc_channel = 0;
uint8_t g_adc_bit2to9[8] = {0,0,0,0,0,0,0,0};
uint8_t g_adc_bit0to1[8] = {0,0,0,0,0,0,0,0};

uint8_t g_newdata = 0;

uint8_t g_cycle = 0;
uint16_t g_adc_time = 0;
uint32_t g_pid_time = 0;
uint8_t g_pid_time_correction = 0;

int16_t g_dv = 0;
int32_t g_dp = 0;
uint8_t g_mode = 0;

void int0_init( void )
{
  //enable int0
  GICR |= _BV( INT0 );

  //interrupt on rising edge
  MCUCR |= _BV( ISC01 );
  MCUCR |= _BV( ISC00 );
}

uint8_t rx_check( void )
{
  if( g_rx_full_buffer_ptr >= ( NB_START + NUM_BYTES_S2P + NB_STOP - 1 ) )
  {
    g_newdata = 1;
    g_rx_full_buffer_ptr = 0;
    return 0;
  }
  if( g_rx_full_buffer_ptr < NB_START )
  {
    if( g_rx_full_buffer[g_rx_full_buffer_ptr] != g_start_sequence[g_rx_full_buffer_ptr] )
    {
      g_rx_full_buffer_ptr = 0;
      return 0;
    }
  }
  else if ( g_rx_full_buffer_ptr >= ( NB_START + NUM_BYTES_S2P ) )
  {
    if( g_rx_full_buffer[g_rx_full_buffer_ptr] != g_stop_sequence[g_rx_full_buffer_ptr - NB_START - NUM_BYTES_S2P] )
    {
      g_rx_full_buffer_ptr = 0;
      return 0;
    }
  }

  return 1;
}

void decode( void )
{
  uint8_t* p;

  cli();

  dir2int( g_dv, g_rx_buffer[S_DV], ( bit_is_set( g_rx_buffer[S_MISCOUT], DV_DIR ) ? 1 : 0 ) );

  p = (uint8_t*)&g_dp;

  *p++ = g_rx_buffer[S_DP_0];
  *p++ = ((int32_t)g_rx_buffer[S_DP_1] << 8);
  *p++ = ((int32_t)g_rx_buffer[S_DP_2] << 16);
  *p = ((int32_t)g_rx_buffer[S_DP_3] << 24);

  if( 0xff == g_rx_buffer[S_KP] && 0xff == g_rx_buffer[S_KI] && 0xff == g_rx_buffer[S_KD] )
  {
    pid_setKP( KP_DEAFAULT );
    pid_setKI( KI_DEAFAULT );
    pid_setKD( KD_DEAFAULT );
  }
  else
  {
    pid_setKP( g_rx_buffer[S_KP] );
    pid_setKI( g_rx_buffer[S_KI] );
    pid_setKD( g_rx_buffer[S_KD] );
  }
  g_mode = bit_is_set( g_rx_buffer[S_MISCOUT], MODE );
  
  motor_setBrake( bit_is_set( g_rx_buffer[S_DO], S_BRAKE ) == 0 );

  setBit( S_DO0_PORT, S_DO0_BIT, bit_is_set( g_rx_buffer[S_DO], S_DO0 ) );
  setBit( S_DO1_PORT, S_DO1_BIT, bit_is_set( g_rx_buffer[S_DO], S_DO1 ) );
  setBit( S_DO2_PORT, S_DO2_BIT, bit_is_set( g_rx_buffer[S_DO], S_DO2 ) );
  setBit( S_DO3_PORT, S_DO3_BIT, bit_is_set( g_rx_buffer[S_DO], S_DO3 ) );
  setBit( S_R0_PORT, S_R0_BIT, bit_is_set( g_rx_buffer[S_DO], S_R0 ) );

  if( bit_is_set( g_rx_buffer[S_MISCOUT], RESET_POS ) )
  {
    motor_resetPosition();
  }

  if( bit_is_set( g_rx_buffer[S_MISCOUT], RESET_TIME ) )
  {
    g_pid_time = 0;
    g_pid_time_correction = 0;
  }

  if( bit_is_set( g_rx_buffer[S_DO], S_LED1 ) )
  {
    do_off( S_LED1 );
  }
  else
  {
    do_on( S_LED1 );
  }

  sei();
}

void encode( void )
{
  uint8_t i;
  int32_t pos = 0;
  uint8_t* p;

  cli();

  g_tx_buffer[ S_AV ] = motor_actualSpeed();
  setBit( g_tx_buffer[ S_MISCIN ], AV_DIR, motor_actualSpeedDirection() );

  pos = motor_actualPosition();
  p = (uint8_t*)&pos;
  g_tx_buffer[ S_AP_0 ] = *p++;
  g_tx_buffer[ S_AP_1 ] = *p++;
  g_tx_buffer[ S_AP_2 ] = *p++;
  g_tx_buffer[ S_AP_3 ] = *p;
  //setBit( g_tx_buffer[ S_MISCIN ], AP_DIR, motor_actualPositionDirection() );

  encode_di( S_DI, S_DI0 );
  encode_di( S_DI, S_DI1 );
  encode_di( S_DI, S_DI2 );
  encode_di( S_DI, S_DI3 );
  setBit( g_tx_buffer[ S_DI ], S_BUMPER, bit_is_clear(S_BUMPER_PIN,S_BUMPER_BIT) );

  for( i=0; i<8; i++ )
  {
    g_tx_buffer[S_AD0_H+i] = g_adc_bit2to9[i];
    if( i<4 )
    {
      g_tx_buffer[S_AD_L0] = ( g_adc_bit0to1[i] << (2*i) );
    }
    else
    {
      g_tx_buffer[S_AD_L1] = ( g_adc_bit0to1[i] << (2*(i-4)) );
    }
  }

  p = (uint8_t*)&g_pid_time;
  g_tx_buffer[ S_TIME_0 ] = *p++;
  g_tx_buffer[ S_TIME_1 ] = *p++;
  g_tx_buffer[ S_TIME_2 ] = *p++;
  g_tx_buffer[ S_TIME_3 ] = *p;

  sei();
}

int main( void )
{
  uint8_t i;
  uint8_t firstTime = 1;

  g_rx_full_buffer_ptr = 0;

  pid_init();

  do_activate( S_LED0 );
  do_activate( S_LED1 );

  for( i=0; i<10; i++ )
  {
    do_toggle( S_LED0 );
    do_toggle( S_LED1 );
    _delay_loop_2( 60000 );
  }
  do_off( S_LED0 );
  do_on( S_LED1 );

  do_activate( S_DO0 );
  do_activate( S_DO1 );
  do_activate( S_DO2 );
  do_activate( S_DO3 );
  do_activate( S_R0 );

  do_off( S_DO0 );
  do_off( S_DO1 );
  do_off( S_DO2 );
  do_off( S_DO3 );
  do_off( S_R0 );

  encode();

  sei();
  
  spislave_send = g_start_sequence[0];
  spislave_init();

  adc_start();

  while( 1 )
  {
    if( g_newdata > 0 )
    {
      if( firstTime == 1 )
      {
        adc_init();
        motor_init();
      }

      g_adc_time = 0;

      g_newdata = 0;
      decode();
      encode();

      if( firstTime == 1 )
      {
        firstTime = 0;
        int0_init();
      }

      g_cycle++;
      if( g_cycle == 2 )
      {
        g_cycle = 0;
        do_toggle( S_LED0 );
      }
    }
  }

  return 0;
}

ISR( SPI_STC_vect )
{
  if( g_rx_full_buffer_ptr >= ( NB_START + NUM_BYTES_S2P + NB_STOP ) )
  {
    g_rx_full_buffer_ptr = 0;
  }

  if( g_rx_full_buffer_ptr+1 < NB_START )
  {
    spislave_send = g_start_sequence[g_rx_full_buffer_ptr+1];
  }
  else if( g_rx_full_buffer_ptr+1 >= (NB_START + NUM_BYTES_S2P) )
  {
    spislave_send = g_stop_sequence[g_rx_full_buffer_ptr + 1 - NB_START - NUM_BYTES_S2P];
  }
  else
  {
    spislave_send = g_tx_buffer[g_rx_full_buffer_ptr+1 - NB_START];
  }

  g_rx_full_buffer[ g_rx_full_buffer_ptr ] = spislave_read;

  if( rx_check() == 0 )
  {
    spislave_send = g_start_sequence[0];
  }
  else
  {
    g_rx_full_buffer_ptr++;
  }
}

void stopMotor()
{
  pid_init();

  motor_set_DV( 0 );
  motor_setBrake( 1 );
}

void INT0_vect(void) __attribute__((interrupt));
void INT0_vect(void)
//INTERRUPT( SIG_INTERRUPT0 )
{
  //wird mit exakt 900Hz aufgerufen

#ifdef PID_DISABLED
  motor_counter_update();
  motor_set_DV( g_dv );
#else
  int16_t pid_result = 0;

  motor_counter_update();

  g_pid_time++;
  g_pid_time_correction++;
  if( g_pid_time_correction == 9 )
  {
    g_pid_time++;
    g_pid_time_correction = 0;
  }

  if( g_adc_time >= TIMEOUT )
  {
    stopMotor();
    return;
  }

  if( g_mode == 0 )
  {
    pid_result = pid_step( motor_actualVelocity(), g_dv );
  }
  else
  {
    //pid_result = pid_step( motor_actualPosition(), g_dp );
  }
  motor_set_DV( pid_result );
#endif
}

void ADC_vect(void) __attribute__((interrupt));
void ADC_vect(void)
{
  uint8_t i;

  g_adc_time++;
  if( g_adc_time >= TIMEOUT )
  {
    g_adc_time = TIMEOUT;
    stopMotor();
  }

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
