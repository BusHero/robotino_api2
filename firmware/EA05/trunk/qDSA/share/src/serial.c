#include "../include/serial.h"
#include "../include/qdsaio.h"
#include <qdsa_protocol.h>
#include <stdlib.h>
#include <avr/io.h>

uint8_t g_serial_rts_detect = 0;

void serial_init( void )
{ 
  //115200 baud with 14.7456MHz crystal
  UBRRH = 0;
  UBRRL = 7;
  // Enable UART receiver and transmitter
  UCSRB |= _BV(RXEN);
  UCSRB |= _BV(TXEN);
  // enable the rx complete interrupt
  UCSRB |= _BV(RXCIE);

  /*
  //configure RTS/CTS flow control
  CTS_DDR |= _BV( CTS_BIT );
  CTS_PORT &= ~_BV( CTS_BIT ); //signal is inverted by 16550 -> CTS=high
  */
}

void serial_write( uint8_t c )
{
  loop_until_bit_is_set( UCSRA, UDRE );

  /*if( bit_is_set( RTS_PIN, RTS_BIT ) )
  {
    g_serial_rts_detect++;
  }
  loop_until_bit_is_clear( RTS_PIN, RTS_BIT );*/
  
  UDR = c;
}

void serial_write_string( uint8_t* string )
{
  uint8_t i = 0;
  while( *(string+i) != '\0' )
  {
    serial_write( *(string+i) );
    i++;
  }
}

void serial_write_buffer( uint8_t* buffer, uint8_t length )
{
  uint8_t i;
  for( i=0; i<length; i++ )
  {
    serial_write( *(buffer+i) );
  }
}

uint8_t serial_write_buffer_sfc( uint8_t* buffer, uint8_t length, uint8_t* start, uint8_t start_length, uint8_t* stop, uint8_t stop_length )
{
  uint8_t i;
  uint8_t packet = 0;
  uint8_t ack;

  // disable the rx complete interrupt
  UCSRB &= ~_BV(RXCIE);

  for( i=0; i<start_length; i++ )
  {
    serial_write( *(start+i) );
    packet++;
    if( packet == NUM_BYTES_PER_PACKET )
    {
      packet = 0;
      serial_wait_for_receive();
      ack = serial_read;
      if( ack != ACK_CHAR )
      {
        // enable the rx complete interrupt
        UCSRB |= _BV(RXCIE);
        return 0;
      }
    }
  }

  for( i=0; i<length; i++ )
  {
    serial_write( *(buffer+i) );
    packet++;
    if( packet == NUM_BYTES_PER_PACKET )
    {
      packet = 0;
      serial_wait_for_receive();
      ack = serial_read;
      if( ack != ACK_CHAR )
      {
        // enable the rx complete interrupt
        UCSRB |= _BV(RXCIE);
        return 0;
      }
    }
  }

  for( i=0; i<stop_length; i++ )
  {
    serial_write( *(stop+i) );
    packet++;
    if( packet == NUM_BYTES_PER_PACKET )
    {
      packet = 0;
      serial_wait_for_receive();
      ack = serial_read;
      if( ack != ACK_CHAR )
      {
        // enable the rx complete interrupt
        UCSRB |= _BV(RXCIE);
        return 0;
      }
    }
  }

  // enable the rx complete interrupt
  UCSRB |= _BV(RXCIE);

  return 1;
}

void serial_wait_for_receive( void )
{
  loop_until_bit_is_set( UCSRA, RXC );
}

void serial_write_uint( uint8_t i )
{
  char str[4] = {'\0','\0','\0','\0'};
  utoa( i, str, 10 );
  serial_write_string( str );
}

uint8_t serial_rts_count( void )
{
  return g_serial_rts_detect;
}



