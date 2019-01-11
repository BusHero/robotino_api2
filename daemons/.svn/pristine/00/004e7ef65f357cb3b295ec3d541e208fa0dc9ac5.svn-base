#include "Robot.h"
#include <iostream>


Robot::Robot( const QString& address )
: _address( address )
{
}

bool Robot::init()
{
  
  _com.setAddress( _address.toStdString().c_str() );
  try
  {
    _com.connect();
  }
  catch( rec::robotino::com::ComException& e )
  {
    Q_EMIT ( log( e.what() ) );
    return false;
  }
  
  return true;
}

void Robot::on_omnidrv( int x, int y, int phi )
{
  if( !_bumper.value() )
    _omniDrive.setVelocity( x, y, phi );
  else
    _omniDrive.setVelocity( 0, 0, 0 );
}

void Robot::on_stop()
{
  _omniDrive.setVelocity( 0, 0, 0 );
}
