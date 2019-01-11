#ifndef _REC_CRUIZCORE_COMIMPL_H_
#define _REC_CRUIZCORE_COMIMPL_H_

#include <QtCore>

#include "rec/cruizcore/Com.h"
#include "rec/serialport/SerialPort.h"

namespace rec
{
	namespace cruizcore
	{
		class ComImpl : public QThread
		{
		public:
			ComImpl( Com* com );

			~ComImpl();

			bool open( rec::serialport::Port port );

			bool open( const QString& port );

			bool open();

			bool isOpen() const;

			void close();

			void reset();

			QString portString() const;

			rec::serialport::Port port() const;

		private:
			void run();
			void stop();

			bool open_i( rec::serialport::Port port );

			bool parse_data( const unsigned char* data, float* grate, float* gangle ) const;

			void setParameters();

			Com* _com;

			rec::serialport::SerialPort _serialPort;

			bool _run;
			bool _reset;
			unsigned int _sequence;

			QSemaphore _comActiveSemaphore;

#ifdef WIN32
			QString _usbId;
#else
			QString _usbName;
#endif
		};
	}
}

#endif
