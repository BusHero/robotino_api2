//  Copyright (C) 2004-2009, Robotics Equipment Corporation GmbH

#ifndef _REC_SERIALPORT_SERIALPORTIMPL_WIN_H_
#define _REC_SERIALPORT_SERIALPORTIMPL_WIN_H_

#include "rec/serialport/SerialPortImpl.h"
#include <windows.h>

namespace rec
{
	namespace serialport
	{
		class SerialPortImpl_win : public SerialPortImpl
		{
		public:
			SerialPortImpl_win();
			virtual ~SerialPortImpl_win();

			virtual void open( Port port, unsigned int speed, unsigned int readTimeout );
			virtual void open( const std::string& port, unsigned int speed, unsigned int readTimeout );
			virtual Port port() const;
			virtual bool isOpen() const;
			virtual void close();

			virtual void setSpeed( unsigned int speed );
			virtual unsigned int speed() const;
			virtual void setReadTimeout( unsigned int timeout );

			virtual int read( unsigned char* buffer, unsigned int length );
			virtual int write( const unsigned char* buffer, unsigned int length );
			virtual void flush();

			virtual const wchar_t* getPortString(Port port) const;
			virtual void configure( unsigned int speed, unsigned int readTimeout );

		protected:
			void open( const std::wstring& port, unsigned int speed, unsigned int readTimeout );

			HANDLE _fd;
			COMMCONFIG _cc;
			DWORD _size;
		};
	}
}

#endif //_REC_SERIALPORT_SERIALPORTIMPL_WIN_H_
