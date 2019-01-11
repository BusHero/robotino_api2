#ifndef _DRIVER_H_
#define _DRIVER_H_

#include <QtCore>
#include <QtGui>

#include "rec/serialport/SerialPort.h"

class Driver : public QThread
{
	Q_OBJECT
	public:
		Driver();
		~Driver();
		
		bool open(const QString& device);
		
	public slots:
		void showImage(const QImage& image);
		
	private:
		void run();
	
		QSemaphore _startSemaphore;
		QMutex _mutex;
		
		unsigned char* _txBuffer;
		rec::serialport::SerialPort _serialPort;
		bool _run;
};

#endif //_DRIVER_H_
