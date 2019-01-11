#include "Driver.h"
#include "rec/robotino/daemons/Log.h"

extern rec::robotino::daemons::Log* logHandler;

const int messageSize = 769;

void serialport_logCb(const std::string& message)
{
	logHandler->log(message.c_str(),2);
}

Driver::Driver()
: _txBuffer(new unsigned char[messageSize])
{
	memset(_txBuffer,0,sizeof(_txBuffer));
	_txBuffer[0] = 0xAA;
	
	_serialPort.setLogCb(serialport_logCb);
}

Driver::~Driver()
{
	_run = false;
	
	while(false == isFinished())
	{
		wait(100);
		qDebug() << "Waiting Driver finished";
	}
	
	delete [] _txBuffer;
}

bool Driver::open(const QString& device)
{
	_serialPort.open(device.toLatin1().constData(), 115200, 1000);
	
	if( _serialPort.isOpen() )
	{
		_run = true;
		start();
		_startSemaphore.acquire();
		return isRunning();
	}
	else
	{
		return false;
	}
}

void Driver::showImage(const QImage& image)
{
	QMutexLocker lk(&_mutex);
	
	qDebug() << "Image size is " << image.width() << "x" << image.height();
	int pixels = image.width() * image.height();
	
	memset(_txBuffer,0,sizeof(_txBuffer));
	
	for (int i=0; i < pixels && i < messageSize; i++)
	{
		int x = i%image.width();
		int y = i/image.width();
		QColor currentPixel = image.pixel(x,y);
		_txBuffer[i*3+1] = currentPixel.red();
		_txBuffer[i*3+2] = currentPixel.green();
		_txBuffer[i*3+3] = currentPixel.blue();
		//qDebug() << (unsigned int)_txBuffer[i*3+1] << ", "<< (unsigned int)_txBuffer[i*3+2] << ", "<< (unsigned int)_txBuffer[i*3+3] << ";  ";
	}
	
	for(int line=0; line<image.height(); ++line)
	{
		QString str;
		for(int x=0; x<image.width(); ++x)
		{
			int index = 3*(line*image.width()+x)+1;
			
			str += QString(" %1|%2|%3").arg(int(0xFF&_txBuffer[index]),2,16,QChar('0')).arg(int(0xFF&_txBuffer[index+1]),2,16,QChar('0')).arg(int(0xFF&_txBuffer[index+2]),2,16,QChar('0'));
		}
		qDebug() << str;
	}
	
	for(int i=0; i< sizeof(_txBuffer); ++i)
	{
		unsigned char c = _txBuffer[i];
		if( 0xAA == c )
		{
			_txBuffer[i] = c+1;
		}
	}
	
	_txBuffer[0] = 0xAA;
}

void Driver::run()
{
	_startSemaphore.release();
	
	unsigned char buffer[messageSize];
	
	while(_run)
	{	
		{
			QMutexLocker lk(&_mutex);
			memcpy(buffer, _txBuffer, messageSize);
		}
		
		_serialPort.write(buffer, messageSize);
		
		msleep(300);
		
		unsigned char rxBuf[messageSize-1];
		int bytesRead = _serialPort.read(rxBuf, sizeof(rxBuf));
		qDebug() << "Bytes read: " << bytesRead;
			
		if(-1 == bytesRead)
		{
			qDebug() << "Serial line error";
			qApp->exit(1);
			return;
		}
			
		if( sizeof(rxBuf) == bytesRead  )
		{
			if(0 == memcmp(rxBuf,buffer+1,sizeof(rxBuf)) )
			{
				qDebug() << "Verified OK";
			}
		}
	}
}