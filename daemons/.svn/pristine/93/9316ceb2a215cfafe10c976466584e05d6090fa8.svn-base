#ifndef _MYRPLIDARIMPL_H_
#define _MYRPLIDARIMPL_H_

#include <QtCore>
#include <rec/LaserScanMessage.h>

namespace rp
{
	namespace standalone
	{
		namespace rplidar
		{
			class RPlidarDriver;
		}
	}
}

class MyRPLidarImpl : public QThread
{
	Q_OBJECT
public:
	MyRPLidarImpl();

	void setFieldOfView(const double minimumAngle, const double maximumAngle);
	bool isDeviceOpen() const;
	bool openDevice(const QString& port, unsigned int speed);
	void closeDevice();

signals:
	void scan(const rec::LaserScanMessage&);

private:
	bool checkRPLIDARHealth();
	void cleanup();
	void run();
	
	QString _port;
	
	QSemaphore _startSemaphore;
	rp::standalone::rplidar::RPlidarDriver* _drv;
	bool _run;
};

#endif //_MYRPLIDARIMPL_H_
