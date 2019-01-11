#include "MapHandler.h"
#include "rec/robotino/daemons/Log.h"
#include "UdpLogger.h"

MapHandler::MapHandler( QObject* parent )
{
	_width = 1600;
	_height = 1600;
	_resolution = 0.05;
	_offsetx = -20;
	_offsety = -20;

	_lastImageTime = 0;

	_angle_min = 4.88692;
	_angle_max = 7.68817;
	_angle_increment = 0.00872665;
    _roll = 0;
}

void MapHandler::on_map_changed(const QByteArray& qba,int width,int height, float resolution, float offsetx, float offsety)
{
	QMutexLocker lk( &_imglock );

	QString logmsg = QString("MapHandler::on_map_changed, w = %1, h = %2, reso = %3, offX = %4, offY = %5")
							.arg(width).arg(height).arg(resolution).arg(offsetx).arg(offsety);
	UdpLogger::Singleton()->broadcastLog(logmsg, 1);
	rec::robotino::daemons::Log::singleton()->log(logmsg, 2);

	if(resolution <= 0)
	{
		rec::robotino::daemons::Log::singleton()->log(QString("MapHandler::on_map_changed, invalid resolution: %1 !!").arg(resolution), 0);
		resolution = 0.01;
	}
	//if(qba.length() < (width*height*sizeof(char)))
	//{
	//	QString msg = QString("MapHandler::on_map_changed, length of map bytearray no enough!! actual length = %1, required = %2!!").arg(resolution).arg(width*height*sizeof(char));

	//	UdpLogger::Singleton()->broadcastLog(msg, 1);
	//	rec::robotino::daemons::Log::singleton()->log(msg, 0);
	//	return;
	//}

	//QImage image = fromOccupancyGrid(qba,height,width);

	//bool s = image.save("/var/tmp/map.png","png");
	//rec::robotino::daemons::Log::singleton()->log(QString("recieved map"),1);

	_lastMapImage = qba;

	_lastImageTime = QDateTime::currentDateTime().toTime_t();
	//_lastImageTime = QDateTime::currentDateTime().toMSecsSinceEpoch();

	_width = width;
	_height = height;
	_resolution = resolution;
	_offsetx = offsetx;
	_offsety = offsety;

	rec::robotino::daemons::Log::singleton()->log("MapHandler::on_map_changed Done", 4);
}

QByteArray  MapHandler::getLastImage()
{
	QMutexLocker lk( &_imglock );
	return _lastMapImage;
}

long long MapHandler::getLastImageTime()
{
	return _lastImageTime;
}

int MapHandler::getWidth()
{
	return _width;
}

int MapHandler::getHeight()
{
	return _height;
}

float MapHandler::getResolution()
{
	return _resolution;
}

float MapHandler::getOffsetX()
{
	return _offsetx;
}

float MapHandler::getOffsetY()
{
	return _offsety;
}

void MapHandler::toMetricCoord(double x, double y, double& x_metric, double& y_metric)
{
    x_metric = (x - _width - _offsetx/_resolution) * _resolution;
    y_metric = - ((y + _offsety/_resolution) * _resolution);
}

void MapHandler::toPixelCoord(double x, double y, double r_deg, double dist, double& x_map, double& y_map, double& r_map_deg, double& dist_map)
{
	x_map = (x/_resolution) + _width + (_offsetx/_resolution);
	y_map = (-y/_resolution) - (_offsety/_resolution);
	r_map_deg = -r_deg;
    dist_map = dist/_resolution;
}
void MapHandler::toPixelCoord(double x, double y, double& x_map, double& y_map)
{
    x_map = (x/_resolution) + _width + (_offsetx/_resolution);
    y_map = (-y/_resolution) - (_offsety/_resolution);
}

void MapHandler::on_scan0_changed(const QVector< float >& ranges
			, const QVector< float >& intensities
			, const QVariantMap& parameters)
{
	QMutexLocker lk( &_scanlock );
	_angle_min = parameters["angle_min"].toFloat();
	_angle_max = parameters["angle_max"].toFloat();

	_angle_increment = parameters["angle_increment"].toFloat();
    _roll = parameters["roll"].toFloat();

	_ranges = ranges;
	//UdpLogger::Singleton()->broadcastLog(QString("on_scan0_changed, ranges.size = %1, %2").arg(ranges.size()).arg(_ranges.size()),1);
}

void MapHandler::setRobotPosition(double x, double y, double phi)
{
    _lastLocation.setX(x);
    _lastLocation.setY(y);
    _lastRotation = phi;
}

void MapHandler::getScanPointsOnMap(QVector<QPointF>& scan_points)
{
	QMutexLocker lk( &_scanlock );
	QMutexLocker lk2( &_poselock );
	int numOfRanges = _ranges.size();
	double pose_x = _lastLocation.x();
	double pose_y = _lastLocation.y();
	double r = _lastRotation;

	for(int idx = 0; idx < numOfRanges; idx++)
	{
		double dx, dy, range;
        double total_angle = (r*3.1416/180);

        if( _roll > M_PI_2 || _roll < -M_PI_2) {
            //laser mounted upside down
            total_angle += _angle_min + _angle_increment*idx;
        }
        else {
            total_angle -= _angle_min + _angle_increment*idx;
        }

		range = _ranges[idx];
		dx = cos(total_angle)*range/_resolution + cos(r*3.1416/180)*0.3/_resolution;
		dy = sin(total_angle)*range/_resolution + sin(r*3.1416/180)*0.3/_resolution;

        scan_points.append(QPointF(pose_x + dx, pose_y + dy));
	}
}
