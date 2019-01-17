#include "pid.h"
#include <ebdmacros.h>

#define EH_MAX_BUFFER_SIZE 32
int16_t g_eh_buffer[EH_MAX_BUFFER_SIZE];
uint8_t g_eh_index = 0;
int16_t g_eh_sum = 0;

uint8_t g_kp = 0;
uint8_t g_ki = 0;
uint8_t g_kd = 0;

int16_t g_error_previous = 0;

void pid_init(void)
{
  // error history buffer
  uint8_t i;
  for( i=0; i<EH_MAX_BUFFER_SIZE; ++i )
  {
    g_eh_buffer[i] = 0;
  }
  g_eh_sum = 0;
  g_eh_index = 0;
  g_error_previous = 0;
}

void pid_setKP( uint8_t kp )
{
  g_kp = kp;
}

void pid_setKD( uint8_t kd )
{
  g_kd = kd;
}

void pid_setKI( uint8_t ki )
{
  g_ki = ki;
}

void guardedadd_int16( int16_t* target, int16_t value )
{
  if( value > 0 )
  {
    if( *target > INT16_MAX - value )
    {
      *target = INT16_MAX;
    }
    else
    {
      *target += value;
    }
  }
  else
  {
    if( *target < INT16_MIN - value )
    {
      *target = INT16_MIN;
    }
    else
    {
      *target += value;
    }
  }
}

void guardedsub_int16( int16_t* target, int16_t value )
{
  if( value < 0 )
  {
    if( *target > INT16_MAX + value )
    {
      *target = INT16_MAX;
    }
    else
    {
      *target -= value;
    }
  }
  else
  {
    if( *target < INT16_MIN + value )
    {
      *target = INT16_MIN;
    }
    else
    {
      *target -= value;
    }
  }
}

void eh_add( int16_t value )
{
  ++g_eh_index;
  if( g_eh_index >= EH_MAX_BUFFER_SIZE )
  {
    g_eh_index = 0;
  }

  // remove oldest value
  guardedsub_int16( &g_eh_sum, g_eh_buffer[ g_eh_index ] );

  // add new value
  guardedadd_int16( &g_eh_sum, value );
  
  g_eh_buffer[ g_eh_index ] = value;
}

int16_t pid_step( int16_t actual, int16_t desired )
{

  int16_t error = 0;
  int32_t proportional = 0;
  int32_t integral = 0;
  int32_t derivative = 0;
  int32_t gain = 0;

  error = (int16_t)desired - (int16_t)actual;

  proportional = (int32_t)error * ( (int32_t)g_kp  );

  eh_add( error );
  integral = (int32_t)g_eh_sum * ( (int32_t)g_ki );

  derivative = error - g_error_previous;
  derivative = derivative * ( (int32_t)g_kd );
  derivative *= 100;
  
  gain = proportional + integral + derivative;
  gain /= 16;

  gain += desired;

  //limit( gain, INT16_MAX / 2 - 1, INT16_MIN / 2 + 1 );
  limit( gain, 255, -255 );

  return (int16_t)gain;
}