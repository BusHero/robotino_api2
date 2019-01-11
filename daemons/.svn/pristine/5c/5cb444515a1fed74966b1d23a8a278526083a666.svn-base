#include "MyRPLidarImpl.h"
#include <rplidar.h>
#include "rec/robotino/daemons/Log.h"

using namespace rp::standalone::rplidar;

MyRPLidarImpl::MyRPLidarImpl()
: _drv(NULL)
, _run(false)
{
	
}

void MyRPLidarImpl::setFieldOfView(const double minimumAngle, const double maximumAngle)
{

}

bool MyRPLidarImpl::isDeviceOpen() const
{
	return (NULL != _drv);
}

bool MyRPLidarImpl::openDevice(const QString& port, unsigned int speed)
{
	if (NULL == _drv)
	{
		_port = port;
		_run = true;
		start();
		_startSemaphore.acquire();
	}

	return isDeviceOpen();
}

void MyRPLidarImpl::closeDevice()
{
	_run = false;
	
	if(_drv)
	{
		_drv->stop();
	}
	

	while (false == isFinished())
	{
		rec::robotino::daemons::Log::singleton()->log("RPLidar wait for thread to finish", 2);
		wait(500);
	}
	//wait();
}

bool MyRPLidarImpl::checkRPLIDARHealth()
{
    u_result     op_result;
    rplidar_response_device_health_t healthinfo;


    op_result = _drv->getHealth(healthinfo);
    if (IS_OK(op_result))
	{ // the macro IS_OK is the preperred way to judge whether the operation is succeed.
		rec::robotino::daemons::Log::singleton()->log(QString("RPLidar health status : %1").arg(healthinfo.status),0);
        if (healthinfo.status == RPLIDAR_STATUS_ERROR)
		{
            rec::robotino::daemons::Log::singleton()->log("Error, rplidar internal error detected. Please reboot the device to retry.",0);
            // enable the following code if you want rplidar to be reboot by software
            // drv->reset();
            return false;
        } else {
			rec::robotino::daemons::Log::singleton()->log(QString("RPLidar health status : OK"),0);
            return true;
        }

    } else {
        rec::robotino::daemons::Log::singleton()->log(QString("Error, cannot retrieve the lidar health code: %1").arg(op_result),0);
        return false;
    }
}

void MyRPLidarImpl::cleanup()
{
	_startSemaphore.release();
	RPlidarDriver::DisposeDriver(_drv);
	_drv = NULL;
}

void MyRPLidarImpl::run()
{
	quint32 opt_com_baudrate = 115200;
	
	_drv = RPlidarDriver::CreateDriver(RPlidarDriver::DRIVER_TYPE_SERIALPORT);
	
	if(IS_FAIL(_drv->connect(_port.toStdString().c_str(), opt_com_baudrate)))
	{
        rec::robotino::daemons::Log::singleton()->log(QString("Error, cannot bind to the specified serial port %1.").arg(_port),0);
		cleanup();
        return;
    }
	
	rplidar_response_device_info_t devinfo;

	// retrieving the device info
    ////////////////////////////////////////
	u_result op_result = _drv->getDeviceInfo(devinfo);

    if (IS_FAIL(op_result))
	{
        rec::robotino::daemons::Log::singleton()->log("Error, cannot get device info.",0);
		cleanup();
		return;
    }
	
	QString serialNumber = "RPLIDAR S/N: ";
    for (int pos = 0; pos < 16 ;++pos)
	{
		int c = devinfo.serialnum[pos];
		serialNumber += QString("%1").arg(quint16(0xFF)&(quint16)c,2,16,QChar('0'));
    }
	rec::robotino::daemons::Log::singleton()->log(serialNumber,0);

	QString firmwareVersion = QString("Firmware Ver: %1.%2").arg(devinfo.firmware_version >> 8).arg(quint16(0xFF)&(quint16)devinfo.firmware_version, 2, 10, QChar('0'));
    QString hardwareVersion = QString("Hardware Rev: %1").arg((int)devinfo.hardware_version);
	
	rec::robotino::daemons::Log::singleton()->log(firmwareVersion,0);
	rec::robotino::daemons::Log::singleton()->log(hardwareVersion,0);

    // check health...
    if (!checkRPLIDARHealth())
	{
		cleanup();
        return;
    }
	
	_startSemaphore.release();
	
	_drv->startMotor();
    // start scan...
	
	if(RESULT_OK == _drv->startScan(true,false) )
	{
		rec::robotino::daemons::Log::singleton()->log("RPLidar startScan: OK", 1);
		
		quint32 seq = 0;
		while(_run)
		{
			rplidar_response_measurement_node_t local_scan[2048];
			int count = _drv->grabScan(&local_scan[0],sizeof(local_scan));

			if(count > 0)
			{
				_drv->ascendScanData(local_scan,count);
				rec::LaserScanMessage msg;
				msg.header.seq = seq++;
				msg.angle_min = 0.0f;
				msg.angle_max = 0.01745f*359.0f;
				msg.angle_increment = 0.01745f*1.0f;
				msg.range_min = 0.1f;
				msg.range_max = 4.0f;
				
				QVector<float> ranges(360,0);

				QString str;
				for (int pos = 0; pos < (int)count; ++pos)
				{
					bool inSync = (local_scan[pos].sync_quality & RPLIDAR_RESP_MEASUREMENT_SYNCBIT) ? true : false;
					double angle = (local_scan[pos].angle_q6_checkbit >> RPLIDAR_RESP_MEASUREMENT_ANGLE_SHIFT) / 64.0f;
					double distance = local_scan[pos].distance_q2 / 4.0f;
					int syncQuality = local_scan[pos].sync_quality >> RPLIDAR_RESP_MEASUREMENT_QUALITY_SHIFT;

					str += QString("%1 %2 %3 %4\n").arg(inSync ? "S" : " ").arg(angle).arg(distance).arg(syncQuality);
					
					int angleInt = (int)round(angle);
					if(angleInt>=ranges.count())
					{
						angleInt = ranges.count()-1;
					}
					else if(angleInt < 0)
					{
						angleInt = 0;
					}
					ranges[ranges.count()-angleInt-1] = 0.001f * distance;
				}
				
				msg.ranges = ranges;
				
				rec::robotino::daemons::Log::singleton()->log(str,1);

				Q_EMIT scan(msg);
			}
			else
			{
				_run = false;
			}
		}
	}

	rec::robotino::daemons::Log::singleton()->log("RPLidar stop", 2);
	_drv->stop();
	rec::robotino::daemons::Log::singleton()->log("RPLidar stopMotor", 2);
	_drv->stopMotor();
	rec::robotino::daemons::Log::singleton()->log("RPLidar DisposeDriver", 2);
	RPlidarDriver::DisposeDriver(_drv);
	_drv = NULL;
	
	rec::robotino::daemons::Log::singleton()->log("RPLidar thread finished", 2);
}