//  Copyright (C) 2004-2009, Robotics Equipment Corporation GmbH

#ifndef _REC_SERIALPORT_SERIALPORTIMPL_RTAI_H_
#define _REC_SERIALPORT_SERIALPORTIMPL_RTAI_H_

#include "rec/serialport/SerialPortImpl.h"

#include <sys/mman.h> //Needed for rtai_serial.h
#include <rtai_serial.h>

namespace rec
{
	namespace serialport
	{
		class SerialPortImpl_rtai : public SerialPortImpl
		{
		public:
			SerialPortImpl_rtai();
			virtual ~SerialPortImpl_rtai();

			virtual void open( Port port, unsigned int speed, unsigned int readTimeout );
			virtual void open( const std::string& port, unsigned int speed, unsigned int readTimeout );
			virtual bool isOpen() const;
			virtual void close();

			virtual Port port() const { return _port; }

			virtual void setSpeed( unsigned int speed );
			virtual unsigned int speed() const;
			virtual void setReadTimeout( unsigned int timeout );

			virtual int read( unsigned char* buffer, unsigned int length );
			virtual int write( const unsigned char* buffer, unsigned int length );
			virtual void flush();

			virtual unsigned int getPortID() const;

		protected:
			bool _isOpen;
			Port _port;
			RTIME _readTimeout;
			unsigned int _speed;
		};
	}
}

#endif
