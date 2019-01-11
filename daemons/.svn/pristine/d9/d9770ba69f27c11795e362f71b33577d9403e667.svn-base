#ifndef _ROSHANDLER_H_
#define _ROSHANDLER_H_

#include <QtCore>
#include <QtGui>
#include "Client.h"

class MapHandler : public QObject
{
	Q_OBJECT
public:
	MapHandler( QObject* parent );

	QByteArray getLastImage();
	long long getLastImageTime();

	void setInitialPose(float x,float y,float r);

	int getWidth();
	int getHeight();
	float getResolution();
	float getOffsetX();
	float getOffsetY();

    void toMetricCoord(double x, double y, double& x_metric, double& y_metric);
    void toPixelCoord(double x, double y, double r, double dist, double& x_map, double& y_map, double& r_map, double& dist_map);
    void toPixelCoord(double x, double y, double& x_map, double& y_map);

    void setRobotPosition(double x, double y, double phi);
    void getScanPointsOnMap(QVector<QPointF> &scan_points);
public Q_SLOTS:
	void on_map_changed(const QByteArray&, int, int, float, float, float);

	void on_scan0_changed(const QVector< float >& ranges
			, const QVector< float >& intensities
			, const QVariantMap& parameters
			);

private:
	QMutex _lock;
	QMutex _imglock;
	QMutex _poselock;
	QMutex _scanlock;

	QByteArray _lastMapImage;
	long long _lastImageTime;

	QPointF _lastLocation;
	double _lastRotation;

	//parameters for mat
	int _width;
	int _height;
	float _resolution;
	float _offsetx;
	float _offsety;

	//parameters for scan0
	float _angle_min;
	float _angle_max;
	float _angle_increment;
    float _roll;
	QVector< float > _ranges;

	static QImage fromOccupancyGrid( QByteArray qba, int height, int width )
	{
		QImage imgBuffer(QImage( (unsigned char *)qba.data(), width, height, width * sizeof( char ), QImage::Format_Indexed8 ).copy());

		QVector<QRgb> colorTable;
		unsigned int i = 0;
		for( i = 0; i <= 128; ++i )
		{
			QRgb col = 0xa0000000;
			int val = (128 - i) / 100 * 255;
			col |= val | (val << 8) | (val << 16);
			colorTable.push_back(col);
		}
		i = 129;
		colorTable.push_back( 0xFF0000FF );	// blue for cells occupied by obstacle growing
		i = 130;
		colorTable.push_back( 0xFF888888 );	// grey for undeletable cells
		for( /*i*/; i < 255; ++i )
		{
			colorTable.push_back( 0xFFFF0000 );	// red for warning
		}
		colorTable.push_back( 0xFF888888 ); // grey for 255 = -1 = "unknown"
		imgBuffer.setColorTable( colorTable );

		return imgBuffer;
	}
};

#endif //_ROSHANDLER_H_
