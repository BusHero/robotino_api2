#ifndef _HANDLER_H_
#define _HANDLER_H_

#include <QtCore>
#include "rec/robotinoxt/serial/SerialThread.h"
#include "rec/rpc/ClientInfo.h"

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
	void set_pressure( const QVector<float>& );
	void set_compressors_enabled( bool enabled );
	void set_water_drain_valve( bool enabled );
	void set_gripper_valve1( bool enabled );
	void set_gripper_valve2( bool enabled );
	void on_client_cbha_readings_info_changed( const rec::rpc::ClientInfoSet& clients );

private Q_SLOTS:
	void on_client_connected();

	void on_serialThread_infoReceived( int majorVer, int minorVer, int patchVer );
	void on_serialThread_statusReceived( const QVector< qint16 >& pressures, bool pressureSensor, const QVector< float >& potis );
	void on_serialThread_serialPortError( const QString& error );

private:
	void timerEvent( QTimerEvent* e );
	void postLog( const QString& message );
	void setupConnections();

	rec::robotinoxt::serial::SerialThread* _serialThread;

	QString _port;
	int _timerId;

	int _max_pressure;

	bool _enterbootloader;
	bool _reset;
	bool _fwVersion;

	bool _isClientConnected;

	int _intern2Extern[8];
	int _extern2Intern[8];

	int _waterDrainValveNum;
	int _gripperValve1Num;
	int _gripperValve2Num;

	static const QVector< qint16 > _zeroPressures;
};

#endif //_HANDLER_H_
