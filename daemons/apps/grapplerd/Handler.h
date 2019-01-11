#ifndef _HANDLER_H_
#define _HANDLER_H_

#include <QtCore>
#include "rec/grappler/serial/SerialThread.h"

class Handler : public QObject
{
	Q_OBJECT
public:
	Handler( const QString& port );
	~Handler();

	void init( bool enterbootloader, bool reset, bool fwVersion );

Q_SIGNALS:
	void log( const QString& message, int level );

public Q_SLOTS:

private Q_SLOTS:
	void on_client_connected();

	void on_serialThread_serialPortError( const QString& /*error*/ );
	void on_serialThread_infoReceived( int majorVer, int minorVer, int patchVer );
	void on_serialThread_statusReceived( int /*id*/, int /*error*/, const QByteArray& /*data*/ );
	void on_serialThread_servosDetected( const rec::grappler::HardwareInfo& /*info*/ );
	void on_serialThread_allServoPositions( const rec::grappler::HardwareInfo& /*info*/ );
	void on_serialThread_storePosition( const rec::grappler::HardwareInfo& /*info*/ );
	void on_serialThread_allAxesLimits( const rec::grappler::HardwareInfo& /*info*/ );
	void on_serialThread_communicationReady();
	void on_serialThread_keepAliveReceived();

	void on_grappler_set_power_changed( int line, bool power );
	void on_grappler_set_positions_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data );
	void on_grappler_toggle_torque_changed();

private:
	void timerEvent( QTimerEvent* e );
	void postLog( const QString& message );
	void setupConnections();

	rec::grappler::serial::SerialThread* _serialThread;

	QString _port;
	int _timerId;

	rec::grappler::HardwareInfo _servosDetectedInfo;

	bool _enterbootloader;
	bool _reset;
	bool _fwVersion;

	QTime _allServoPositions_time;
	int _allServoPositions_minTime;

	bool _isClientConnected;

	int _maxPositionDelta;
};

#endif //_HANDLER_H_
