#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "rec/robotino/rpc/Client.h"

class Client : public rec::robotino::rpc::Client
{
	Q_OBJECT
public:
	Client();

	QHostAddress ea09_ip4address() const { return _ea09_ip4address; }
	QHostAddress ea09_ip4netmask() const { return _ea09_ip4netmask; }

	bool emergencyBumper() const { return _emergencyBumper; }

	bool emergency_bumper() const { return _emergency_bumper; }

	QString motorDriver() const { return _motorDriverStr; }

	void disconnectMotorDriver();

	float battery_voltage() const { return _battery_voltage; }
	float system_current() const { return _system_current; }

	void set_controller_active_enabled( bool enable );

Q_SIGNALS:
	void my_ea09_ip4address_changed( const QHostAddress&, const QHostAddress& );

private Q_SLOTS:
	void on_connected();
	void on_disconnected();
	void on_ea09_ip4address_changed( const QHostAddress&, const QHostAddress& netmask );
	void on_emergency_bumper_changed( bool );

	void on_display_text_info_changed( const rec::rpc::ClientInfoSet& );
	void on_display_backlight_info_changed( const rec::rpc::ClientInfoSet& );
	void on_display_vbar_info_changed( const rec::rpc::ClientInfoSet& );
	void on_display_hbar_info_changed( const rec::rpc::ClientInfoSet& );
	void on_display_progress_info_changed( const rec::rpc::ClientInfoSet& );
	void on_display_clear_info_changed( const rec::rpc::ClientInfoSet& );

	void on_ea_version_changed( const QString& board, quint16 firmware_major, quint16 firmware_minor, quint16 firmware_patch );

	void on_motor_setpoint_changed( float speed, const rec::rpc::ClientInfo& );
	void on_motor_setpoints_changed( float, float, float, const rec::rpc::ClientInfo& );
	void on_omnidrive_changed( float vx, float vy, float omega, const rec::rpc::ClientInfo& );

	void on_motorTimer_timeout();

	void on_battery_changed( float battery_voltage, float system_current );
	void on_api1_setstate_received_changed();

private:
	bool checkDisplaySubscribers() const;
	void activateDisplay( bool before );
	void restartMotorTimer();

	QHostAddress _ea09_ip4address;
	QHostAddress _ea09_ip4netmask;
	bool _emergencyBumper;

	int _numDisplayTextSubscribers;
	int _numDisplayBacklightSubscribers;
	int _numDisplayVBarSubscribers;
	int _numDisplayHBarSubscribers;
	int _numDisplayProgressSubscribers;
	int _numDisplayClearSubscribers;

	bool _emergency_bumper;

	QTimer* _motorTimer;
	QString _motorDriverStr;
	rec::rpc::ClientInfo _motorDriverInfo;
	float _battery_voltage;
	float _system_current;
};

#endif //_CLIENT_H_
