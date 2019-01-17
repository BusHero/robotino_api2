//  Copyright (C) 2004-2009, Robotics Equipment Corporation GmbH

#ifndef _REC_SERIALPORT_SERIALPORTIMPL_LINUX_H_
#define _REC_SERIALPORT_SERIALPORTIMPL_LINUX_H_

#include "rec/serialport/SerialPortImpl.h"
#include <termios.h>
#include <fcntl.h>

namespace rec
{
	namespace serialport
	{
		class SerialPortImpl_Linux : public SerialPortImpl
		{
		public:
			SerialPortImpl_Linux();
			virtual ~SerialPortImpl_Linux();

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

			virtual void configure( unsigned int speed, unsigned int readTimeout );
			virtual const char* getPortString(Port port) const;
			virtual unsigned int convSpeed( unsigned int speed ) const;
			
			void setDTR();
			void clearDTR();

			void setLogCb(rec_SerialPort_logCb_t func);
			
		protected:
			int _fd;
			unsigned int _speed;
			rec::serialport::Port _port;
			struct timeval _timeout;
			fd_set _inputFdSet;
			rec_SerialPort_logCb_t _logCb;
		};
	}
}

#endif //_REC_SERIALPORT_SERIALPORTIMPL_LINUX_H_
