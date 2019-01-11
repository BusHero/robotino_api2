//  Copyright (C) 2004-2009, Robotics Equipment Corporation GmbH

#include "rec/serialport/SerialPortImpl_linux.h"
#include "rec/Timer.h"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h> //for DTR bits
#include <string.h>
#include <sstream>

#include <iostream>
#include <string>
#include <map>

#define INVALID_HANDLE_VALUE -1

using rec::serialport::Port;
using rec::serialport::SerialPortImpl_Linux;
using rec::serialport::SerialPortException;

SerialPortImpl_Linux::SerialPortImpl_Linux()
: _fd( INVALID_HANDLE_VALUE )
, _speed( 0 )
, _port( rec::serialport::UNDEFINED )
, _logCb(NULL)
{
  setReadTimeout(0);
  FD_ZERO(&_inputFdSet);
}

SerialPortImpl_Linux::~SerialPortImpl_Linux()
{
  close();
}

bool SerialPortImpl_Linux::isOpen() const
{
  return (_fd != INVALID_HANDLE_VALUE);
}

void SerialPortImpl_Linux::open( Port port, unsigned int speed, unsigned int readTimeout )
{
	_port = port;
	open( getPortString( port ), speed, readTimeout );
}

void SerialPortImpl_Linux::open( const std::string& port, unsigned int speed, unsigned int readTimeout )
{
	if( isOpen() )
		throw SerialPortException("Port is already open");

	_fd = ::open( port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY );

	if( isOpen() )
	{
		fcntl( _fd, F_SETFL, 0 );
		FD_ZERO(&_inputFdSet);
		FD_SET(_fd, &_inputFdSet);
		configure( speed, readTimeout );
	}
	else
	{
		throw SerialPortException("Couldn't open port");
	}
}

rec::serialport::Port SerialPortImpl_Linux::port() const
{
	return _port;
}

void SerialPortImpl_Linux::close()
{
  if( isOpen() )
  {
    ::close( _fd );
    _fd = INVALID_HANDLE_VALUE;
	FD_ZERO(&_inputFdSet);
	_port = rec::serialport::UNDEFINED;
  }
}


void SerialPortImpl_Linux::setSpeed( unsigned int speed )
{
  struct termios options;
  //Get the current options for the port...
  tcgetattr( _fd, &options);
  //Set the baud rates to speed...
	_speed = speed;
  speed = convSpeed( speed );
  cfsetispeed( &options, speed );
  cfsetospeed( &options, speed );
  tcsetattr( _fd, TCSANOW, &options );
}

unsigned int SerialPortImpl_Linux::speed() const
{
	return _speed;
}

void SerialPortImpl_Linux::setReadTimeout( unsigned int timeout )
{
  struct termios options;
  //Get the current options for the port...
  //tcgetattr( _fd, &options);
  //options.c_cc[VTIME] = timeout / 100;
  //tcsetattr( _fd, TCSANOW, &options );
  
  int usecs = timeout * 1000;
  int secs = 0;
  if(usecs >= 1000000)
  {
	  secs = usecs / 1000000;
	  usecs = usecs - 1000000*secs;
  }
  
  _timeout.tv_sec  = secs;
  _timeout.tv_usec = usecs;
}

int SerialPortImpl_Linux::read( unsigned char* buffer, unsigned int length )
{
	/*
	If MIN = 0 and TIME > 0, TIME serves as a timeout value. The read will be satisfied if a single character is read, or TIME is exceeded (t = TIME *0.1 s). If TIME is exceeded, no character will be returned. 
	If MIN > 0 and TIME > 0, TIME serves as an inter-character timer. The read will be satisfied if MIN characters are received, or the time between two characters exceeds TIME. The timer is restarted every time a character is received and only becomes active after the first character has been received. 
	*/
	
	fd_set input;
	struct timeval tv;
	
	rec::Timer timer;
	
	//std::cout << "Timeout " << _timeout.tv_sec << "s" << "  " << _timeout.tv_usec << "usecs" << std::endl;
	
	int bytes_read = 0;
	int loopCount = 0;
	while( bytes_read < length )
	{
		memcpy(&tv, &_timeout, sizeof(tv));
		memcpy(&input,&_inputFdSet,sizeof(input));
		
		timer.start();
		int n = select(_fd+1, &input,  NULL, NULL, &tv );
		float msecsElapsed = timer.msecsElapsed();
		
		//std::cout << "Select wait time: " << msecsElapsed << "msecs" << std::endl;
	
		/* See if there was an error */
		if (n < 0)
		{
			if(_logCb)
			{
				_logCb("select failed");
			}
			return -1;
		}
		else if (n == 0)
		{
			if(_logCb)
			{
				std::ostringstream os;
				os << "select TIMEOUT after " << msecsElapsed << " msecs";
				_logCb(os.str());
			}
			return bytes_read;
		}
		else
		{
			int res = ::read( _fd, buffer+bytes_read, length-bytes_read );
			if(res < 1)
			{
				return -1;
			}
			bytes_read += res;
			
			if(loopCount > 0 && bytes_read < length)
			{
				if(_logCb)
				{
					std::ostringstream os;
					os << "Read " << res << " bytes. " << length-bytes_read << " bytes missing";
					_logCb(os.str());
				}
			}
		}
		
		++loopCount;
	}

//	int bytes_read = 0;
//	while( bytes_read < length )
//	{
//		int res = ::read( _fd, buffer+bytes_read, length-bytes_read );
//		if( res < 1 )
//		{
			//std::cout << "read less than 1 character" << std::endl;
//			break;
//		}

//		bytes_read += res;
		//std::cout << "read " << bytes_read << " of " << length << " characters" << std::endl;
//	}
	return bytes_read;
}

int SerialPortImpl_Linux::write( const unsigned char* buffer, unsigned int length )
{
  return ::write( _fd, buffer, length );
}

void SerialPortImpl_Linux::flush()
{
  tcflush( _fd, TCIOFLUSH ); 
}

void SerialPortImpl_Linux::configure( unsigned int speed, unsigned int readTimeout )
{
  struct termios options;

  //Get the current options for the port...
  tcgetattr( _fd, &options);

  //Set the baud rates to 115200...

	_speed = speed;
  speed = convSpeed( speed );
  cfsetispeed( &options, speed );
  cfsetospeed( &options, speed );

  //Enable the receiver and set local mode...
  options.c_cflag |= CLOCAL;
  options.c_cflag |= CREAD;
  options.c_cflag &= ~CSIZE;
  options.c_cflag |= CS8;
  //options.c_cflag |=  CRTSCTS;
  options.c_cflag &= ~CRTSCTS;
  options.c_cflag &= ~CSTOPB;
  options.c_cflag &= ~PARENB;
  
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  options.c_iflag &= ~(IXON|IXOFF|IXANY); /* Bye to IXON/IXOFF */
  options.c_iflag &= ~INLCR; //Map NL to CR
  options.c_iflag &= ~ICRNL; //Map CR to NL

  options.c_oflag &= ~OPOST; //Choosing Raw Output

/*
  options.c_oflag &= ~ONLCR; //Map NL to CR-NL
  options.c_oflag &= ~OCRNL; //Map CR to NL
  options.c_oflag |= NL0; //No delay for NLs
  options.c_oflag |= CR0; //No delay for CRs
  options.c_oflag |= TAB0; //No delay for TABs
*/

  options.c_cc[VMIN] = 0;
  options.c_cc[VTIME] = 0; //readTimeout / 100;
  
  setReadTimeout(readTimeout);

  //Set the new options for the port...
  tcsetattr( _fd, TCSANOW, &options );
}

const char* SerialPortImpl_Linux::getPortString(Port port) const
{
  switch(port)
  {
  case COM1:
    return "/dev/ttyS0";
  case COM2:
    return "/dev/ttyS1";
  case COM3:
    return "/dev/ttyS2";
  case COM4:
    return "/dev/ttyS3";
  case USB0:
    return "/dev/ttyUSB0";
	case USB1:
    return "/dev/ttyUSB1";
	case USB2:
    return "/dev/ttyUSB2";
	case USB3:
    return "/dev/ttyUSB3";
	case USB4:
    return "/dev/ttyUSB4";
	case USB5:
    return "/dev/ttyUSB5";
	case USB6:
    return "/dev/ttyUSB6";
	case USB7:
    return "/dev/ttyUSB7";
  default:
    throw SerialPortException("Port not supported by this implementation");
  }
}

unsigned int SerialPortImpl_Linux::convSpeed( unsigned int speed ) const
{
  unsigned int ret = B115200;
  switch( speed )
  {
  case 1200:
    ret = B1200;
    break;
  case 4800:
    ret = B4800;
    break;
  case 9600:
    ret = B9600;
    break;
  case 19200:
    ret = B19200;
    break;
  case 38400:
    ret = B38400;
    break;
  case 57600:
    ret = B57600;
    break;
  case 115200:
    ret = B115200;
    break;
  }
  return ret;
}

void SerialPortImpl_Linux::setDTR()
{
    if ( !isOpen() ) return;

    unsigned int dtr_bit = TIOCM_DTR;
    ioctl(_fd, TIOCMBIS, &dtr_bit);
}

void SerialPortImpl_Linux::clearDTR()
{
    if ( !isOpen() ) return;

    unsigned int dtr_bit = TIOCM_DTR;
    ioctl(_fd, TIOCMBIC, &dtr_bit);
}

void SerialPortImpl_Linux::setLogCb(rec_SerialPort_logCb_t func)
{
	_logCb = func;
}
