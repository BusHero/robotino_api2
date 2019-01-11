#ifndef _DISTANCEHANDLER_
#define _DISTANCEHANDLER_

#include <QtGui>

class ControlHandler;

class DistanceHandler : public QObject
{
	Q_OBJECT
public:
	DistanceHandler( QObject* parent );

	QByteArray page( const QImage& liveimage, ControlHandler* controlHandler, bool isWebControl ) const;

public Q_SLOTS:
	void on_distance_sensors_changed( const QVector< float >& );
	void on_bumper_changed( bool );
	void on_battery_changed( float battery_voltage, float system_current );
	void on_odometry_changed( double x, double y, double phi, float vx, float vy, float omega, unsigned int sequence );
	void on_parameters_changed( const QMap< QString, QVariant >& );

private:
	void drawLine( QPainter* p, const QPoint& p0, float phi_deg, unsigned int length ) const;
	QImage _template;
	QVector< float > _distances;
	bool _bumper;
	float _battery_voltage;
	float _system_current;
	QImage _noimageavailable;
	double _odoX;
	double _odoY;
	double _odoPhi;
	float _max_voltage;
	float _sleep_voltage;
};

#endif //_DISTANCEHANDLER_
