#ifndef _Motor_H_
#define _Motor_H_

#include <QtCore>
#include "rec/utils/MeanBuffer.h"
#include "rec/rpc/ClientInfo.h"

class Motor : public QObject
{
	Q_OBJECT
public:
	Motor( unsigned int id );

Q_SIGNALS:
	void motor_setpoint_changed( float );
	void log( const QString&, int );
	void motor_mode_changed( const QString& );
	void gripper_state_changed( int state );
	void piControllerEnabledChanged(int motor,bool enabled);

public Q_SLOTS:
	void set_motor_mode( const QString& mode );
	void set_motor_readings( const QVector< float >& /*speeds in rpm*/, const QVector< int >& /*positions*/, const QVector< float >& /*currents in A*/, float /*deltat*/ );
	void set_motor_setpoint( float, const rec::rpc::ClientInfo& );

private Q_SLOTS:
	void on_client_connected();
	void on_overCurrentTimer_timeout();

private:
	typedef enum { VelMode, PosMode, FreeMode, GripperMode } Mode_t;

	const unsigned int _id;
	Mode_t _mode;

	float _setPoint;

	float _speed;
	int _position;

	rec::MeanBuffer<float> _current;

	float _maxcurrent;
	float _maxcurrentFromConfig;
	float _onreach;
	unsigned int _exceedtime;

	QTimer* _overCurrentTimer;
	bool _isOverCurrentDetected;
	float _setPointAtOverCurrent;
};

#endif //_Motor_H_
