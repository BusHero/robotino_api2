//  Copyright (C) 2004-2009, Robotics Equipment Corporation GmbH

#include "rec/serialport/SerialPortImpl_win.h"

#include <Setupapi.h>

#include <iostream>
#include <string>
#include <map>

std::wstring toWString( const std::string& str )
{
	std::wstring res;
	size_t s = ::mbstowcs( NULL, str.c_str(), str.size() );
	res.resize( s );
	::mbstowcs( &res[0], str.c_str(), str.size() );
	return res;
}

using rec::serialport::Port;
using rec::serialport::SerialPortImpl_win;
using rec::serialport::SerialPortException;

const wchar_t* SerialPortImpl_win::getPortString(Port port) const
{
  switch(port)
  {
  case COM1:
    return L"COM1";
  case COM2:
    return L"COM2";
  case COM3:
    return L"COM3";
  case COM4:
    return L"COM4";
	case COM5:
    return L"COM5";
	case COM6:
    return L"COM6";
	case COM7:
    return L"COM7";
	case COM8:
    return L"COM8";
	case COM9:
    return L"COM9";
	case COM10:
    return L"COM10";
	case COM11:
    return L"COM11";
  default:
    throw SerialPortException("Port not supported by this implementation");
  }
}

SerialPortImpl_win::SerialPortImpl_win()
  : _fd( INVALID_HANDLE_VALUE )
{
}

SerialPortImpl_win::~SerialPortImpl_win()
{
  close();
}

bool SerialPortImpl_win::isOpen() const
{
  return (_fd != INVALID_HANDLE_VALUE);
}

void SerialPortImpl_win::open( Port port, unsigned int speed, unsigned int readTimeout )
{
  open( getPortString( port ), speed, readTimeout );
}

void SerialPortImpl_win::open( const std::string& port, unsigned int speed, unsigned int readTimeout )
{
	open( toWString( port ), speed, readTimeout );
}

void SerialPortImpl_win::open( const std::wstring& port, unsigned int speed, unsigned int readTimeout )
{
  if( isOpen() )
    throw SerialPortException( "Port is already open" );

  _fd = ::CreateFile( ( std::wstring( L"\\\\.\\" ) + port ).c_str(), GENERIC_READ|GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
  if( isOpen() )
  {
    _size = sizeof( COMMCONFIG );
    memset( (void*) &_cc, 0, _size );
    if( ! ::GetDefaultCommConfig( port.c_str(), &_cc, &_size ) )
      throw SerialPortException( "GetDefaultCommConfig failed" );
    configure( speed, readTimeout );
  }
  else
    throw SerialPortException( "Couldn't open port" );
}

rec::serialport::Port SerialPortImpl_win::port() const
{
	return rec::serialport::UNDEFINED;
}

void SerialPortImpl_win::close()
{
  if( isOpen() )
  {
    ::CloseHandle( _fd );
    _fd = INVALID_HANDLE_VALUE;
  }
}

void SerialPortImpl_win::setSpeed( unsigned int speed )
{
  _cc.dcb.BaudRate = (DWORD) speed;
  if(!::SetCommConfig( _fd, & _cc, _size ) )
    throw SerialPortException( "Changing serial port speed failed" );
}

unsigned int SerialPortImpl_win::speed() const
{
	return _cc.dcb.BaudRate;
}

void SerialPortImpl_win::setReadTimeout( unsigned int timeout )
{
  BOOL error = FALSE;
  COMMTIMEOUTS to;
  error |= ! ::GetCommTimeouts( _fd, &to );
  to.ReadIntervalTimeout = timeout; // in ms
  to.ReadTotalTimeoutMultiplier = 1;
  to.ReadTotalTimeoutConstant = timeout; // in ms
  to.WriteTotalTimeoutMultiplier = 0;
  to.WriteTotalTimeoutConstant = 0; // in ms
  error |= ! ::SetCommTimeouts( _fd, &to );

  if(error)
    throw SerialPortException( "Changing read timeout failed" );
}

int SerialPortImpl_win::read( unsigned char* buffer, unsigned int length )
{
  DWORD bytesRead;
  if( ::ReadFile( _fd, buffer, length, &bytesRead, NULL ) )
  {
    return (int) bytesRead;
  }
  else
  {
    return -1;
  }
}

int SerialPortImpl_win::write( const unsigned char* buffer, unsigned int length )
{
  DWORD bytesWritten;
  if( ::WriteFile( _fd, buffer, length, &bytesWritten, NULL ) )
  {
    return (int) bytesWritten;
  }
  else
  {
    return -1;
  }
}

void SerialPortImpl_win::flush()
{
  PurgeComm( _fd, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR );
}

void SerialPortImpl_win::configure( unsigned int speed, unsigned int readTimeout )
{
  BOOL error = FALSE;
  ::SetupComm( _fd, 1024, 1024 );
  // set timeouts for read
  //error |= ! ::SetCommMask( _fd, EV_BREAK|EV_ERR|EV_RXCHAR);
  COMMTIMEOUTS to;
  error |= ! ::GetCommTimeouts( _fd, &to );
  to.ReadIntervalTimeout = readTimeout; // in ms
  to.ReadTotalTimeoutMultiplier = 1;
  to.ReadTotalTimeoutConstant = readTimeout; // in ms
  to.WriteTotalTimeoutMultiplier = 0;
  to.WriteTotalTimeoutConstant = 0; // in ms
  error |= ! ::SetCommTimeouts( _fd, &to );
  _cc.dcb.BaudRate = (DWORD) speed;
  _cc.dcb.fRtsControl = 0;
  _cc.dcb.fParity = 0;
  _cc.dcb.fOutxCtsFlow = 0;
  _cc.dcb.fOutxDsrFlow = 0;
  _cc.dcb.fDtrControl = DTR_CONTROL_DISABLE;
  _cc.dcb.fDsrSensitivity = 0;
  _cc.dcb.fTXContinueOnXoff = 0;
  _cc.dcb.fOutX = 0;
  _cc.dcb.fInX = 0;
  _cc.dcb.fErrorChar = 0;
  _cc.dcb.fNull = 0;
  _cc.dcb.fRtsControl = RTS_CONTROL_DISABLE;
  _cc.dcb.fAbortOnError = 0;
  _cc.dcb.ByteSize = 8;
  _cc.dcb.Parity = NOPARITY;
  _cc.dcb.StopBits = ONESTOPBIT;

  error |= ! ::SetCommConfig( _fd, &_cc, _size );

  if(error)
    throw SerialPortException("Configure port failed");
}
