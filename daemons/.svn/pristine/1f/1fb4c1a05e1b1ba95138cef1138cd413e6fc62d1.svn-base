#include <iostream>
#include <sstream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Joystick.h"

bool running = true;

Joystick::Joystick()
{
  _joystick_fd = -1;
}

Joystick::~Joystick()
{
  closeJoystick();
}

bool Joystick::init()
{
  start();
  return true;
}
void Joystick::run()
{
  struct xbox_js_event xjse;
  openJoystick();
  while( running && ( getJoystickStatus( &xjse ) != -1) )
  {
    if( ( xjse.stick_left_x > -20000 && xjse.stick_left_x < 20000 ) &&
        ( xjse.stick_left_y > -20000 && xjse.stick_left_y < 20000 ) &&
        ( xjse.stick_right_x > -20000 && xjse.stick_right_x < 20000 ) )
    {
      Q_EMIT( stop() );
    }
    else
    {
      float acc = ( xjse.lt + 32769 ) / 10000;
      if( acc < 1 )
        acc = 1;
      int y = -1 * 0.006 * xjse.stick_left_y * acc;
      int x = -1 * 0.006 * xjse.stick_left_x * acc;
      int phi = -1 * 0.0012 * xjse.stick_right_x * acc;
      Q_EMIT( omnidrv( x, y, phi) );
    }
    usleep(10000);
  }
}

int Joystick::openJoystick()
{
  while( _joystick_fd < 0 )
  {
    _joystick_fd = open( JOYSTICK_DEVNAME, O_RDONLY | O_NONBLOCK );
    if( _joystick_fd < 0 )
    {
      Q_EMIT( log( "Error in opening joystick!" ) );
      //return _joystick_fd;
    }
    usleep( 10000 ); 
  }

  return _joystick_fd;
}

void Joystick::closeJoystick()
{
  close( _joystick_fd );
}

int Joystick::readJoystickEvent( struct js_event *jse )
{
  int noofbytes = -1;
  noofbytes = read( _joystick_fd, jse, sizeof( *jse ) );

  if( noofbytes == -1 )
  {
    return 0;
  }
  else if( noofbytes == sizeof( *jse ) )
  {
    return 1;
  }
  else
  {
    Q_EMIT( log( QString( "Unexpected bytes read from joystick: %1" ).arg( noofbytes ) ) );
    return -1;
  }
  return -1;
}

int Joystick::getJoystickStatus( struct xbox_js_event *xjse )
{
  int rc;
  struct js_event jse;
  if( _joystick_fd < 0 )
  {
    Q_EMIT( log( "Invalid file descriptor." ) );
    return -1;
  }

  while( ( rc = readJoystickEvent( &jse ) ) == 1 )
  {
    jse.type &= ~JS_EVENT_INIT; /* ignore synthetic events */
    if( jse.type == JS_EVENT_AXIS )
    {
      switch ( jse.number )
      {
      case 0: xjse->stick_left_y = jse.value;
              break;
      case 1: xjse->stick_left_x = jse.value;
              break;
      case 2: xjse->lt = jse.value;
              break;
      case 3: xjse->stick_right_x = jse.value;
              break;
      case 4: xjse->stick_right_y = jse.value;
              break;
      case 5: xjse->rt = jse.value;
              break;
      default:
              break;
      }
    }
    else if( jse.type == JS_EVENT_BUTTON )
    {
      if( jse.number < 15 )
      {
        switch( jse.value )
        {
          case 0: xjse->button[jse.number] = jse.value;
                  break;
          case 1: xjse->button[jse.number] = jse.value;
                  break;
          default:
                  break;
        }
      }
    }
  }
}
