#include "MyUrg.h"
#include "Urg_driver.h"

#include "Client.h"
#include "rec/robotino/daemons/Log.h"

#include <qhostaddress.h>

extern Client* client;
extern rec::robotino::daemons::Log* logHandler;

using namespace qrk;

MyUrg::MyUrg(const QString& device, unsigned int channel)
	: _run(true)
	, _device(device)
	, _channel(channel)
{

}

MyUrg::~MyUrg()
{
	_run = false;
	wait();
}

void MyUrg::run()
{
	Lidar::connection_type_t connectionType = Lidar::Serial;
	long connect_baudrate = 115200;

	QHostAddress address;
	if (address.setAddress(_device))
	{
		connectionType = Lidar::Ethernet;
		connect_baudrate = client->getParameter("~/baudrate_or_port", 10940).toInt();;
	}

	Urg_driver urg;

	logHandler->log(QString("Trying to open scanner %1 baudrate:%2 connectionType:%3").arg(_device).arg(connect_baudrate).arg(Lidar::Serial == connectionType ? "Serial" : "Ethernet"), 0);
	if (false == urg.open(_device.toLatin1().constData(), connect_baudrate, connectionType))
	{
		logHandler->log(QString("Error"), 0);
		qApp->exit(1);
		return;
	}

	logHandler->log(QString("Success"), 0);

	int angle_min_deg = client->getParameter("~/angle_min", -90).toInt();
	if (angle_min_deg < -120)
	{
		rec::robotino::daemons::Log::singleton()->log("angle_min to small", 0);
		angle_min_deg = -120;
	}

	int angle_max_deg = client->getParameter("~/angle_max", 90).toInt();
	if (angle_max_deg > 120)
	{
		rec::robotino::daemons::Log::singleton()->log("angle_max to large", 0);
		angle_max_deg = 120;
	}

	if (angle_min_deg >= angle_max_deg)
	{
		rec::robotino::daemons::Log::singleton()->log("angle_min greater or equal angle_max", 0);
		angle_min_deg = -90;
		angle_max_deg = 90;
	}

	QString str = QString("Setting angle_min:%1 angle_max:%2").arg(angle_min_deg).arg(angle_max_deg);
	logHandler->log(str, 0);
	urg.set_scanning_parameter(urg.deg2step(angle_min_deg), urg.deg2step(angle_max_deg), 0);

	QVariantMap parameters;
	parameters["frame_id"] = "";
	
	const bool readIntensities = (client->getParameter("~/read_intensities", 0).toInt() > 0);

	if(readIntensities)
	{
		urg.start_measurement(Urg_driver::Distance_intensity, Urg_driver::Infinity_times, 0);
	}
	else
	{
		urg.start_measurement(Urg_driver::Distance, Urg_driver::Infinity_times, 0);
	}
	
	unsigned int seq = 0;
	long timestamp_hist = 0;
	while (_run)
	{
		std::vector<long> data;
		std::vector<unsigned short> intensity;
		long time_stamp = 0;
		
		if(readIntensities)
		{
			if (false == urg.get_distance_intensity(data,intensity, &time_stamp))
			{
				logHandler->log(QString("Urg_driver::get_distance_intensity(): %1").arg(urg.what()), 0);
				break;
			}
		}
		else
		{
			if (false == urg.get_distance(data, &time_stamp))
			{
				logHandler->log(QString("Urg_driver::get_distance(): %1").arg(urg.what()), 0);
				break;
			}
		}
		
		//qDebug() << data.size() << " " << time_stamp;

		double angle_min = urg.index2rad(0);
		double angle_max = urg.index2rad(data.size()-1);

		parameters["seq"] = seq++;
		parameters["stamp"] = 0.001*time_stamp;
		parameters["angle_min"] = angle_min;
		parameters["angle_max"] = angle_max;
		parameters["angle_increment"] = (angle_max - angle_min) / (data.size()-1);
		parameters["time_increment"] = 0.001*(time_stamp - timestamp_hist);
		parameters["range_min"] = 0.001*urg.min_distance();
		parameters["range_max"] = 0.001*urg.max_distance();

		//qDebug() << parameters;

		QVector<float> ranges(data.size());
		for (int i = 0; i < data.size(); ++i)
		{
			ranges[i] = 0.001 * data[i];
		}
		
		QVector<float> intensities(intensity.size());
		for (int i = 0; i < intensity.size(); ++i)
		{
			intensities[i] = intensity[i];
		}

		timestamp_hist = time_stamp;

		Q_EMIT scan(_channel, ranges, intensities, parameters);
	}

	urg.stop_measurement();
	urg.close();
	
	if(_run)
	{
		qApp->exit(1);
	}	
}