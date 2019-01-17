#include "motor.h"
#include <avr/interrupt.h>
#include <qdsaio.h>
#include <ebdmacros.h>

#define DIR_PORT PORTB
#define DIR_DDR DDRB
#define DIR_BIT 3

#define BRAKE_PORT PORTB
#define BRAKE_DDR DDRB
#define BRAKE_BIT 2

#define UP_PORT PORTB
#define UP_DDR DDRB
#define UP_BIT 1

#define DOWN_PORT PORTB
#define DOWN_DDR DDRB
#define DOWN_BIT 0

#define PWM_PIN PIND
#define PWM_BIT 7

uint16_t g_motor_up_previous = 0;
uint8_t g_motor_down_previous = 0;
int32_t g_motor_actualposition = 0;
//uint8_t g_motor_actualposition_dir = 0;
uint8_t g_motor_actualspeed = 0;
uint8_t g_motor_actualspeed_dir = 0;

uint8_t g_motor_brake = 0;

void motor_resetPosition( void )
{
  g_motor_actualposition = 0;
}

void motor_counter_update( void )
{
  uint16_t up;
  uint8_t down;
  uint8_t up_ticks = 0;
  uint8_t down_ticks = 0;

  cli();
  up  = TCNT1L;
  up |= ( TCNT1H << 8 );
  down = TCNT0;
  sei();

  if( g_motor_up_previous > up )
  {
    //overrun
    up_ticks = (uint8_t)( 0xFFFF - ( g_motor_up_previous - up ) );
  }
  else
  {
    up_ticks = (uint8_t)( up - g_motor_up_previous );
  }
  g_motor_up_previous = up;

  if( g_motor_down_previous > down )
  {
    //overrun
    down_ticks = (uint8_t)( 0xFF - ( g_motor_down_previous - down ) ); 
  }
  else
  {
    down_ticks = (uint8_t)((uint16_t)down - g_motor_down_previous);
  }
  g_motor_down_previous = down;

  if( up_ticks > down_ticks )
  {
    g_motor_actualspeed_dir = 1;
    g_motor_actualspeed = up_ticks - down_ticks;
  }
  else
  {
    g_motor_actualspeed_dir = 0;
    g_motor_actualspeed = down_ticks - up_ticks;
  }

  /*if( g_motor_actualspeed_dir == g_motor_actualposition_dir )
  {
    g_motor_actualposition += g_motor_actualspeed;
  }
  else
  {
    if( g_motor_actualposition >= g_motor_actualspeed )
    {
      g_motor_actualposition -= g_motor_actualspeed;
    }
    else
    {
      g_motor_actualposition_dir = ( g_motor_actualposition_dir ? 0 : 1 );
      g_motor_actualposition = g_motor_actualspeed - g_motor_actualposition;
    }
  }*/

  if( g_motor_actualspeed_dir == 1 )
  {
    g_motor_actualposition += g_motor_actualspeed;
  }
  else
  {
    g_motor_actualposition -= g_motor_actualspeed;
  }
}

void motor_pwm_init( void ) 
{
  //no prescaling
  TCCR2 &= ~_BV( CS22 );
  TCCR2 &= ~_BV( CS21 );
  TCCR2 |= _BV( CS20 );

  //fast pwm mode
  TCCR2 |= _BV( WGM21 );
  TCCR2 |= _BV( WGM20 );

  //OC2 is set on compare match and clear on TOP, if OCR2=TOP -> OC2=low
  TCCR2 |= _BV( COM21 );
  TCCR2 |= _BV( COM20 );
  
  //DDD7 is also OC2, it must be configured as output
  DDRD |= _BV( DDD7 );
}

void motor_bridge_init( void ) 
{
  //output for motor direction
  DIR_DDR |= _BV( DIR_BIT );

  //output for brake
  BRAKE_DDR |= _BV( BRAKE_BIT );
}

void motor_counter_init( void ) 
{
  UP_PORT |= _BV( UP_BIT ); // enable pullup resistor
  UP_DDR &= ~_BV( UP_BIT ); // configure port as input

  //Clock-select: Count external Pin T1 (PB1), falling edge
  TCCR1B |= _BV( CS12 );
  TCCR1B |= _BV( CS11 );
  TCCR1B &= ~_BV( CS10 );

  DOWN_PORT |= _BV( DOWN_BIT );
  DOWN_DDR &= ~_BV( DOWN_BIT );

  //Clock-select: Count external Pin T0 (PB0), falling edge
  TCCR0 |= _BV( CS02 );
  TCCR0 |= _BV( CS01 );
  TCCR0 &= ~_BV( CS00 );
}

void motor_setDirection( uint8_t dir )
{
  if( dir )
  {
    DIR_PORT |= _BV( DIR_BIT );
  }
  else
  {
    DIR_PORT &= ~_BV( DIR_BIT );
  }
}

void motor_setSpeed( uint8_t speed )
{
  uint8_t inverted_speed;

  if( g_motor_brake )
  {
    return;
  }

  inverted_speed = 0xFF - speed;

  //wait until OCR2 can be written
  loop_until_bit_is_clear( ASSR , OCR2UB );
  
  if( inverted_speed < 0xFF )
  {
    //set speed
    BRAKE_PORT &= ~_BV( BRAKE_BIT );
    OCR2 = inverted_speed; // set output compare value for PWM
  }
  else
  {
    //free run
    OCR2 = 0xFF;
    loop_until_bit_is_clear( PWM_PIN, PWM_BIT );
    BRAKE_PORT |= _BV( BRAKE_BIT );
  }
}

void motor_set_DV( int16_t velocity )
{
  limit( velocity, 255, -255 );
  if( velocity > 0 )
  {
    motor_setDirection( 1 );
    motor_setSpeed( (uint8_t) velocity );
  }
  else
  {
    motor_setDirection( 0 );
    velocity = -velocity;
    motor_setSpeed( (uint8_t) velocity );
  }
}

void motor_setBrake( uint8_t on )
{
  g_motor_brake = on;

  if( on )
  {
    //wait until OCR2 can be written
    loop_until_bit_is_clear( ASSR , OCR2UB );
    OCR2 = 0xFF;
    loop_until_bit_is_clear( PWM_PIN, PWM_BIT );
    BRAKE_PORT &= ~_BV( BRAKE_BIT );
  }
}

int16_t motor_actualVelocity( void )
{
  int16_t av;
  dir2int( av, g_motor_actualspeed, ( g_motor_actualspeed_dir ? 0 : 1 ) );
  return av;
}

uint8_t motor_actualSpeed( void )
{
  return g_motor_actualspeed;
}

uint8_t motor_actualSpeedDirection( void )
{
  return ( g_motor_actualspeed_dir ? 0 : 1 );
}

int32_t motor_actualPosition( void )
{
  return g_motor_actualposition;
}

/*uint8_t motor_actualPositionDirection( void )
{
  return g_motor_actualposition_dir;
}*/

void motor_init( void ) 
{
  motor_bridge_init();
  motor_pwm_init();
  motor_counter_init();
  motor_setDirection( 1 );
  motor_setSpeed( 0 );
}

