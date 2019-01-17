#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/delay.h>

#include <serial.h>
#include <led.h>

int main(int argc, char **argv) 
{ 
  serial_init();
  led_init();

  sei(); //enable interrupt

  led_blink( 20, 200 );

  //serial_write_string( "Hello\0" );
  
  while( 1 )
  {
  }
}

SIGNAL( SIG_UART_RECV )
{
  led_toggle();
  serial_write( serial_read );
}

