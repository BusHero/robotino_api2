#include "../pid.h"
#include <iostream>
#include <conio.h>

int main( int argc, char** argv )
{
  pid_setKP( 100 );
  pid_setKD( 100 );

  pid_init();
  for( unsigned int i=0; i<10; i++ )
  {
    int res = pid_step( 0, 1000 );
    std::cout << "step " << i << "  " << res << std::endl;
  }
  pid_init();
  pid_setKD( 0 );
  for( unsigned int i=0; i<10; i++ )
  {
    int res = pid_step( 0, 1000 );
    std::cout << "step " << i << "  " << res << std::endl;
  }

  _getch();

  return 0;
}