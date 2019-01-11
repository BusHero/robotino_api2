#include "Client.h"
#include "rec/robotino/daemons/Log.h"

extern rec::robotino::daemons::Log* logHandler;

#ifdef WIN32
#define DATA_DIR "c:/temp"
#else
#define DATA_DIR "/tmp"
#endif

#define JSON_TOPIC_FUNCTIONS(TOPICNAME) \
	void Client::on_##TOPICNAME##_changed(const QByteArray& jsonData) \
	{ \
		QString str = QString("%1\n").arg(#TOPICNAME); \
		QString jsonStr = QString(jsonData); \
		jsonStr.replace("\\r\\n","\n"); \
		Q_EMIT logWithInfo(str, jsonStr, 0); \
		checkEchoCount(); \
	}

#define JSON_WITH_DATA_TOPIC_FUNCTIONS(TOPICNAME) \
	void Client::on_##TOPICNAME##_changed(const QByteArray& jsonData, const QByteArray& data) \
	{ \
		QString str = QString("%1 data size:%2\n").arg(#TOPICNAME).arg(data.size()); \
		emit logWithInfo(str,QString(jsonData),0); \
		if(logHandler->verbosity() > 0) \
		{ \
			QString fileName = QString("%1/%2.data").arg(DATA_DIR).arg(#TOPICNAME); \
			QFile file(fileName); \
			if (file.open(QIODevice::WriteOnly)) \
			{ \
				file.write(data); \
				Q_EMIT log(QString("%1 written").arg(fileName), 0); \
			} \
			else \
			{ \
				Q_EMIT log(QString("Error writing %1").arg(fileName), 0); \
			} \
		} \
		checkEchoCount(); \
	}
	
#define JSON_TOPIC_CONNECTION(TOPICNAME) \
	connected &= (bool)connect( this \
		, SIGNAL( TOPICNAME##_changed( const QByteArray& ) ) \
		, SLOT( on_##TOPICNAME##_changed( const QByteArray& ) ) ); \
	Q_ASSERT( connected );

#define JSON_WITH_DATA_TOPIC_CONNECTION(TOPICNAME) \
	connected &= (bool)connect( this \
		, SIGNAL( TOPICNAME##_changed( const QByteArray&,const QByteArray& ) ) \
		, SLOT( on_##TOPICNAME##_changed( const QByteArray&,const QByteArray& ) ) ); \
	Q_ASSERT( connected );
	
#define JSON_TOPIC_PUBLISH(TOPICNAME) \
	else if ( #TOPICNAME == _topicName) \
	{ \
		if (1 != _publish_parameters.size()) \
		{ \
			Q_EMIT log(QString("Expected: %1 filename").arg(#TOPICNAME), 0); \
			exit(1); \
		} \
		\
		QByteArray data; \
		QString filename = _publish_parameters.at(0); \
		QFile file(filename); \
		if (file.open(QIODevice::ReadOnly)) \
		{ \
			data = file.readAll(); \
			file.close(); \
		} \
		else \
		{ \
			Q_EMIT log(QString("Error loading %1").arg(filename), 0); \
			exit(1); \
		} \
		\
		if (false == set_##TOPICNAME(data)) \
		{ \
			Q_EMIT log( QString("Error calling set_%1").arg(#TOPICNAME), 0); \
			exit(1); \
		} \
		\
		Q_EMIT log(QString("published %1 %2").arg(#TOPICNAME).arg(data.constData()), 0); \
	}

#define JSON_WITH_DATA_TOPIC_PUBLISH(TOPICNAME) \
	else if ( #TOPICNAME == _topicName) \
	{ \
		if (2 != _publish_parameters.size()) \
		{ \
			Q_EMIT log(QString("Expected: %1 jsonFile dataFile").arg(#TOPICNAME), 0); \
			exit(1); \
		} \
		\
		QByteArray jsonData; \
		QString jsonFileName = _publish_parameters.at(0); \
		QFile jsonFile(jsonFileName); \
		if (jsonFile.open(QIODevice::ReadOnly)) \
		{ \
			jsonData = jsonFile.readAll(); \
			jsonFile.close(); \
		} \
		else \
		{ \
			Q_EMIT log(QString("Error loading %1").arg(jsonFileName), 0); \
			exit(1); \
		} \
		QByteArray data; \
		QString fileName = _publish_parameters.at(1); \
		QFile file(fileName); \
		if (file.open(QIODevice::ReadOnly)) \
		{ \
			data = file.readAll(); \
			file.close(); \
		} \
		else \
		{ \
			Q_EMIT log(QString("Error loading %1").arg(fileName), 0); \
			exit(1); \
		} \
		\
		if (false == set_##TOPICNAME(jsonData,data)) \
		{ \
			Q_EMIT log( QString("Error calling set_%1").arg(#TOPICNAME), 0); \
			exit(1); \
		} \
		\
		Q_EMIT log(QString("published %1 %2\ndata size:%3") \
			.arg(#TOPICNAME).arg(jsonData.constData()).arg(data.size()), 0); \
	}

Client::Client( int timeout )
	: _publish_timer( NULL )
	, _echoCount(0)
	, _echoMaxCount(0)
{
	bool connected = true;

	if( timeout > 0 )
	{
		QTimer* timeoutTimer = new QTimer( this );
		timeoutTimer->setSingleShot( true );
		timeoutTimer->setInterval( timeout );
		connected &= (bool)connect( timeoutTimer, SIGNAL( timeout() ), SLOT( on_timeout() ) );
		Q_ASSERT( connected );
		timeoutTimer->start();
	}

	connected &= (bool)connect( this, SIGNAL( is_connected() ), SLOT( on_connected() ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( is_disconnected() ), SLOT( on_disconnected() ) );
	Q_ASSERT( connected );


	connected &= (bool)connect( this, SIGNAL( image0_changed( const QByteArray&,
		unsigned int,
		unsigned int,
		unsigned int,
		const QString& ) ), SLOT( on_image0_changed( const QByteArray&,
		unsigned int,
		unsigned int,
		unsigned int,
		const QString& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( camera0_settings_changed( unsigned int, unsigned int, const QString& ) ), SLOT( on_camera0_settings_changed( unsigned int, unsigned int, const QString& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_camera0_settings_changed( unsigned int, unsigned int, const QString& ) ), SLOT( on_set_camera0_settings_changed( unsigned int, unsigned int, const QString& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_camera0_control_changed( const QString&, int ) ), SLOT( on_set_camera0_control_changed( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( camera0_capabilities_changed( const QString&, const QMap<QString, QVector<QSize> >&, const QStringList& ) ), SLOT( on_camera0_capabilities_changed( const QString&, const QMap<QString, QVector<QSize> >&, const QStringList& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( camera0_calibration_changed( const QVector<double>& ) ), SLOT( on_camera0_calibration_changed( const QVector<double>& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( gyroscope_changed( double, double, const rec::rpc::ClientInfo& ) ), SLOT( on_gyroscope_changed( double, double, const rec::rpc::ClientInfo& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( gyroscope_info_changed( const rec::rpc::ClientInfoSet& ) ), SLOT( on_gyroscope_info_changed( const rec::rpc::ClientInfoSet& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect(this, SIGNAL(gyroscope_ext_changed(double, double, const rec::rpc::ClientInfo&)), SLOT(on_gyroscope_ext_changed(double, double, const rec::rpc::ClientInfo&)));
	Q_ASSERT(connected);

	connected &= (bool)connect(this, SIGNAL(gyroscope_ext_info_changed(const rec::rpc::ClientInfoSet&)), SLOT(on_gyroscope_ext_info_changed(const rec::rpc::ClientInfoSet&)));
	Q_ASSERT(connected);

	connected &= (bool)connect( this, SIGNAL( set_shutdown_changed( bool ) ), SLOT( on_set_shutdown_changed( bool ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_shutdown_info_changed( const rec::rpc::ClientInfoSet& ) ), SLOT( on_set_shutdown_info_changed( const rec::rpc::ClientInfoSet& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( power_button_changed( bool ) ), SLOT( on_power_button_changed( bool ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( power_button_info_changed( const rec::rpc::ClientInfoSet& ) ), SLOT( on_power_button_info_changed( const rec::rpc::ClientInfoSet& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( request_shutdown_changed( bool ) ), SLOT( on_request_shutdown_changed( bool ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( request_shutdown_info_changed( const rec::rpc::ClientInfoSet& ) ), SLOT( on_request_shutdown_info_changed( const rec::rpc::ClientInfoSet& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( emergency_bumper_changed( bool ) ), SLOT( on_emergency_bumper_changed( bool ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_emergency_bumper_changed( bool ) ), SLOT( on_set_emergency_bumper_changed( bool ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( ea09_ip4address_changed( const QHostAddress&, const QHostAddress& ) ), SLOT( on_ea09_ip4address_changed( const QHostAddress&, const QHostAddress& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_ea09_ip4address_changed( const QHostAddress&, const QHostAddress& ) ), SLOT( on_set_ea09_ip4address_changed( const QHostAddress&, const QHostAddress& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this
		, SIGNAL( scan0_changed( const QVector< float >&,	const QVector< float >&, const QVariantMap& ) )
		, SLOT( on_scan0_changed( const QVector< float >&,	const QVector< float >&, const QVariantMap& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( scan0_info_changed( const rec::rpc::ClientInfoSet& ) ), SLOT( on_scan0_info_changed( const rec::rpc::ClientInfoSet& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this
		, SIGNAL( scan1_changed( const QVector< float >&,	const QVector< float >&, const QVariantMap& ) )
		, SLOT( on_scan1_changed( const QVector< float >&,	const QVector< float >&, const QVariantMap& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( scan1_info_changed( const rec::rpc::ClientInfoSet& ) ), SLOT( on_scan1_info_changed( const rec::rpc::ClientInfoSet& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this
		, SIGNAL( scan2_changed( const QVector< float >&,	const QVector< float >&, const QVariantMap& ) )
		, SLOT( on_scan2_changed( const QVector< float >&,	const QVector< float >&, const QVariantMap& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( scan2_info_changed( const rec::rpc::ClientInfoSet& ) ), SLOT( on_scan2_info_changed( const rec::rpc::ClientInfoSet& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this
		, SIGNAL( scan3_changed( const QVector< float >&,	const QVector< float >&, const QVariantMap& ) )
		, SLOT( on_scan3_changed( const QVector< float >&,	const QVector< float >&, const QVariantMap& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( scan3_info_changed( const rec::rpc::ClientInfoSet& ) ), SLOT( on_scan3_info_changed( const rec::rpc::ClientInfoSet& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( omnidrive_changed( float, float, float, const rec::rpc::ClientInfo& ) ), SLOT( on_omnidrive_changed( float, float, float, const rec::rpc::ClientInfo& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( digital_input_changed( const QVector< bool >& ) ), SLOT( on_digital_input_changed( const QVector< bool >& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( analog_input_changed( const QVector< float >& ) ), SLOT( on_analog_input_changed( const QVector< float >& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( distance_sensors_changed( const QVector< float >& ) ), SLOT( on_distance_sensors_changed( const QVector< float >& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( bumper_changed( bool ) ), SLOT( on_bumper_changed( bool ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( battery_changed( float, float, bool, int, const QString&, bool, int ) )
		, SLOT( on_battery_changed( float, float, bool, int, const QString&, bool, int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_digital_output_changed( unsigned int /*index*/, bool /*state*/ ) ), SLOT( on_set_digital_output_changed( unsigned int /*index*/, bool /*state*/ ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_digital_output_array_changed( const QVector<bool>& ) ), SLOT( on_set_digital_output_array_changed( const QVector<bool>& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_relay_changed( unsigned int /*index*/, bool /*state*/ ) ), SLOT( on_set_relay_changed( unsigned int /*index*/, bool /*state*/ ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_relay_array_changed( const QVector<bool>& ) ), SLOT( on_set_relay_array_changed( const QVector<bool>& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( display_text_changed( const QString&, unsigned int, unsigned int, bool, bool ) ),
		SLOT( on_display_text_changed( const QString&, unsigned int, unsigned int, bool, bool ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( odometry_changed( double, double, double, float, float, float, unsigned int ) ),
		SLOT( on_odometry_changed( double, double, double, float, float, float, unsigned int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_odometry_changed( double, double, double ) ),
		SLOT( on_set_odometry_changed( double, double, double ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect(this, SIGNAL(pose_changed(double /*x*/, double /*y*/, double /*phi*/, double /*errx*/, double /*erry*/, double /*errphi*/, unsigned int /*sequence*/)),
		SLOT(on_pose_changed(double /*x*/, double /*y*/, double /*phi*/, double /*errx*/, double /*erry*/, double /*errphi*/, unsigned int /*sequence*/)));
	Q_ASSERT(connected);

	connected &= (bool)connect( this, SIGNAL( clients_connected_changed( const QVector< rec::rpc::ClientInfo >& ) ),
		SLOT( on_clients_connected_changed( const QVector< rec::rpc::ClientInfo >& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( northstar_changed( unsigned int,
		unsigned int,
		unsigned int,
		float,
		float,
		float,
		unsigned int,
		unsigned int ) ),
		SLOT( on_northstar_changed( unsigned int,
		unsigned int,
		unsigned int,
		float,
		float,
		float,
		unsigned int,
		unsigned int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( cbha_readings_changed( const QVector<float>&, bool, const QVector<float>&, float ) ),
		SLOT( on_cbha_readings_changed( const QVector<float>&, bool, const QVector<float>&, float ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( cbha_set_pressure_changed( const QVector<float>& ) ),
		SLOT( on_cbha_set_pressure_changed( const QVector<float>& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( grappler_store_positions_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& ) ),
		SLOT( on_grappler_store_positions_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( grappler_set_power_changed( int, bool ) ),
		SLOT( on_grappler_set_power_changed( int, bool ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( grappler_set_positions_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& ) ),
		SLOT( on_grappler_set_positions_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( grappler_servos_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& ) ),
		SLOT( on_grappler_servos_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( grappler_readings_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& ) ),
		SLOT( on_grappler_readings_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( grappler_toggle_torque_changed() ),
		SLOT( on_grappler_toggle_torque_changed() ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( log_changed( const QString&, const QString&, int ) ),
		SLOT( on_log_changed( const QString&, const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_log_level_changed( const QString&, int ) ),
		SLOT( on_set_log_level_changed( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( motor0_setpoint_changed( float, const rec::rpc::ClientInfo& ) ),
		SLOT( on_motor0_setpoint_changed( float, const rec::rpc::ClientInfo& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( motor0_reset_position_changed( int, const rec::rpc::ClientInfo& ) ),
		SLOT( on_motor0_reset_position_changed( int, const rec::rpc::ClientInfo& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( motor1_setpoint_changed( float, const rec::rpc::ClientInfo& ) ),
		SLOT( on_motor1_setpoint_changed( float, const rec::rpc::ClientInfo& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( motor1_reset_position_changed( int, const rec::rpc::ClientInfo& ) ),
		SLOT( on_motor1_reset_position_changed( int, const rec::rpc::ClientInfo& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( motor2_setpoint_changed( float, const rec::rpc::ClientInfo& ) ),
		SLOT( on_motor2_setpoint_changed( float, const rec::rpc::ClientInfo& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( motor2_reset_position_changed( int, const rec::rpc::ClientInfo& ) ),
		SLOT( on_motor2_reset_position_changed( int, const rec::rpc::ClientInfo& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( motor3_setpoint_changed( float, const rec::rpc::ClientInfo& ) ),
		SLOT( on_motor3_setpoint_changed( float, const rec::rpc::ClientInfo& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( motor3_reset_position_changed( int, const rec::rpc::ClientInfo& ) ),
		SLOT( on_motor3_reset_position_changed( int, const rec::rpc::ClientInfo& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( motor_setpoints_changed( float, float, float, const rec::rpc::ClientInfo& ) ),
		SLOT( on_motor_setpoints_changed( float, float, float, const rec::rpc::ClientInfo& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( motor_readings_changed( const QVector< float >&, const QVector< int >&, const QVector< float >&, float ) ),
		SLOT( on_motor_readings_changed( const QVector< float >&, const QVector< int >&, const QVector< float >&, float ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( custom_message_changed( unsigned int, const QByteArray& ) ),
		SLOT( on_custom_message_changed( unsigned int, const QByteArray& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( custom_message0_changed( const QByteArray& ) ),
		SLOT( on_custom_message0_changed( const QByteArray& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( custom_message1_changed( const QByteArray& ) ),
		SLOT( on_custom_message1_changed( const QByteArray& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( custom_message2_changed( const QByteArray& ) ),
		SLOT( on_custom_message2_changed( const QByteArray& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( custom_message3_changed( const QByteArray& ) ),
		SLOT( on_custom_message3_changed( const QByteArray& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( kinect0_depth_changed( const QByteArray&, const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ),
		SLOT( on_kinect0_depth_changed( const QByteArray&, const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( kinect1_depth_changed( const QByteArray&, const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ),
		SLOT( on_kinect1_depth_changed( const QByteArray&, const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( kinect2_depth_changed( const QByteArray&, const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ),
		SLOT( on_kinect2_depth_changed( const QByteArray&, const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( kinect3_depth_changed( const QByteArray&, const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ),
		SLOT( on_kinect3_depth_changed( const QByteArray&, const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( kinect0_video_changed( const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ),
		SLOT( on_kinect0_video_changed( const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( kinect1_video_changed( const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ),
		SLOT( on_kinect1_video_changed( const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( kinect2_video_changed( const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ),
		SLOT( on_kinect2_video_changed( const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( kinect3_video_changed( const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ),
		SLOT( on_kinect3_video_changed( const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( kinect0_tilt_changed( double ) ),
		SLOT( on_kinect0_tilt_changed( double ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( kinect1_tilt_changed( double ) ),
		SLOT( on_kinect1_tilt_changed( double ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( kinect2_tilt_changed( double ) ),
		SLOT( on_kinect2_tilt_changed( double ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( kinect3_tilt_changed( double ) ),
		SLOT( on_kinect3_tilt_changed( double ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( kinect0_accel_changed( double, double, double ) ),
		SLOT( on_kinect0_accel_changed( double, double, double ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( kinect1_accel_changed( double, double, double ) ),
		SLOT( on_kinect1_accel_changed( double, double, double ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( kinect2_accel_changed( double, double, double ) ),
		SLOT( on_kinect2_accel_changed( double, double, double ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( kinect3_accel_changed( double, double, double ) ),
		SLOT( on_kinect3_accel_changed( double, double, double ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( ea_version_changed( const QString&, quint16, quint16, quint16 ) ),
		SLOT( on_ea_version_changed( const QString&, quint16, quint16, quint16 ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( process_status_changed( const rec::robotino::rpc::ProcessStatus& ) ),
		SLOT( on_process_status_changed( const rec::robotino::rpc::ProcessStatus& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( process_output_changed( const rec::robotino::rpc::ProcessOutput& ) ),
		SLOT( on_process_output_changed( const rec::robotino::rpc::ProcessOutput& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( disconnect_api1_clients_changed() ),
		SLOT( on_disconnect_api1_clients_changed() ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( api1_setstate_received_changed() ),
		SLOT( on_api1_setstate_received_changed() ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( charger0_info_changed( unsigned int, float, float, float, float, int, const QString& ) ),
		SLOT( on_charger0_info_changed( unsigned int, float, float, float, float, int, const QString& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( charger0_version_changed( int, int, int ) ),
		SLOT( on_charger0_version_changed( int, int, int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( charger0_error_changed( unsigned int, const QString& ) ),
		SLOT( on_charger0_error_changed( unsigned int, const QString& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( charger0_clear_error_changed() ),
		SLOT( on_charger0_clear_error_changed() ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( charger1_info_changed( unsigned int, float, float, float, float, int, const QString& ) ),
		SLOT( on_charger1_info_changed( unsigned int, float, float, float, float, int, const QString& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( charger1_version_changed( int, int, int ) ),
		SLOT( on_charger1_version_changed( int, int, int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( charger1_error_changed( unsigned int, const QString& ) ),
		SLOT( on_charger1_error_changed( unsigned int, const QString& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( charger1_clear_error_changed() ),
		SLOT( on_charger1_clear_error_changed() ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( charger2_info_changed( unsigned int, float, float, float, float, int, const QString& ) ),
		SLOT( on_charger2_info_changed( unsigned int, float, float, float, float, int, const QString& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( charger2_version_changed( int, int, int ) ),
		SLOT( on_charger2_version_changed( int, int, int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( charger2_error_changed( unsigned int, const QString& ) ),
		SLOT( on_charger2_error_changed( unsigned int, const QString& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( charger2_clear_error_changed() ),
		SLOT( on_charger2_clear_error_changed() ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( gripper_state_changed( int ) ),
		SLOT( on_gripper_state_changed( int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( sensors_changed( const QVector< QString >&, const QVector< float >&, const QVector< QString >& ) ),
		SLOT( on_sensors_changed( const QVector< QString >&, const QVector< float >&, const QVector< QString >& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( poseOnMap_changed( const QPointF&, double) ), SLOT(on_poseOnMap_changed(const QPointF&, double) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect(this
		, SIGNAL(set_pid_parameters_changed(unsigned int, float, float, float))
		, SLOT(on_set_pid_parameters_changed(unsigned int, float, float, float)));
	Q_ASSERT(connected);

	JSON_TOPIC_CONNECTION(smartlicense)
	JSON_TOPIC_CONNECTION(smartdockingvis)
	
	JSON_TOPIC_CONNECTION(mclayout)
	JSON_TOPIC_CONNECTION(mcstatus)
	JSON_TOPIC_CONNECTION(pathnetwork)
	JSON_TOPIC_CONNECTION(pathnetwork_edited)
	JSON_TOPIC_CONNECTION(localizationMode)
	JSON_TOPIC_CONNECTION(smartlog)
	JSON_TOPIC_CONNECTION(smartnavigationplan)
	JSON_TOPIC_CONNECTION(smartlocations)
	JSON_TOPIC_CONNECTION(smartlocations_edited)
	JSON_TOPIC_CONNECTION(smartstations)
	JSON_TOPIC_CONNECTION(smartstations_edited)
	JSON_TOPIC_CONNECTION(smartrobotinfo)
	JSON_TOPIC_CONNECTION(smartmyrobotid)
	JSON_TOPIC_CONNECTION(smartjoblist)
	JSON_TOPIC_CONNECTION(mapDir)
	JSON_TOPIC_CONNECTION(festool_charger_info)
	JSON_TOPIC_CONNECTION(fleetcom_response)
	JSON_WITH_DATA_TOPIC_CONNECTION(mapDir_response)
	JSON_WITH_DATA_TOPIC_CONNECTION(mapDir_request)

	connected &= (bool)connect(this
	, SIGNAL(fleetcom_request_changed(const QString&))
	, SLOT(on_fleetcom_request_changed(const QString&)));
	Q_ASSERT(connected);

	connected &= (bool)connect(this, SIGNAL(map_changed(const QByteArray&, int, int, float, float, float)),
	SLOT(on_map_changed(const QByteArray&, int, int, float, float, float)));
	Q_ASSERT(connected);

	connected &= (bool)connect(this, SIGNAL(mapPlanner_changed(const QByteArray&, int, int, float, float, float)),
		SLOT(on_mapPlanner_changed(const QByteArray&, int, int, float, float, float)));
	Q_ASSERT(connected);

	connected &= (bool)connect(this, SIGNAL(mapPlannerEdited_changed(const QByteArray&, int, int, float, float, float)),
		SLOT(on_mapPlannerEdited_changed(const QByteArray&, int, int, float, float, float)));
	Q_ASSERT(connected);
	
	connected &= (bool)connect( this
		, SIGNAL( json_message_with_data_changed( int, const QByteArray&,const QByteArray& ) )
		, SLOT( on_json_message_with_data_changed( int, const QByteArray&,const QByteArray& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect(this
		, SIGNAL(motor_debug_changed(int /*motor*/, const QVector<unsigned int>& /*times*/, const QVector<float>& /*speed_setpoints*/, const QVector<float>& /*speeds*/, const QVector<float>& /*currents*/, const QVector<float>& /*control_points*/))
		, SLOT(on_motor_debug_changed(int /*motor*/, const QVector<unsigned int>& /*times*/, const QVector<float>& /*speed_setpoints*/, const QVector<float>& /*speeds*/, const QVector<float>& /*currents*/, const QVector<float>& /*control_points*/)));
	Q_ASSERT(connected);
	
	connected &= (bool)connect(this
		, SIGNAL(start_motor_debug_changed(int /*motor*/))
		, SLOT(on_start_motor_debug_changed(int /*motor*/)));
	Q_ASSERT(connected);

	connected = connect(this, &Client::upload_program_changed, this, &Client::on_upload_program_changed);
	Q_ASSERT(connected);

	set_rec_robotino_rpc_set_log_level_enabled( true );
	setAutoReconnectEnabled( false );
}

bool Client::publish( const QStringList& parameters )
{
	if( parameters.size() < 2 )
	{
		Q_EMIT log( "Need at least topicName and freq", 0 );
		return false;
	}

	_publish_parameters = parameters;

	_topicName = _publish_parameters.at( 0 );
	_publish_parameters.pop_front();

	QString freqStr = _publish_parameters.at( 0 );
	_publish_parameters.pop_front();

	int freq = freqStr.toInt();

	_publish_timer = new QTimer( this );
	
	bool connected = true;
	connected &= (bool)connect( _publish_timer, SIGNAL( timeout() ), SLOT( on_publish_timer_timeout() ) );
	Q_ASSERT( connected );

	if( "once" == freqStr || 0 == freq )
	{
		_publish_timer->setSingleShot( true );
		_publish_timer->setInterval( 100 );
	}
	else
	{
		_publish_timer->setSingleShot( false );
		int ms = 1000 / freq;
		_publish_timer->setInterval( ms );
	}

	return true;
}

void Client::on_timeout()
{
	qApp->quit();
}

void Client::on_publish_timer_timeout()
{
	if( "gyroscope" == _topicName )
	{
		if( 2 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: gyroscope phi rate", 0 );
			exit( 1 );
		}
		else
		{
			double phi = _publish_parameters.at( 0 ).toDouble();
			double rate = _publish_parameters.at( 1 ).toDouble();

			if( false == set_gyroscope( phi, rate ) )
			{
				Q_EMIT log( "Error calling set_gyroscope", 0 );
				exit( 1 );
			}

			Q_EMIT log( QString( "published gyroscope %1 %2" ).arg( phi ).arg( rate ), 0 );
		}
	}
	else if ("gyroscope_ext" == _topicName)
	{
		if (2 != _publish_parameters.size())
		{
			Q_EMIT log("Expected: gyroscope_ext phi rate", 0);
			exit(1);
		}
		else
		{
			double phi = _publish_parameters.at(0).toDouble();
			double rate = _publish_parameters.at(1).toDouble();

			if (false == set_gyroscope_ext(phi, rate))
			{
				Q_EMIT log("Error calling set_gyroscope_ext", 0);
				exit(1);
			}

			Q_EMIT log(QString("published gyroscope_ext %1 %2").arg(phi).arg(rate), 0);
		}
	}
	else if( "set_shutdown" == _topicName )
	{
		if( 1 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: set_shutdown 0|1", 0 );
			exit( 1 );
		}

		if( false == set_set_shutdown( ( "0" == _publish_parameters.at( 0 ) ? false : true ) ) )
		{
			Q_EMIT log( "Error calling set_set_shutdown", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_shutdown %1" ).arg( "0" == _publish_parameters.at( 0 ) ? "false" : "true" ), 0 );
	}
	else if ("start_hotswap" == _topicName)
	{
		if (false == start_hotswap())
		{
			Q_EMIT log("Error calling start_hotswap", 0);
			exit(1);
		}

		Q_EMIT log(QString("published set_shutdown %1").arg("0" == _publish_parameters.at(0) ? "false" : "true"), 0);
	}
	else if ("start_motor_debug" == _topicName)
	{
		if (1 != _publish_parameters.size())
		{
			Q_EMIT log("Expected: start_motor_debug 0|1|2|3", 0);
			exit(1);
		}

		int motor = _publish_parameters.at(0).toInt();
		if (motor < 0 || motor>3)
		{
			Q_EMIT log("Expected: start_motor_debug 0|1|2|3", 0);
			exit(1);
		}

		if (false == start_motor_debug(motor))
		{
			Q_EMIT log("Error calling start_motor_debug", 0);
			exit(1);
		}

		Q_EMIT log(QString("published start_motor_debug %1").arg("0" == _publish_parameters.at(0) ? "false" : "true"), 0);
	}
	else if( "power_button" == _topicName )
	{
		if( 1 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: power_button 0|1", 0 );
			exit( 1 );
		}

		if( false == set_power_button( ( "0" == _publish_parameters.at( 0 ) ? false : true ) ) )
		{
			Q_EMIT log( "Error calling set_power_button", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published power_button %1" ).arg( "0" == _publish_parameters.at( 0 ) ? "false" : "true" ), 0 );
	}
	else if( "request_shutdown" == _topicName )
	{
		if( 1 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: request_shutdown 0|1", 0 );
			exit( 1 );
		}

		if( false == set_request_shutdown( ( "0" == _publish_parameters.at( 0 ) ? false : true ) ) )
		{
			Q_EMIT log( "Error calling set_request_shutdown", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published request_shutdown %1" ).arg( "0" == _publish_parameters.at( 0 ) ? "false" : "true" ), 0 );
	}
	else if( QRegExp( "motor\\d_setpoint" ).exactMatch( _topicName ) )
	{
		QString str = _topicName;
		str.remove( "motor" );
		str.remove( "_setpoint" );
		unsigned int number = str.toUInt();

		if( number > 3 )
		{
			Q_EMIT log( "motor number out of range", 0 );
			exit( 1 );
		}

		if( 1 != _publish_parameters.size() )
		{
			Q_EMIT log( QString( "Expected: motor%1_setpoint speed" ).arg( number ), 0 );
			exit( 1 );
		}

		float speed = _publish_parameters.at( 0 ).toFloat();

		switch( number )
		{
		case 0:
			if( false == set_motor0_setpoint( speed ) )
			{
				Q_EMIT log( "Error calling set_motor0_setpoint", 0 );
				exit( 1 );
			}
			break;

		case 1:
			if( false == set_motor1_setpoint( speed ) )
			{
				Q_EMIT log( "Error calling set_motor1_setpoint", 0 );
				exit( 1 );
			}
			break;

		case 2:
			if( false == set_motor2_setpoint( speed ) )
			{
				Q_EMIT log( "Error calling set_motor2_setpoint", 0 );
				exit( 1 );
			}
			break;

		case 3:
			if( false == set_motor3_setpoint( speed ) )
			{
				Q_EMIT log( "Error calling set_motor3_setpoint", 0 );
				exit( 1 );
			}
			break;
		}

		Q_EMIT log( QString( "published set_motor%1_setpoint %2" ).arg( number ).arg( speed ), 0 );
	}
	else if( QRegExp( "motor\\d_reset_position" ).exactMatch( _topicName ) )
	{
		QString str = _topicName;
		str.remove( "motor" );
		str.remove( "_reset_position" );
		unsigned int number = str.toUInt();

		if( number > 3 )
		{
			Q_EMIT log( "motor number out of range", 0 );
			exit( 1 );
		}

		if( 1 != _publish_parameters.size() )
		{
			Q_EMIT log( QString( "Expected: motor%1_reset_position newPosition" ).arg( number ), 0 );
			exit( 1 );
		}

		int pos = _publish_parameters.at( 0 ).toInt();

		switch( number )
		{
		case 0:
			if( false == set_motor0_reset_position( pos ) )
			{
				Q_EMIT log( "Error calling set_motor0_reset_position", 0 );
				exit( 1 );
			}
			break;

		case 1:
			if( false == set_motor1_reset_position( pos ) )
			{
				Q_EMIT log( "Error calling set_motor1_reset_position", 0 );
				exit( 1 );
			}
			break;

		case 2:
			if( false == set_motor2_reset_position( pos ) )
			{
				Q_EMIT log( "Error calling set_motor2_reset_position", 0 );
				exit( 1 );
			}
			break;

		case 3:
			if( false == set_motor3_reset_position( pos ) )
			{
				Q_EMIT log( "Error calling set_motor3_reset_position", 0 );
				exit( 1 );
			}
			break;
		}

		Q_EMIT log( QString( "published set_motor%1_reset_position %2" ).arg( number ).arg( pos ), 0 );
	}
	else if( QRegExp( "set_motor\\d_mode" ).exactMatch( _topicName ) )
	{
		QString str = _topicName;
		str.remove( "set_motor" );
		str.remove( "_mode" );
		unsigned int number = str.toUInt();

		if( number > 3 )
		{
			Q_EMIT log( "motor number out of range", 0 );
			exit( 1 );
		}

		if( 1 != _publish_parameters.size() )
		{
			Q_EMIT log( QString( "Expected: set_motor%1_mode mode" ).arg( number ), 0 );
			exit( 1 );
		}

		QString mode = _publish_parameters.at( 0 );

		switch( number )
		{
		case 0:
			if( false == set_set_motor0_mode( mode ) )
			{
				Q_EMIT log( "Error calling set_set_motor0_mode", 0 );
				exit( 1 );
			}
			break;

		case 1:
			if( false == set_set_motor1_mode( mode ) )
			{
				Q_EMIT log( "Error calling set_set_motor1_mode", 0 );
				exit( 1 );
			}
			break;

		case 2:
			if( false == set_set_motor2_mode( mode ) )
			{
				Q_EMIT log( "Error calling set_set_motor2_mode", 0 );
				exit( 1 );
			}
			break;

		case 3:
			if( false == set_set_motor3_mode( mode ) )
			{
				Q_EMIT log( "Error calling set_set_motor3_mode", 0 );
				exit( 1 );
			}
			break;
		}

		Q_EMIT log( QString( "published set_motor%1_mode %2" ).arg( number ).arg( mode ), 0 );
	}
	else if( "set_pid_parameters" == _topicName )
	{
		if( 4 != _publish_parameters.size() )
		{
			Q_EMIT log( QString( "Expected: set_pid_parameters motor kp ki kid" ), 0 );
			exit( 1 );
		}

		int motor = _publish_parameters.at( 0 ).toInt();
		float kp = _publish_parameters.at( 1 ).toFloat();
		float ki = _publish_parameters.at( 2 ).toFloat();
		float kd = _publish_parameters.at( 3 ).toFloat();

		if( motor > 3 || motor < 0 )
		{
			Q_EMIT log( "motor number out of range", 0 );
			exit( 1 );
		}

		if( false == set_set_pid_parameters( motor, kp, ki, kd ) )
		{
			Q_EMIT log( "Error calling set_set_pid_parameters", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_pid_parameters %1 %2 %3 %4" ).arg( motor ).arg( kp ).arg( ki ).arg( kd ), 0 );
	}
	else if( "motor_setpoints" == _topicName )
	{
		if( 3 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: motor_setpoints m0 m1 m2", 0 );
			exit( 1 );
		}

		float m0 = _publish_parameters.at( 0 ).toFloat();
		float m1 = _publish_parameters.at( 1 ).toFloat();
		float m2 = _publish_parameters.at( 2 ).toFloat();
		if( false == set_motor_setpoints( m0, m1, m2 ) )
		{
			Q_EMIT log( "Error calling set_motor_setpoints", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_motor_setpoints %1 %2 %3" ).arg( m0 ).arg( m1 ).arg( m1 ), 0 );
	}
	else if( "emergency_bumper" == _topicName )
	{
		if( 1 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: emergency_bumper 0|1", 0 );
			exit( 1 );
		}

		if( false == set_emergency_bumper( ( "0" == _publish_parameters.at( 0 ) ? false : true ) ) )
		{
			Q_EMIT log( "Error calling set_emergency_bumper", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published emergency_bumper %1" ).arg( "0" == _publish_parameters.at( 0 ) ? "false" : "true" ), 0 );
	}
	else if( "set_emergency_bumper" == _topicName )
	{
		if( 1 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: set_emergency_bumper 0|1", 0 );
			exit( 1 );
		}

		if( false == set_set_emergency_bumper( ( "0" == _publish_parameters.at( 0 ) ? false : true ) ) )
		{
			Q_EMIT log( "Error calling set_set_emergency_bumper", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_emergency_bumper %1" ).arg( "0" == _publish_parameters.at( 0 ) ? "false" : "true" ), 0 );
	}
	else if( "ea09_ip4address" == _topicName )
	{
		if( 2 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: ea09_ip4address address netmask", 0 );
			exit( 1 );
		}

		QHostAddress addr( _publish_parameters.at( 0 ) );
		QHostAddress mask( _publish_parameters.at( 1 ) );
		if( false == set_ea09_ip4address( addr, mask ) )
		{
			Q_EMIT log( "Error calling set_ea09_ip4address", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published ea09_ip4address %1 %2" ).arg( addr.toString() ).arg( mask.toString() ), 0 );
	}
	else if( "set_ea09_ip4address" == _topicName )
	{
		if( 2 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: set_ea09_ip4address address netmask", 0 );
			exit( 1 );
		}

		QHostAddress addr( _publish_parameters.at( 0 ) );
		QHostAddress mask( _publish_parameters.at( 1 ) );
		if( false == set_set_ea09_ip4address( addr, mask ) )
		{
			Q_EMIT log( "Error calling set_set_ea09_ip4address", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_ea09_ip4address %1 %2" ).arg( addr.toString() ).arg( mask.toString() ), 0 );
	}
	else if( QRegExp( "camera\\d_settings" ).exactMatch( _topicName ) )
	{
		QString str = _topicName;
		str.remove( "camera" );
		str.remove( "_settings" );
		unsigned int number = str.toUInt();

		if( number > 3 )
		{
			Q_EMIT log( "camera number out of range", 0 );
			exit( 1 );
		}

		if( 3 != _publish_parameters.size() )
		{
			Q_EMIT log( QString( "Expected: %1 width height format" ).arg( _topicName ), 0 );
			exit( 1 );
		}

		unsigned int width = _publish_parameters.at( 0 ).toUInt();
		unsigned int height = _publish_parameters.at( 1 ).toUInt();
		QString format = _publish_parameters.at( 2 );

		if( false == set_camera_settings( number, width, height, format ) )
		{
			exit( 1 );
		}

		Q_EMIT log( QString( "published %1 %2 %3 %4" ).arg( _topicName ).arg( width ).arg( height ).arg( format ), 0 );
	}
	else if( QRegExp( "set_camera\\d_settings" ).exactMatch( _topicName ) )
	{
		QString str = _topicName;
		str.remove( "set_camera" );
		str.remove( "_settings" );
		unsigned int number = str.toUInt();

		if( number > 3 )
		{
			Q_EMIT log( "camera number out of range", 0 );
			exit( 1 );
		}

		if( 3 != _publish_parameters.size() )
		{
			Q_EMIT log( QString( "Expected: %1 width height format" ).arg( _topicName ), 0 );
			exit( 1 );
		}

		unsigned int width = _publish_parameters.at( 0 ).toUInt();
		unsigned int height = _publish_parameters.at( 1 ).toUInt();
		QString format = _publish_parameters.at( 2 );

		if( false == set_set_camera_settings( number, width, height, format ) )
		{
			exit( 1 );
		}

		Q_EMIT log( QString( "published %1 %2 %3 %4" ).arg( _topicName ).arg( width ).arg( height ).arg( format ), 0 );
	}
	else if( QRegExp( "set_camera\\d_control" ).exactMatch( _topicName ) )
	{
		QString str = _topicName;
		str.remove( "set_camera" );
		str.remove( "_control" );
		unsigned int number = str.toUInt();

		if( number > 3 )
		{
			Q_EMIT log( "camera number out of range", 0 );
			exit( 1 );
		}

		if( 2 != _publish_parameters.size() )
		{
			Q_EMIT log( QString( "Expected: %1 name value" ).arg( _topicName ), 0 );
			exit( 1 );
		}

		if( false == set_set_camera_control( number, _publish_parameters.at( 0 ), _publish_parameters.at( 1 ).toUInt() ) )
		{
			exit( 1 );
		}

		Q_EMIT log( QString( "published %1 %2 %3 %4" ).arg( _topicName ).arg( _publish_parameters.at( 0 ) ).arg( _publish_parameters.at( 1 ) ), 0 );
	}
	else if( "omnidrive" == _topicName )
	{
		if( 3 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: omnidrive vx vy omega", 0 );
			exit( 1 );
		}

		float vx = _publish_parameters.at( 0 ).toFloat();
		float vy = _publish_parameters.at( 1 ).toFloat();
		float omega = _publish_parameters.at( 2 ).toFloat();

		if( false == set_omnidrive( vx, vy, omega ) )
		{
			Q_EMIT log( "Error calling set_omnidrive", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published omnidrive %1 %2 %3" ).arg( vx ).arg( vy ).arg( omega ), 0 );
	}
	else if( "digital_input" == _topicName )
	{
		if( 8 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: digital_input din1 din2 ... din8", 0 );
			exit( 1 );
		}

		QVector< bool > din;
		Q_FOREACH( const QString& str, _publish_parameters )
		{
			din << ( str.toInt() > 0 ? true : false );
		}

		if( false == set_digital_input( din ) )
		{
			Q_EMIT log( "Error calling set_digital_input", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_digital_input %1 %2 %3 %4 %5 %6 %7 %8" ).arg( din[0] ).arg( din[1] ).arg( din[2] ).arg( din[3] ).arg( din[4] ).arg( din[5] ).arg( din[6] ).arg( din[7] ), 0 );
	}
	else if( "analog_input" == _topicName )
	{
		if( 8 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: analog_input ain1 ain2 ... ain8", 0 );
			exit( 1 );
		}

		QVector< float > ain;
		Q_FOREACH( const QString& str, _publish_parameters )
		{
			ain << str.toFloat();
		}

		if( false == set_analog_input( ain ) )
		{
			Q_EMIT log( "Error calling set_analog_input", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_analog_input %1 %2 %3 %4 %5 %6 %7 %8" ).arg( ain[0] ).arg( ain[1] ).arg( ain[2] ).arg( ain[3] ).arg( ain[4] ).arg( ain[5] ).arg( ain[6] ).arg( ain[7] ), 0 );
	}
	else if( "distance_sensors" == _topicName )
	{
		if( 9 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: distance_sensors ir1 ir2 ... ir9", 0 );
			exit( 1 );
		}

		QVector< float > ain;
		Q_FOREACH( const QString& str, _publish_parameters )
		{
			ain << str.toFloat();
		}

		if( false == set_distance_sensors( ain ) )
		{
			Q_EMIT log( "Error calling set_distance_sensors", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_distance_sensors %1 %2 %3 %4 %5 %6 %7 %8 %9" ).arg( ain[0] ).arg( ain[1] ).arg( ain[2] ).arg( ain[3] ).arg( ain[4] ).arg( ain[5] ).arg( ain[6] ).arg( ain[7] ).arg( ain[8] ), 0 );
	}
	else if( "bumper" == _topicName )
	{
		if( 1 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: bumper value", 0 );
			exit( 1 );
		}

		bool bump = _publish_parameters.at( 0 ).toInt() > 0;

		if( false == set_bumper( bump ) )
		{
			Q_EMIT log( "Error calling set_bumper", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_bumper %1" ).arg( bump ? "hit" : "not hit" ), 0 );
	}
	else if( "battery" == _topicName )
	{
		if( 7 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: battery voltage current ext_power(0,1) num_chargers batteryType(0=Pb,1=NiMH) batteryLow(0,1) batteryLowShutdownCounter", 0 );
			exit( 1 );
		}

		float voltage = _publish_parameters.at( 0 ).toFloat();
		float current = _publish_parameters.at( 1 ).toFloat();
		bool ext_power = _publish_parameters.at( 2 ).toInt() > 0;
		int num_chargers = _publish_parameters.at( 3 ).toInt();
		QString batteryType = (_publish_parameters.at( 4 ).toInt()>0?"NiMH":"Pb");
		bool batteryLow = _publish_parameters.at( 5 ).toInt() > 0;
		int batteryLowShutdownCounter = _publish_parameters.at( 6 ).toInt();

		if( false == set_battery( voltage, current, ext_power, num_chargers, batteryType, batteryLow, batteryLowShutdownCounter) )
		{
			Q_EMIT log( "Error calling set_battery", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_battery"
			"\nVoltage: %1"
			"\nCurrent: %2"
			"\nExternal power: %3"
			"\nNum chargers: %4"
			"\nBattery type: %5"
			"\nBattery low: %6"
			"\nBattery shutdown counter: %7" )
			.arg( voltage )
			.arg( current )
			.arg( ext_power?"yes":"no" )
			.arg( num_chargers )
			.arg( batteryType )
			.arg( batteryLow?"yes":"no" )
			.arg( batteryLowShutdownCounter )
			, 0 );
	}
	else if( QRegExp( "charger\\d_info" ).exactMatch( _topicName ) )
	{
		QString str = _topicName;
		str.remove( "charger" );
		str.remove( "_info" );
		unsigned int number = str.toUInt();

		if( number > 2 )
		{
			Q_EMIT log( "charger number out of range", 0 );
			exit( 1 );
		}

		if( 7 != _publish_parameters.size() )
		{
			Q_EMIT log( QString( "Expected: %1 time batteryVoltage chargingCurrent bat1temp bat2temp state_number stateString" ).arg( _topicName ), 0 );
			exit( 1 );
		}

		unsigned int time = _publish_parameters.at(0).toUInt();
		float batteryVoltage = _publish_parameters.at(1).toFloat();
		float chargingCurrent = _publish_parameters.at(2).toFloat();
		float bat1temp = _publish_parameters.at(3).toFloat();
		float bat2temp = _publish_parameters.at(4).toFloat();
		int state_number = _publish_parameters.at(5).toInt();
		QString state = _publish_parameters.at(6);

		bool ok = false;
		switch( number )
		{
		case 0:
			ok = set_charger0_info( time, batteryVoltage, chargingCurrent, bat1temp, bat2temp, state_number, state );
			break;

		case 1:
			ok = set_charger1_info( time, batteryVoltage, chargingCurrent, bat1temp, bat2temp, state_number, state );
			break;

		case 2:
			ok = set_charger2_info( time, batteryVoltage, chargingCurrent, bat1temp, bat2temp, state_number, state );
			break;

		default:
			break;
		}

		if( false == ok )
		{
			exit( 1 );
		}

		Q_EMIT log( QString( "published %1 %2 %3 %4 %5 %6 %7 %8" )
			.arg( _topicName )
			.arg( time )
			.arg(batteryVoltage )
			.arg(chargingCurrent )
			.arg(bat1temp )
			.arg(bat2temp )
			.arg(state_number )
			.arg(state )
			, 0 );
	}
	else if( "display_text" == _topicName )
	{
		if( 1 > _publish_parameters.size() || 5 < _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: display_text text row col clear_before clear_after", 0 );
			exit( 1 );
		}

		QString text = _publish_parameters.at( 0 );
		text.replace( "\\n", "\n" );
		unsigned int row = 0;
		unsigned int col = 0;
		bool clear_before = true;
		bool clear_after = true;

		if( _publish_parameters.size() > 1 )
		{
			row = _publish_parameters.at( 1 ).toUInt();
		}
		if( _publish_parameters.size() > 2 )
		{
			col = _publish_parameters.at( 2 ).toUInt();
		}
		if( _publish_parameters.size() > 3 )
		{
			clear_before = ( _publish_parameters.at( 3 ).toInt() > 0 );
		}
		if( _publish_parameters.size() > 4 )
		{
			clear_after = ( _publish_parameters.at( 4 ).toInt() > 0 );
		}

		if( false == set_display_text( text, row, col, clear_before, clear_after ) )
		{
			Q_EMIT log( "Error calling set_display_text", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_display_text\n%1\nr=%2 c=%3 cb=%4 ca=%5" ).arg( text ).arg( row ).arg( col ).arg( clear_before ? "1" : "0" ).arg( clear_after ? "1" : "0" ), 0 );
	}
	else if( "odometry" == _topicName )
	{
		if( 3 != _publish_parameters.size() && 6 != _publish_parameters.size() && 7 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: odometry x[m] y[m] phi[deg] [vx[m/s] vy[m/s] omega[rad/s] [seq]]", 0 );
			exit( 1 );
		}

		double x = _publish_parameters.at( 0 ).toDouble();
		double y = _publish_parameters.at( 1 ).toDouble();
		double phideg =_publish_parameters.at( 2 ).toDouble();
		float vx = 0.0f;
		float vy = 0.0f;
		float omega = 0.0f;
		if( 6 == _publish_parameters.size() )
		{
			vx = _publish_parameters.at( 3 ).toFloat();
			vy = _publish_parameters.at( 4 ).toFloat();
			omega = _publish_parameters.at( 5 ).toFloat();
		}
		unsigned int seq = 0;
		if( 7 == _publish_parameters.size() )
		{
			seq = _publish_parameters.at( 6 ).toUInt();
		}
		double phi = M_PI * phideg / 180.0;

		if( false == set_odometry( x, y, phi, vx, vy, omega, seq ) )
		{
			Q_EMIT log( "Error calling set_odometry", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published odometry %1 %2 %3" ).arg( x ).arg( y ).arg( phideg ), 0 );
	}
	else if( "set_odometry" == _topicName )
	{
		if( 3 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: set_odometry x[m] y[m] phi[deg]", 0 );
			exit( 1 );
		}

		double x = _publish_parameters.at( 0 ).toDouble();
		double y = _publish_parameters.at( 1 ).toDouble();
		double phideg =_publish_parameters.at( 2 ).toDouble();
		double phi = M_PI * phideg / 180.0;

		if( false == set_set_odometry( x, y, phi ) )
		{
			Q_EMIT log( "Error calling set_set_odometry", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_odometry %1 %2 %3" ).arg( x ).arg( y ).arg( phideg ), 0 );
	}
	else if ("pose" == _topicName)
	{
		if (7 != _publish_parameters.size())
		{
			Q_EMIT log("Expected: pose x[m] y[m] phi[deg] errx erry errphi sequence", 0);
			exit(1);
		}

		double x = _publish_parameters.at(0).toDouble();
		double y = _publish_parameters.at(1).toDouble();
		double phideg = _publish_parameters.at(2).toDouble();
		double phi = M_PI * phideg / 180.0;
		double errx = _publish_parameters.at(3).toDouble();
		double erry = _publish_parameters.at(4).toDouble();
		double errphi = _publish_parameters.at(5).toDouble();
		double sequence = _publish_parameters.at(6).toDouble();

		if (false == set_pose(x, y, phi,errx,erry,errphi,sequence))
		{
			Q_EMIT log("Error calling set_pose", 0);
			exit(1);
		}

		Q_EMIT log(QString("published set_pose %1 %2 %3 %4 %5 %6 %7").arg(x).arg(y).arg(phideg).arg(errx).arg(erry).arg(errphi).arg(sequence), 0);
	}
	else if( "log" == _topicName )
	{
		if( 2 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: log message level", 0 );
			exit( 1 );
		}

		QString message = _publish_parameters.at( 0 );
		int level = _publish_parameters.at( 1 ).toInt();

		if( false == set_log( message, level ) )
		{
			Q_EMIT log( "Error calling set_log", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published log %1 %2" ).arg( message ).arg( level ), 0 );
	}
	else if( "set_log_level" == _topicName )
	{
		if( 2 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: set_log_level publisher level", 0 );
			exit( 1 );
		}

		QString publisher = _publish_parameters.at( 0 );
		int level = _publish_parameters.at( 1 ).toInt();

		if( false == set_set_log_level( publisher, level ) )
		{
			Q_EMIT log( "Error calling set_set_log_level", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_log_level %1 %2" ).arg( publisher ).arg( level ), 0 );
	}
	else if( "custom_message" == _topicName )
	{
		if( 2 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: custom_message id data", 0 );
			exit( 1 );
		}

		unsigned int id = _publish_parameters.at( 0 ).toUInt();
		const QString& data = _publish_parameters.at( 1 );

		if( false == set_custom_message( id, data.toLatin1() ) )
		{
			Q_EMIT log( "Error calling set_custom_message", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_custom_message %1 %2" ).arg( id ).arg( data ), 0 );
	}
	else if( _topicName.contains( QRegExp( "^custom_message[0-3]{1,1}$" ) ) )
	{
		QString idStr = _topicName;
		idStr.remove( "custom_message" );
		int id = idStr.toInt();

		if( 1 != _publish_parameters.size() )
		{
			Q_EMIT log( QString( "Expected: custom_message%1 data" ).arg( id ), 0 );
			exit( 1 );
		}

		const QString& data = _publish_parameters.at( 0 );

		bool ret = true;
		switch( id )
		{
		case 0:
			ret = set_custom_message0( data.toLatin1() );
			break;

		case 1:
			ret = set_custom_message1( data.toLatin1() );
			break;

		case 2:
			ret = set_custom_message2( data.toLatin1() );
			break;

		case 3:
			ret = set_custom_message3( data.toLatin1() );
			break;

		default:
			Q_ASSERT( false );
		}

		if( false == ret )
		{
			Q_EMIT log( QString( "Error calling set_custom_message%1" ).arg( id ), 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_custom_message%1 %2" ).arg( id ).arg( data ), 0 );
	}
	else if( _topicName.contains( QRegExp( "^kinect[0-3]{1,1}_set_tilt$" ) ) )
	{
		QString idStr = _topicName;
		idStr.remove( "kinect" );
		idStr.remove( "_set_tilt" );
		int id = idStr.toInt();

		if( 1 != _publish_parameters.size() )
		{
			Q_EMIT log( QString( "Expected: kinect%1_set_tilt degrees" ).arg( id ), 0 );
			exit( 1 );
		}

		int deg = _publish_parameters.at( 0 ).toInt();

		if( false == set_kinect_set_tilt( id, deg ) )
		{
			Q_EMIT log( QString( "Error calling set_kinect%1_set_tilt" ).arg( id ), 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_kinect%1_set_tilt %2" ).arg( id ).arg( deg ), 0 );
	}
	else if( _topicName.contains( QRegExp( "^kinect[0-3]{1,1}_set_led$" ) ) )
	{
		QString idStr = _topicName;
		idStr.remove( "kinect" );
		idStr.remove( "_set_led" );
		int id = idStr.toInt();

		if( 1 != _publish_parameters.size() )
		{
			Q_EMIT log( QString( "Expected: kinect%1_set_led state" ).arg( id ), 0 );
			exit( 1 );
		}

		unsigned int state = _publish_parameters.at( 0 ).toUInt();

		if( false == set_kinect_set_led( id, state ) )
		{
			Q_EMIT log( QString( "Error calling set_kinect%1_set_led" ).arg( id ), 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_kinect%1_set_led %2" ).arg( id ).arg( state ), 0 );
	}
	else if( _topicName.contains( QRegExp( "^kinect[0-3]{1,1}_set_video_format$" ) ) )
	{
		QString idStr = _topicName;
		idStr.remove( "kinect" );
		idStr.remove( "_set_video_format" );
		int id = idStr.toInt();

		if( 1 != _publish_parameters.size() )
		{
			Q_EMIT log( QString( "Expected: kinect%1_set_video_format format" ).arg( id ), 0 );
			exit( 1 );
		}

		unsigned int state = _publish_parameters.at( 0 ).toUInt();

		if( false == set_kinect_set_video_format( id, state ) )
		{
			Q_EMIT log( QString( "Error calling set_kinect%1_set_video_format" ).arg( id ), 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_kinect%1_set_video_format %2" ).arg( id ).arg( state ), 0 );
	}
	else if( _topicName.contains( QRegExp( "^kinect[0-3]{1,1}_set_depth_format$" ) ) )
	{
		QString idStr = _topicName;
		idStr.remove( "kinect" );
		idStr.remove( "_set_depth_format" );
		int id = idStr.toInt();

		if( 1 != _publish_parameters.size() )
		{
			Q_EMIT log( QString( "Expected: kinect%1_set_depth_format format" ).arg( id ), 0 );
			exit( 1 );
		}

		unsigned int state = _publish_parameters.at( 0 ).toUInt();

		if( false == set_kinect_set_depth_format( id, state ) )
		{
			Q_EMIT log( QString( "Error calling set_kinect%1_set_depth_format" ).arg( id ), 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_kinect%1_set_depth_format %2" ).arg( id ).arg( state ), 0 );
	}
	else if( "grappler_servos" == _topicName )
	{
		if( _publish_parameters.empty() || 0 != _publish_parameters.size() %4 )
		{
			Q_EMIT log( "Expected: grappler_servos channel id cwlimit ccwlimit [channel id cwlimit ccwlimit [channel id cwlimit ccwlimit [...]]]", 0 );
			exit( 1 );
		}
		QVector< rec::robotino::rpc::GrapplerServoInfo> data;

		for( int i=0; i<_publish_parameters.size(); i+=4 )
		{
			rec::robotino::rpc::GrapplerServoInfo info;
			info.channel = _publish_parameters.at(i).toInt();
			info.id = _publish_parameters.at(i+1).toInt();
			info.cwAxisLimit = _publish_parameters.at(i+2).toFloat();
			info.ccwAxisLimit = _publish_parameters.at(i+3).toFloat();
			data << info;
		}

		if( false == set_grappler_servos( data ) )
		{
			Q_EMIT log( "Error calling set_grappler_servos", 0 );
			exit( 1 );
		}

		QString str = "published grappler_servos";
		for( int i=0; i<data.size(); ++i )
		{
			const rec::robotino::rpc::GrapplerServoInfo& info = data[i];
			str += QString("\n*** channel=%1 id=%2 found=%3 ***").arg( info.channel ).arg( info.id ).arg( info.found ? "true" : "false" );
			str += QString("\nangle=%1 speed=%2 error=%3").arg( info.angle ).arg( info.speed ).arg( info.error );
			str += QString("\ncwAxisLimit=%1 ccwAxisLimit=%2").arg( info.cwAxisLimit ).arg( info.ccwAxisLimit );
		}

		Q_EMIT log( str, 0 );
	}
	else if( "grappler_readings" == _topicName )
	{
		if( _publish_parameters.empty() || 0 != _publish_parameters.size() %4 )
		{
			Q_EMIT log( "Expected: grappler_readings channel id angle speed [channel id angle speed [channel id angle speed [...]]]", 0 );
			exit( 1 );
		}
		QVector< rec::robotino::rpc::GrapplerServoInfo> data;

		for( int i=0; i<_publish_parameters.size(); i+=4 )
		{
			rec::robotino::rpc::GrapplerServoInfo info;
			info.channel = _publish_parameters.at(i).toInt();
			info.id = _publish_parameters.at(i+1).toInt();
			info.angle = _publish_parameters.at(i+2).toFloat();
			info.speed = _publish_parameters.at(i+3).toFloat();
			data << info;
		}

		if( false == set_grappler_readings( data ) )
		{
			Q_EMIT log( "Error calling set_grappler_readings", 0 );
			exit( 1 );
		}

		QString str = "published set_grappler_readings";
		for( int i=0; i<data.size(); ++i )
		{
			const rec::robotino::rpc::GrapplerServoInfo& info = data[i];
			str += QString("\n*** channel=%1 id=%2 found=%3 ***").arg( info.channel ).arg( info.id ).arg( info.found ? "true" : "false" );
			str += QString("\nangle=%1 speed=%2 error=%3").arg( info.angle ).arg( info.speed ).arg( info.error );
			str += QString("\ncwAxisLimit=%1 ccwAxisLimit=%2").arg( info.cwAxisLimit ).arg( info.ccwAxisLimit );
		}

		Q_EMIT log( str, 0 );
	}
	else if( "grappler_set_positions" == _topicName )
	{
		if( _publish_parameters.empty() || 0 != _publish_parameters.size() %2 )
		{
			Q_EMIT log( "Expected: grappler_set_positions angle1 speed1 [angle2 speed2 [angle3 speed3 [...]]]", 0 );
			exit( 1 );
		}
		QVector< rec::robotino::rpc::GrapplerServoInfo> vec;
		QString str;

		QStringList::const_iterator iter = _publish_parameters.constBegin();
		while( _publish_parameters.constEnd() != iter )
		{
			rec::robotino::rpc::GrapplerServoInfo i;
			i.angle = (*iter++).toFloat();
			i.speed = (*iter++).toFloat();
			vec << i;

			str += QString("\n%1 %2").arg( i.angle ).arg( i.speed );
		}

		if( false == set_grappler_set_positions( vec ) )
		{
			Q_EMIT log( "Error calling set_grappler_set_positions", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published grappler_set_positions %1" ).arg( str ), 0 );
	}
	else if( "grappler_set_position" == _topicName )
	{
		if( 4 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: grappler_set_position channel id angle speed", 0 );
			exit( 1 );
		}
		QVector< rec::robotino::rpc::GrapplerServoInfo> vec;
		rec::robotino::rpc::GrapplerServoInfo i;
		i.channel = _publish_parameters.at(0).toInt();
		i.id = _publish_parameters.at(1).toInt();
		i.angle = _publish_parameters.at(2).toInt();
		i.speed = _publish_parameters.at(3).toInt();
		vec << i;

		if( false == set_grappler_set_positions( vec ) )
		{
			Q_EMIT log( "Error calling set_grappler_set_positions", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published grappler_set_position %1 %2 %3 %4" ).arg( i.channel ).arg( i.id ).arg( i.angle ).arg( i.speed ), 0 );
	}
	else if( "grappler_set_power" == _topicName )
	{
		if( 2 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: grappler_set_power line power", 0 );
			exit( 1 );
		}
		
		int line = _publish_parameters.at(0).toInt();
		bool power = ( _publish_parameters.at(1).toInt() > 0 );

		if( false == set_grappler_set_power( line, power ) )
		{
			Q_EMIT log( "Error calling set_grappler_set_power", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published grappler_set_power %1 %2" ).arg( line ).arg( power ? "on" : "off" ), 0 );
	}
	else if( "grappler_toggle_torque" == _topicName )
	{
		if( 0 != _publish_parameters.size() )
		{
			Q_EMIT log( "Expected: grappler_toggle_torque", 0 );
			exit( 1 );
		}
		
		if( false == set_grappler_toggle_torque() )
		{
			Q_EMIT log( "Error calling set_grappler_toggle_torque", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published grappler_toggle_torque" ), 0 );
	}
	else if( "set_digital_output" == _topicName )
	{
		if( 2 != _publish_parameters.size() )
		{
			Q_EMIT log( QString( "Expected: set_digital_output index state" ), 0 );
			exit( 1 );
		}

		int index = _publish_parameters.at( 0 ).toInt();
		bool state = (_publish_parameters.at( 1 ).toInt() != 0 );

		if( index > 7 || index < 0 )
		{
			Q_EMIT log( "index out of range", 0 );
			exit( 1 );
		}

		if( false == set_set_digital_output( index, state ) )
		{
			Q_EMIT log( "Error calling set_set_digital_output", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_digital_output %1 %2" ).arg( index ).arg( state ? 1 : 0 ), 0 );
	}
	else if( "set_digital_output_array" == _topicName )
	{
		if( 1 != _publish_parameters.size() )
		{
			Q_EMIT log( QString( "Expected: set_digital_output_array data" ), 0 );
			exit( 1 );
		}

		int data = _publish_parameters.at( 0 ).toInt();

		QVector<bool> vec( 8 , false );
		for( int i=0; i<vec.size(); ++i )
		{
			if( data & 1<<i )
			{
				vec[i] = true;
			}
		}

		if( false == set_set_digital_output_array( vec ) )
		{
			Q_EMIT log( "Error calling set_set_digital_output_array", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_digital_output_array %1" ).arg( data ), 0 );
	}
	else if( "set_relay" == _topicName )
	{
		if( 2 != _publish_parameters.size() )
		{
			Q_EMIT log( QString( "Expected: set_relay index state" ), 0 );
			exit( 1 );
		}

		int index = _publish_parameters.at( 0 ).toInt();
		bool state = (_publish_parameters.at( 1 ).toInt() != 0 );

		if( index > 7 || index < 0 )
		{
			Q_EMIT log( "index out of range", 0 );
			exit( 1 );
		}

		if( false == set_set_relay( index, state ) )
		{
			Q_EMIT log( "Error calling set_set_relay", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_relay %1 %2" ).arg( index ).arg( state ? 1 : 0 ), 0 );
	}
	else if( "set_relay_array" == _topicName )
	{
		if( 1 != _publish_parameters.size() )
		{
			Q_EMIT log( QString( "Expected: set_relay_array data" ), 0 );
			exit( 1 );
		}

		int data = _publish_parameters.at( 0 ).toInt();

		QVector<bool> vec( 2 , false );
		for( int i=0; i<vec.size(); ++i )
		{
			if( data & 1<<i )
			{
				vec[i] = true;
			}
		}

		if( false == set_set_relay_array( vec ) )
		{
			Q_EMIT log( "Error calling set_set_relay_array", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published set_relay_array %1" ).arg( data ), 0 );
	}
	else if( QRegExp( "charger\\d_clear_error" ).exactMatch( _topicName ) )
	{
		QString str = _topicName;
		str.remove( "charger" );
		str.remove( "_clear_error" );
		unsigned int number = str.toUInt();

		if( number > 2 )
		{
			Q_EMIT log( "charger number out of range", 0 );
			exit( 1 );
		}

		if( 0 != _publish_parameters.size() )
		{
			Q_EMIT log( QString( "Expected: charger%1_clear_error" ), 0 );
			exit( 1 );
		}

		switch( number )
		{
		case 0:
			if( false == set_charger0_clear_error() )
			{
				Q_EMIT log( "Error calling set_charger0_clear_error", 0 );
				exit( 1 );
			}
			break;

		case 1:
			if( false == set_charger1_clear_error() )
			{
				Q_EMIT log( "Error set_charger1_clear_error", 0 );
				exit( 1 );
			}
			break;

		case 2:
			if( false == set_charger2_clear_error() )
			{
				Q_EMIT log( "Error set_charger2_clear_error", 0 );
				exit( 1 );
			}
			break;
		}

		Q_EMIT log( QString( "published charger%1_clear_error" ).arg( number ), 0 );
	}
	else if( "gripper_state" == _topicName )
	{
		if( 1 != _publish_parameters.size() )
		{
			Q_EMIT log( QString( "Expected: gripper_state moving|open|closed" ), 0 );
			exit( 1 );
		}

		int state = 0;
		QString stateStr = _publish_parameters.at( 0 );

		if( "moving" == stateStr )
		{
			state = 0;
		}
		else if( "open" == stateStr )
		{
			state = 1;
		}
		else if( "closed" == stateStr )
		{
			state = 2;
		}
		else
		{
			Q_EMIT log( QString( "Expected: gripper_state moving|open|closed" ), 0 );
			exit( 1 );
		}

		if( false == set_gripper_state( state ) )
		{
			Q_EMIT log( "Error calling set_gripper_state", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published gripper_state %1" ).arg( stateStr ), 0 );
	}
	else if( "map" == _topicName )
	{
		if( 6 != _publish_parameters.size() )
		{
			Q_EMIT log( QString( "Expected: map filename width height resolution offsetx offsety" ), 0 );
			exit( 1 );
		}

		QByteArray data;
		QString filename = _publish_parameters.at( 0 );
		QFile file( filename );
		if( file.open( QIODevice::ReadOnly ) )
		{
			data = file.readAll();
			file.close();
		}
		else
		{
			Q_EMIT log( QString( "Error loading %1" ).arg( filename ), 0 );
			exit( 1 );
		}

		float w = _publish_parameters.at( 1 ).toFloat();
		float h = _publish_parameters.at( 2 ).toFloat();
		float res = _publish_parameters.at( 3 ).toFloat();
		float ox = _publish_parameters.at( 4 ).toFloat();
		float oy = _publish_parameters.at( 5 ).toFloat();

		if( false == set_map( data, w, h, res, ox, oy ) )
		{
			Q_EMIT log( "Error calling set_map", 0 );
			exit( 1 );
		}

		Q_EMIT log( QString( "published map %1 %2 %3 %4 %5 %6" ).arg( data.size() ).arg( w ).arg( h ).arg( res ).arg( ox ).arg( oy ), 0 );
	}
	else if ("mapPlannerEdited" == _topicName )
	{
		if (1 != _publish_parameters.size())
		{
			Q_EMIT log(QString("Expected: map pngFileName"), 0);
			exit(1);
		}

		QByteArray data;
		QString filename = _publish_parameters.at(0);
		QFile file(filename);
		if (file.open(QIODevice::ReadOnly))
		{
			data = file.readAll();
			file.close();
		}
		else
		{
			Q_EMIT log(QString("Error loading %1").arg(filename), 0);
			exit(1);
		}

		if (false == set_mapPlannerEdited(data, 0, 0, 0, 0, 0))
		{
			Q_EMIT log("Error calling set_mapPlannerEdited", 0);
			exit(1);
		}

		Q_EMIT log(QString("published mapPlannerEdited %1").arg(data.size()), 0);
	}
	else if( "sensors" == _topicName )
	{
		if( _publish_parameters.size() < 3 || _publish_parameters.size()%3 != 0 )
		{
			Q_EMIT log( "Expected: sensors name0 value0 unit0 [name1 value1 unit1 [...]]", 0 );
			exit( 1 );
		}

		QVector< QString > names;
		QVector< float > values;
		QVector< QString > units;
		for( int i=0; i<_publish_parameters.size(); i += 3 )
		{
			names << _publish_parameters.at( i );
			values << _publish_parameters.at( i+1 ).toFloat();
			units << _publish_parameters.at( i+2 );
		}

		if( false == set_sensors( names, values, units ) )
		{
			Q_EMIT log( "Error calling set_sensors", 0 );
			exit( 1 );
		}

		QString str = QString( "published sensors\n" );
		for( int i=0; i<names.size(); ++i )
		{
			str += QString("%1 %2%3\n").arg( names[i] ).arg( values[i] ).arg( units[i] );
		}

		Q_EMIT log( str, 0 );
	}
	else if( "poseOnMap" == _topicName )
	{
		if( 3 != _publish_parameters.size() )
		{
			Q_EMIT log( QString( "Expected: poseOnMap x y r" ), 0 );
			exit( 1 );
		}
		float x = _publish_parameters.at( 0 ).toFloat();
		float y = _publish_parameters.at( 1 ).toFloat();
		float r = _publish_parameters.at( 2 ).toFloat();
		
		QPointF pos;
		pos.setX(x);
		pos.setY(y);
		if( false == set_poseOnMap( pos, (double)r ) )
		{
			Q_EMIT log( "Error calling set_poseOnMap", 0 );
			exit( 1 );
		}
	}
	else if( "scan0" == _topicName )
	{
		QVector< float > ranges( 512 );
		QVector< float > intensities( 512 );

		for( int i=0; i<ranges.size(); ++i )
		{
			ranges[i] = 0.01 * i + 1;
			intensities[i] = ( (i>300 && i<400) ? 1 : 0 );
		}

		QVariantMap parameters;

		QString key;
		QString value;
		for( int i=0; i<_publish_parameters.size(); ++i )
		{
			if( key.isEmpty() )
			{
				key = _publish_parameters.at(i);
			}
			else
			{
				parameters[key] =  _publish_parameters.at(i);
				key.clear();
			}
		}

		if( false == set_scan0( ranges,	intensities, parameters	) )
		{
			Q_EMIT log( "Error calling set_scan0", 0 );
			exit( 1 );
		}

		QString str = QString( "published scan0\nnum ranges %1" ).arg( ranges.size() );
		Q_FOREACH( const QString& key, parameters.keys() )
		{
			QString value = parameters[key].toString();
			str += QString("\n%1 : %2").arg( key ).arg( value );
		}

		Q_EMIT log( str, 0 );
	}
	else if ("fleetcom_request" == _topicName)
	{
		if (1 != _publish_parameters.size())
		{
			Q_EMIT log(QString("Expected: fleetcom_request message"), 0);
			exit(1);
		}

		QString message = _publish_parameters.at(0);
		if (false == set_fleetcom_request(message))
		{
			Q_EMIT log("Error calling set_fleetcom_request", 0);
			exit(1);
		}

		Q_EMIT log(QString("published set_fleetcom_request %1").arg(message), 0);
	}
	JSON_TOPIC_PUBLISH(smartlicense)
	JSON_TOPIC_PUBLISH(smartdockingvis)
	JSON_TOPIC_PUBLISH(mclayout)
	JSON_TOPIC_PUBLISH(mcstatus)
	JSON_TOPIC_PUBLISH(pathnetwork)
	JSON_TOPIC_PUBLISH(pathnetwork_edited)
	JSON_TOPIC_PUBLISH(localizationMode)
	JSON_TOPIC_PUBLISH(smartlog)
	JSON_TOPIC_PUBLISH(smartnavigationplan)
	JSON_TOPIC_PUBLISH(smartlocations)
	JSON_TOPIC_PUBLISH(smartlocations_edited)
	JSON_TOPIC_PUBLISH(smartstations)
	JSON_TOPIC_PUBLISH(smartstations_edited)
	JSON_TOPIC_PUBLISH(smartrobotinfo)
	JSON_TOPIC_PUBLISH(smartmyrobotid)
	JSON_TOPIC_PUBLISH(smartjoblist)
	JSON_TOPIC_PUBLISH(mapDir)
	JSON_TOPIC_PUBLISH(festool_charger_info)
	JSON_TOPIC_PUBLISH(fleetcom_response)
	JSON_WITH_DATA_TOPIC_PUBLISH(mapDir_request)
	JSON_WITH_DATA_TOPIC_PUBLISH(mapDir_response)
	else if ( _topicName.startsWith("json_message_with_data_topic_"))
	{
		publish_json_message_with_data(_topicName);
	}
	else
	{
		bool found = false;
		for(int i=0; i<rec::robotino::rpc::json_message_with_data_topics_max; ++i)
		{
			const QString topicAlias = json_message_with_data_topics_alias(i);
			if(topicAlias == _topicName)
			{
				found = true;
				QString tpn = QString("json_message_with_data_topic_%1").arg(i);
				publish_json_message_with_data(tpn);
			}
		}
		
		if(false == found)
		{
			Q_EMIT log( QString( "Unkown topic %1" ).arg( _topicName ), 0 );
			exit( 1 );
		}
	}

	if( _publish_timer->isSingleShot() )
	{
		QTime t;
		t.start();
		while( t.elapsed() < 100 )
		{
			qApp->processEvents();
		}

		qApp->quit();
	}
}

void Client::publish_json_message_with_data(QString topicName)
{
	if (_publish_parameters.size() > 2 || _publish_parameters.size() < 1)
	{
		Q_EMIT log(QString("Expected: %1 jsonFile [dataFile]").arg(_topicName), 0);
		exit(1);
	}
	
	QString topicNumberStr = topicName.remove("json_message_with_data_topic_");
	int topicNum = topicNumberStr.toInt();
	if(topicNum < 0 || topicNum >= rec::robotino::rpc::json_message_with_data_topics_max)
	{
		Q_EMIT log(QString("Topic number %1 out of range").arg(topicNum), 0);
		exit(1);
	}
	
	QByteArray jsonData;
	QString jsonFileName = _publish_parameters.at(0);
	QFile jsonFile(jsonFileName);
	if (jsonFile.open(QIODevice::ReadOnly))
	{
		jsonData = jsonFile.readAll();
		jsonFile.close();
	}
	else
	{
		Q_EMIT log(QString("Error loading %1").arg(jsonFileName), 0);
		exit(1);
	}
	QByteArray data;
	if(_publish_parameters.size() > 1)
	{
		QString fileName = _publish_parameters.at(1);
		QFile file(fileName);
		if (file.open(QIODevice::ReadOnly))
		{
			data = file.readAll();
			file.close();
		}
		else
		{
			Q_EMIT log(QString("Error loading %1").arg(fileName), 0);
			exit(1);
		}
	}
	
	if (false == set_json_message_with_data(topicNum,jsonData,data))
	{
		Q_EMIT log( QString("Error calling set_json_message_with_data %1").arg(topicNum), 0);
		exit(1);
	}
	
	Q_EMIT log(QString("published %1 %2\ndata size:%3")
		.arg(_topicName).arg(jsonData.constData()).arg(data.size()), 0);
}

void Client::on_connected()
{
	if( _publish_timer )
	{
		_publish_timer->start();
	}
}

void Client::on_disconnected()
{
	Q_EMIT log( QString( "Connection refused" ), 0 );
	qApp->exit(1);
}

void Client::checkEchoCount()
{
	if(_echoMaxCount > 0)
	{
		++_echoCount;
		if(_echoCount >= _echoMaxCount)
		{
			qApp->quit();
		}
	}
}

void Client::on_camera0_settings_changed( unsigned int width, unsigned int height, const QString& format )
{
	Q_EMIT log( QString("camera0_settings: width=%1 height=%2 format=%3").arg( width ).arg( height ).arg( format ), 0 );
	checkEchoCount();
}

void Client::on_camera0_settings_info_changed( const rec::rpc::ClientInfoSet& info )
{
	QString str;
	Q_FOREACH( const rec::rpc::ClientInfo& i, info )
	{
		str += i.toString() + "\n";
	}
	str.remove( str.length() - 1, 1 );

	Q_EMIT log( QString("camera0_settings_info:\n%1").arg( str ), 0 );
	checkEchoCount();
}

void Client::on_set_camera0_control_changed( const QString& name, int value )
{
	Q_EMIT log( QString( "set_camera_control\nname=%1\nvalue=%2" ).arg( name ).arg( value ), 0 );
	checkEchoCount();
}

void Client::on_set_camera0_control_info_changed( const rec::rpc::ClientInfoSet& info )
{
	QString str;
	Q_FOREACH( const rec::rpc::ClientInfo& i, info )
	{
		str += i.toString() + "\n";
	}
	str.remove( str.length() - 1, 1 );

	Q_EMIT log( QString("camera0_control_info:\n%1").arg( str ), 0 );
	checkEchoCount();
}

void Client::on_set_camera0_settings_changed( unsigned int width, unsigned int height, const QString& format )
{
	Q_EMIT log( QString("set_camera0_settings: width=%1 height=%2 format=%3").arg( width ).arg( height ).arg( format ), 0 );
	checkEchoCount();
}

void Client::on_set_camera0_settings_info_changed( const rec::rpc::ClientInfoSet& info )
{
	QString str;
	Q_FOREACH( const rec::rpc::ClientInfo& i, info )
	{
		str += i.toString() + "\n";
	}
	str.remove( str.length() - 1, 1 );

	Q_EMIT log( QString("set_camera0_settings_info:\n%1").arg( str ), 0 );
	checkEchoCount();
}

void Client::on_camera0_capabilities_changed( const QString& cameraName, const QMap<QString, QVector<QSize> >& capabilities, const QStringList& controls )
{
	QString str = cameraName + "\n";
	QMap<QString, QVector<QSize> >::const_iterator iter = capabilities.constBegin();
	while( capabilities.constEnd() != iter )
	{
		str += iter.key() + " ";
		for( int i=0; i<iter.value().size(); ++i )
		{
			str += QString( "%1x%2 " ).arg( iter.value()[i].width() ).arg( iter.value()[i].height() );
		}
		str += "\n";
		++iter;
	}
	Q_FOREACH( const QString& c, controls )
	{
		str += c + "\n";
	}
	str.remove( str.length() - 1, 1 );

	Q_EMIT log( QString("camera0_capabilities:\n%1").arg( str ), 0 );
	checkEchoCount();
}

void Client::on_camera0_capabilities_info_changed( const rec::rpc::ClientInfoSet& info )
{
	QString str;
	Q_FOREACH( const rec::rpc::ClientInfo& i, info )
	{
		str += i.toString() + "\n";
	}
	str.remove( str.length() - 1, 1 );

	Q_EMIT log( QString("camera0_capabilities_info:\n%1").arg( str ), 0 );
	checkEchoCount();
}

void Client::on_camera0_calibration_changed( const QVector<double>& calibration )
{
	QString str;
	for( int i=0; i<calibration.size(); ++i )
	{
		str += QString::number( calibration[i] ) + " ";
	}
	str.remove( str.length() - 1, 1 );

	Q_EMIT log( QString("camera0_calibration:\n%1").arg( str ), 0 );
	checkEchoCount();
}

void Client::on_image0_changed( const QByteArray& data,
	unsigned int width,
	unsigned int height,
	unsigned int step,
	const QString& format )
{
	static int count = 0;

	QFile file( QString( "test%1.jpg" ).arg( count++ ) );
	file.open( QIODevice::WriteOnly );
	file.write( data );
	file.close();

	Q_EMIT log( QString("image0: width=%1 height=%2 format=%3 numbytes=%4").arg( width ).arg( height ).arg( format ).arg( data.size() ), 0 );
	checkEchoCount();
}

void Client::on_image0_info_changed( const rec::rpc::ClientInfoSet& info )
{
	QString str;
	Q_FOREACH( const rec::rpc::ClientInfo& i, info )
	{
		str += i.toString() + "\n";
	}
	str.remove( str.length() - 1, 1 );

	Q_EMIT log( QString("image0_info:\n%1").arg( str ), 0 );
	checkEchoCount();
}

void Client::on_motor0_setpoint_changed( float speed, const rec::rpc::ClientInfo& )
{
	Q_EMIT log( QString("motor0_setpoint %1[rpm]").arg( speed ), 0 );
	checkEchoCount();
}
void Client::on_motor0_setpoint_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_motor0_reset_position_changed( int position, const rec::rpc::ClientInfo& )
{
	Q_EMIT log( QString("motor0_reset_position %1").arg( position ), 0 );
	checkEchoCount();
}
void Client::on_motor0_reset_position_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_set_motor0_mode_changed( const QString& mode ) {}
void Client::on_set_motor0_mode_info_changed(  const rec::rpc::ClientInfoSet& ) {}

void Client::on_motor1_setpoint_changed( float speed, const rec::rpc::ClientInfo& )
{
	Q_EMIT log( QString("motor1_setpoint %1[rpm]").arg( speed ), 0 );
	checkEchoCount();
}
void Client::on_motor1_setpoint_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_motor1_reset_position_changed( int position, const rec::rpc::ClientInfo& )
{
	Q_EMIT log( QString("motor1_reset_position %1").arg( position ), 0 );
	checkEchoCount();
}
void Client::on_motor1_reset_position_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_set_motor1_mode_changed( const QString& mode ) {}
void Client::on_set_motor1_mode_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_motor2_setpoint_changed( float speed, const rec::rpc::ClientInfo& )
{
	Q_EMIT log( QString("motor2_setpoint %1[rpm]").arg( speed ), 0 );
	checkEchoCount();
}
void Client::on_motor2_setpoint_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_motor2_reset_position_changed( int position, const rec::rpc::ClientInfo& )
{
	Q_EMIT log( QString("motor2_reset_position %1").arg( position ), 0 );
	checkEchoCount();
}
void Client::on_motor2_reset_position_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_set_motor2_mode_changed( const QString& mode ) {}
void Client::on_set_motor2_mode_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_motor3_setpoint_changed( float speed, const rec::rpc::ClientInfo& )
{
	Q_EMIT log( QString("motor3_setpoint %1[rpm]").arg( speed ), 0 );
	checkEchoCount();
}
void Client::on_motor3_setpoint_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_motor3_reset_position_changed( int position, const rec::rpc::ClientInfo& )
{
	Q_EMIT log( QString("motor3_reset_position %1").arg( position ), 0 );
	checkEchoCount();
}
void Client::on_motor3_reset_position_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_set_motor3_mode_changed( const QString& mode ) {}
void Client::on_set_motor3_mode_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_motor_setpoints_changed( float m0, float m1, float m2, const rec::rpc::ClientInfo& )
{
	Q_EMIT log( QString("motor_setpoints\nm0=%1[rpm]\nm1=%2[rpm]\nm2=%3[rpm]").arg( m0 ).arg( m1 ).arg( m2 ), 0 );
	checkEchoCount();
}

void Client::on_motor_readings_changed( const QVector< float >& speeds, const QVector< int >& positions, const QVector< float >& currents, float time_delta )
{
	QString str = QString( "motor_readings dt=%1:\n" ).arg( time_delta );
	for( int i=0; i<speeds.size(); ++i )
	{
		str += QString( "\n M%1 vel:%2 pos:%3 current:%4" ).arg( i ).arg( speeds[i] ).arg( positions[i] ).arg( currents[i] );
	}
	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_motor_readings_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_omnidrive_changed( float vx, float vy, float omega, const rec::rpc::ClientInfo& )
{
	Q_EMIT log( QString( "omnidrive\nvx=%1\nvy=%2\nomega=%3" ).arg( vx ).arg( vy ).arg( omega ), 0 );
	checkEchoCount();
}

void Client::on_omnidrive_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_odometry_changed( double x, double y, double phi, float vx, float vy, float omega, unsigned int sequence )
{
	Q_EMIT log( QString( "odometry\nx[m]=%1\ny[m]=%2\nphi[deg]=%3\nvx=%4[m/s]\nvy=%5[m/s]\nomega=%6[deg/s]\nseq=%7" )
		.arg( x )
		.arg( y )
		.arg( 180.0 * phi / M_PI )
		.arg( vx )
		.arg( vy )
		.arg( 180.0 * omega / M_PI )
		.arg( sequence )
		, 0 );
	checkEchoCount();
}

void Client::on_odometry_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_set_odometry_changed( double x, double y, double phi )
{
	Q_EMIT log( QString("set_odometry\nx[m]=%1\ny[m]=%2\nphi[deg]=%3")
		.arg( x )
		.arg( y )
		.arg( 180.0 * phi / M_PI )
		, 0 );
	checkEchoCount();
}

void Client::on_pose_changed(double x, double y, double phi, double errx, double erry, double errphi, unsigned int sequence)
{
	Q_EMIT log(QString("pose\nx[m]=%1\ny[m]=%2\nphi[deg]=%3\nerrx=%4\nerry=%5\nerrphi=%6\nsequence=%7")
		.arg(x)
		.arg(y)
		.arg(180.0 * phi / M_PI)
		.arg(errx)
		.arg(erry)
		.arg(errphi)
		.arg(sequence)
		, 0);
	checkEchoCount();
}

void Client::on_northstar_changed( unsigned int sequenceNumber,
	unsigned int roomId,
	unsigned int numSpotsVisible,
	float posX,
	float posY,
	float posTheta,
	unsigned int magSpot0,
	unsigned int magSpot1 )
{
	Q_EMIT log( QString( "northstar\nseq=%1\nroom=%2\nnumSpots=%3\nx=%4\ny=%5\ntheta=%6\nmagSpot0=%7\nmagSpot1=%8" )
		.arg( sequenceNumber )
		.arg( roomId )
		.arg( numSpotsVisible )
		.arg( posX )
		.arg( posY )
		.arg( posTheta )
		.arg( magSpot0 )
		.arg( magSpot1 )
		, 0 );
	checkEchoCount();
}

void Client::on_northstar_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_set_northstar_parameters_changed( unsigned int roomId, float ceilingCal ) {}
void Client::on_set_northstar_parameters_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_gyroscope_changed( double phi, double rate, const rec::rpc::ClientInfo& client )
{
	Q_EMIT log( QString( "gyroscope\nphi=%1deg\nrate=%2deg/s" ).arg( 57.3*phi ).arg( 57.3*rate ), 0 );
	checkEchoCount();
}

void Client::on_gyroscope_info_changed( const rec::rpc::ClientInfoSet& )
{
}

void Client::on_gyroscope_ext_changed(double phi, double rate, const rec::rpc::ClientInfo& client)
{
	Q_EMIT log(QString("gyroscope_ext\nphi=%1deg\nrate=%2deg/s").arg(57.3*phi).arg(57.3*rate), 0);
	checkEchoCount();
}

void Client::on_gyroscope_ext_info_changed(const rec::rpc::ClientInfoSet&)
{
}

void Client::on_emergency_bumper_changed( bool enable )
{
	Q_EMIT log( QString( "emergency_bumper: %1" ).arg( enable ? "on" : "off" ), 0 );
	checkEchoCount();
}

void Client::on_emergency_bumper_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_set_emergency_bumper_changed( bool enable )
{
	Q_EMIT log( QString( "set_emergency_bumper: %1" ).arg( enable ? "on" : "off" ), 0 );
	checkEchoCount();
}

void Client::on_set_emergency_bumper_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_display_text_changed( const QString& text, unsigned int row, unsigned int col, bool clear_before, bool clear_after )
{
	Q_EMIT log( QString( "display_text:\n%1\nr=%2 c=%3 cb=%4 ca=%5" ).arg( text ).arg( row ).arg( col ).arg( clear_before ? "1" : "0" ).arg( clear_after ? "1" : "0" ), 0 );
	checkEchoCount();
}

void Client::on_display_text_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_display_backlight_changed( bool on ) {}
void Client::on_display_backlight_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_display_buttons_changed( bool up, bool down, bool back, bool enter ) {}
void Client::on_display_buttons_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_display_vbar_changed( float value, unsigned int col, unsigned int start_row, unsigned int end_row ) {}
void Client::on_display_vbar_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_display_hbar_changed( float value, unsigned int row, unsigned int start_col, unsigned int end_col ) {}
void Client::on_display_hbar_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_display_progress_changed( unsigned int step, unsigned int row ) {}
void Client::on_display_progress_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_display_clear_changed() {}
void Client::on_display_clear_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_parameters_changed( const QMap< QString, QVariant >& values ) {}
void Client::on_parameters_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_current_controller_changed( const rec::rpc::ClientInfo& ) {}
void Client::on_current_controller_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_ea_version_changed( const QString& board, quint16 firmware_major, quint16 firmware_minor, quint16 firmware_patch )
{
	Q_EMIT log( QString("ea_version %1 %2.%3.%4").arg( board ).arg( firmware_major ).arg( firmware_minor ).arg( firmware_patch ), 0 );
	checkEchoCount();
}

void Client::on_ea_version_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_ea09_ip4address_changed( const QHostAddress& address, const QHostAddress& netmask )
{
	Q_EMIT log( QString( "ea09_ip4address: %1 %2" ).arg( address.toString() ).arg( netmask.toString() ), 0 );
	checkEchoCount();
}

void Client::on_ea09_ip4address_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_set_ea09_ip4address_changed( const QHostAddress& address, const QHostAddress& netmask )
{
	Q_EMIT log( QString( "set_ea09_ip4address: %1 %2" ).arg( address.toString() ).arg( netmask.toString() ), 0 );
	checkEchoCount();
}

void Client::on_set_ea09_ip4address_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_clients_connected_changed( const QVector< rec::rpc::ClientInfo >& clients )
{
	QString str = "clients_connected:";
	Q_FOREACH( const rec::rpc::ClientInfo& info, clients )
	{
		str += "\n";
		str += info.toString();
	}
	str += "\n";

	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_clients_connected_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_digital_input_changed( const QVector< bool >& data )
{
	QString str = "digital_input:\n";
	int index = 1;
	Q_FOREACH( bool b, data )
	{
		str += QString( "DIN%1: %2\n" ).arg( index++ ).arg( b ? "1" : "0" );
	}

	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_digital_input_info_changed( const rec::rpc::ClientInfoSet& )
{
}

void Client::on_analog_input_changed( const QVector< float >& data )
{
	QString str = "analog_input:\n";
	int index = 1;
	Q_FOREACH( float f, data )
	{
		str += QString( "AIN%1: %2\n" ).arg( index++ ).arg( f );
	}

	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_analog_input_info_changed( const rec::rpc::ClientInfoSet& )
{
}

void Client::on_distance_sensors_changed( const QVector< float >& data )
{
	QString str = "distance_sensors:\n";
	int index = 1;
	Q_FOREACH( float f, data )
	{
		str += QString( "IR%1: %2\n" ).arg( index++ ).arg( f );
	}

	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_distance_sensors_info_changed( const rec::rpc::ClientInfoSet& )
{
}

void Client::on_bumper_changed( bool b )
{
	Q_EMIT log( QString( "bumper: %1" ).arg( b ? "hit" : "not hit" ), 0 );
	checkEchoCount();
}

void Client::on_bumper_info_changed( const rec::rpc::ClientInfoSet& )
{
}

void Client::on_battery_changed( float battery_voltage, float system_current, bool ext_power, int num_chargers, const QString& batteryType, bool batteryLow, int batteryLowShutdownCounter )
{
	Q_EMIT log( QString( "Voltage: %1"
							"\nCurrent: %2"
							"\nExternal power: %3"
							"\nNum chargers: %4"
							"\nBattery type: %5"
							"\nBattery low: %6"
							"\nBattery shutdown counter: %7" )
							.arg( battery_voltage )
							.arg( system_current )
							.arg( ext_power ? "Yes" : "No" )
							.arg( num_chargers )
							.arg( batteryType )
							.arg( batteryLow ? "Yes" : "No" )
							.arg( batteryLowShutdownCounter ), 0 );
	checkEchoCount();
}

void Client::on_battery_info_changed( const rec::rpc::ClientInfoSet& )
{
}

void Client::on_set_digital_output_changed( unsigned int index, bool state )
{
	Q_EMIT log( QString( "dout%1: %2" ).arg( index ).arg( state ? 1 : 0 ), 0 );
	checkEchoCount();
}

void Client::on_set_digital_output_array_changed(const QVector<bool>& data )
{
	QString str( "digital output array:\n" );
	for( int i=0; i<data.size(); ++i )
	{
		str += QString( "%1\n" ).arg( data[i] ? 1 : 0 );
	}

	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_set_digital_output_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_set_relay_changed( unsigned int index, bool state )
{
	Q_EMIT log( QString( "relay%1: %2" ).arg( index ).arg( state ? 1 : 0 ), 0 );
	checkEchoCount();
}

void Client::on_set_relay_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_set_relay_array_changed(const QVector<bool>& data )
{
	QString str( "relay array:\n" );
	for( int i=0; i<data.size(); ++i )
	{
		str += QString( "%1\n" ).arg( data[i] ? 1 : 0 );
	}

	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_set_shutdown_changed( bool shutdown )
{
	Q_EMIT log( QString( "set_shutdown %1" ).arg( shutdown ? "true" : "false" ), 0 );
}

void Client::on_set_shutdown_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_power_button_changed( bool pressed )
{
	Q_EMIT log( QString( "power_button %1" ).arg( pressed ? "true" : "false" ), 0 );
	checkEchoCount();
}

void Client::on_power_button_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_request_shutdown_changed( bool doit )
{
	Q_EMIT log( QString( "request_shutdown %1" ).arg( doit ? "true" : "false" ), 0 );
	checkEchoCount();
}

void Client::on_request_shutdown_info_changed( const rec::rpc::ClientInfoSet& ) {}

void Client::on_scan0_changed( const QVector< float >& ranges,	const QVector< float >& intensities, const QVariantMap& parameters )
{
	QString str = QString( "scan0\nnum ranges %1\nnum intensities %2" ).arg( ranges.size() ).arg( intensities.size() );
	Q_FOREACH( const QString& key, parameters.keys() )
	{
		QString value = parameters[key].toString();
		str += QString("\n%1 : %2").arg( key ).arg( value );
	}

	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_scan0_info_changed( const rec::rpc::ClientInfoSet& info )
{
	QString str;
	Q_FOREACH( const rec::rpc::ClientInfo& i, info )
	{
		str += i.toString() + "\n";
	}
	str.remove( str.length() - 1, 1 );

	Q_EMIT log( QString("scan0_info:\n%1").arg( str ), 0 );
	checkEchoCount();
}

void Client::on_scan1_changed( const QVector< float >& ranges,	const QVector< float >& intensities, const QVariantMap& parameters )
{
	QString str = QString( "scan1\nnum ranges %1\nnum intensities %2" ).arg( ranges.size() ).arg( intensities.size() );
	Q_FOREACH( const QString& key, parameters.keys() )
	{
		QString value = parameters[key].toString();
		str += QString("\n%1 : %2").arg( key ).arg( value );
	}

	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_scan1_info_changed( const rec::rpc::ClientInfoSet& info )
{
	QString str;
	Q_FOREACH( const rec::rpc::ClientInfo& i, info )
	{
		str += i.toString() + "\n";
	}
	str.remove( str.length() - 1, 1 );

	Q_EMIT log( QString("scan1_info:\n%1").arg( str ), 0 );
	checkEchoCount();
}

void Client::on_scan2_changed( const QVector< float >& ranges,	const QVector< float >& intensities, const QVariantMap& parameters )
{
	QString str = QString( "scan0\nnum ranges %1\nnum intensities %2" ).arg( ranges.size() ).arg( intensities.size() );
	Q_FOREACH( const QString& key, parameters.keys() )
	{
		QString value = parameters[key].toString();
		str += QString("\n%1 : %2").arg( key ).arg( value );
	}

	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_scan2_info_changed( const rec::rpc::ClientInfoSet& info )
{
	QString str;
	Q_FOREACH( const rec::rpc::ClientInfo& i, info )
	{
		str += i.toString() + "\n";
	}
	str.remove( str.length() - 1, 1 );

	Q_EMIT log( QString("scan2_info:\n%1").arg( str ), 0 );
	checkEchoCount();
}

void Client::on_scan3_changed( const QVector< float >& ranges,	const QVector< float >& intensities, const QVariantMap& parameters )
{
	QString str = QString( "scan0\nnum ranges %1\nnum intensities %2" ).arg( ranges.size() ).arg( intensities.size() );
	Q_FOREACH( const QString& key, parameters.keys() )
	{
		QString value = parameters[key].toString();
		str += QString("\n%1 : %2").arg( key ).arg( value );
	}

	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_scan3_info_changed( const rec::rpc::ClientInfoSet& info )
{
	QString str;
	Q_FOREACH( const rec::rpc::ClientInfo& i, info )
	{
		str += i.toString() + "\n";
	}
	str.remove( str.length() - 1, 1 );

	Q_EMIT log( QString("scan3_info:\n%1").arg( str ), 0 );
	checkEchoCount();
}

void Client::on_cbha_readings_changed( const QVector<float>& pressures, bool pressureSensor, const QVector<float>& stringPots, float foilPot )
{
	QString str = "cbha_readings";
	for( int i=0; i<pressures.size(); ++i )
	{
		str += QString("\nbellows%1=%2bar").arg( i ).arg( pressures[i] );
	}
	for( int i=0; i<stringPots.size(); ++i )
	{
		str += QString("\nstringPots%1=%2").arg( i ).arg( stringPots[i] );
	}

	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_cbha_set_pressure_changed( const QVector<float>& pressures )
{
	QString str = "cbha_set_pressure";
	for( int i=0; i<pressures.size(); ++i )
	{
		str += QString("\nbellows%1=%2bar").arg( i ).arg( pressures[i] );
	}
	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_grappler_store_positions_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data )
{
	QString str = "grappler_store_positions";
	for( int i=0; i<data.size(); ++i )
	{
		const rec::robotino::rpc::GrapplerServoInfo& info = data[i];
		str += QString("\n*** channel=%1 id=%2 found=%3 ***").arg( info.channel ).arg( info.id ).arg( info.found ? "true" : "false" );
		str += QString("\nangle=%1 speed=%2 error=%3").arg( info.angle ).arg( info.speed ).arg( info.error );
		str += QString("\ncwAxisLimit=%1 ccwAxisLimit=%2").arg( info.cwAxisLimit ).arg( info.ccwAxisLimit );
	}
	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_grappler_set_power_changed( int line, bool power )
{
	Q_EMIT log( QString( "grappler_set_power: line=%1 power=%2" ).arg( line ).arg( power ? "true" : "false" ), 0 );
	checkEchoCount();
}

void Client::on_grappler_set_positions_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data )
{
	QString str = "grappler_set_positions";
	for( int i=0; i<data.size(); ++i )
	{
		const rec::robotino::rpc::GrapplerServoInfo& info = data[i];
		str += QString("\n*** channel=%1 id=%2 found=%3 ***").arg( info.channel ).arg( info.id ).arg( info.found ? "true" : "false" );
		str += QString("\nangle=%1 speed=%2 error=%3").arg( info.angle ).arg( info.speed ).arg( info.error );
		str += QString("\ncwAxisLimit=%1 ccwAxisLimit=%2").arg( info.cwAxisLimit ).arg( info.ccwAxisLimit );
	}
	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_grappler_servos_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data )
{
	QString str = "grappler_servos";
	for( int i=0; i<data.size(); ++i )
	{
		const rec::robotino::rpc::GrapplerServoInfo& info = data[i];
		str += QString("\n*** channel=%1 id=%2 found=%3 ***").arg( info.channel ).arg( info.id ).arg( info.found ? "true" : "false" );
		str += QString("\nangle=%1 speed=%2 error=%3").arg( info.angle ).arg( info.speed ).arg( info.error );
		str += QString("\ncwAxisLimit=%1 ccwAxisLimit=%2").arg( info.cwAxisLimit ).arg( info.ccwAxisLimit );
	}
	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_grappler_readings_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data )
{
	QString str = "grappler_readings";
	for( int i=0; i<data.size(); ++i )
	{
		const rec::robotino::rpc::GrapplerServoInfo& info = data[i];
		str += QString("\n*** channel=%1 id=%2 found=%3 ***").arg( info.channel ).arg( info.id ).arg( info.found ? "true" : "false" );
		str += QString("\nangle=%1 speed=%2 error=%3").arg( info.angle ).arg( info.speed ).arg( info.error );
		str += QString("\ncwAxisLimit=%1 ccwAxisLimit=%2").arg( info.cwAxisLimit ).arg( info.ccwAxisLimit );
	}
	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_grappler_toggle_torque_changed()
{
	Q_EMIT log( "grappler_toggle_torque", 0 );
	checkEchoCount();
}

void Client::on_log_changed( const QString& publisher, const QString& message, int level )
{
	Q_EMIT log( QString( "%1: %2" ).arg( publisher ).arg( message ), 0 );
	checkEchoCount();
}

void Client::on_set_log_level_changed( const QString& publisher, int level )
{
	Q_EMIT log( QString( "set_log_level %1 %2" ).arg( publisher ).arg( level ), 0 );
	checkEchoCount();
}

void Client::on_custom_message_changed( unsigned int id, const QByteArray& data )
{
	Q_EMIT log( QString("custom_message\nid=%1\ndata=%2").arg( id ).arg( QString( data ) ), 0 );
	checkEchoCount();
}

void Client::on_custom_message0_changed( const QByteArray& data )
{
	Q_EMIT log( QString("custom_message0\ndata=%1").arg( QString( data ) ), 0 );
	checkEchoCount();
}

void Client::on_custom_message1_changed( const QByteArray& data )
{
	Q_EMIT log( QString("custom_message1\ndata=%1").arg( QString( data ) ), 0 );
	checkEchoCount();
}

void Client::on_custom_message2_changed( const QByteArray& data )
{
	Q_EMIT log( QString("custom_message2\ndata=%1").arg( QString( data ) ), 0 );
	checkEchoCount();
}

void Client::on_custom_message3_changed( const QByteArray& data )
{
	Q_EMIT log( QString("custom_message3\ndata=%1").arg( QString( data ) ), 0 );
	checkEchoCount();
}

void Client::on_kinect0_depth_changed( const QByteArray& data, const QByteArray& object_data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	Q_EMIT log( QString( "kinect0_depth\ndataSize=%1\nobject_data_size=%2\nwidth=%3\nheight=%4\nformat=%5\nstamp=%6" ).arg( data.size() ).arg( object_data.size() ).arg( width ).arg( height ).arg( format ).arg( stamp ), 0 );
	checkEchoCount();
}

void Client::on_kinect1_depth_changed( const QByteArray& data, const QByteArray& object_data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	Q_EMIT log( QString( "kinect1_depth\ndataSize=%1\nobject_data_size=%2\nwidth=%3\nheight=%4\nformat=%5\nstamp=%6" ).arg( data.size() ).arg( object_data.size() ).arg( width ).arg( height ).arg( format ).arg( stamp ), 0 );
	checkEchoCount();
}

void Client::on_kinect2_depth_changed( const QByteArray& data, const QByteArray& object_data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	Q_EMIT log( QString( "kinect2_depth\ndataSize=%1\nobject_data_size=%2\nwidth=%3\nheight=%4\nformat=%5\nstamp=%6" ).arg( data.size() ).arg( object_data.size() ).arg( width ).arg( height ).arg( format ).arg( stamp ), 0 );
	checkEchoCount();
}

void Client::on_kinect3_depth_changed( const QByteArray& data, const QByteArray& object_data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	Q_EMIT log( QString( "kinect3_depth\ndataSize=%1\nobject_data_size=%2\nwidth=%3\nheight=%4\nformat=%5\nstamp=%6" ).arg( data.size() ).arg( object_data.size() ).arg( width ).arg( height ).arg( format ).arg( stamp ), 0 );
	checkEchoCount();
}

void Client::on_kinect0_video_changed( const QByteArray& data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	Q_EMIT log( QString( "kinect0_video\ndataSize=%1\nwidth=%2\nheight=%3\nformat=%4\nstamp=%5" ).arg( data.size() ).arg( width ).arg( height ).arg( format ).arg( stamp ), 0 );
	checkEchoCount();
}

void Client::on_kinect1_video_changed( const QByteArray& data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	Q_EMIT log( QString( "kinect1_video\ndataSize=%1\nwidth=%2\nheight=%3\nformat=%4\nstamp=%5" ).arg( data.size() ).arg( width ).arg( height ).arg( format ).arg( stamp ), 0 );
	checkEchoCount();
}

void Client::on_kinect2_video_changed( const QByteArray& data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	Q_EMIT log( QString( "kinect2_video\ndataSize=%1\nwidth=%2\nheight=%3\nformat=%4\nstamp=%5" ).arg( data.size() ).arg( width ).arg( height ).arg( format ).arg( stamp ), 0 );
	checkEchoCount();
}

void Client::on_kinect3_video_changed( const QByteArray& data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	Q_EMIT log( QString( "kinect3_video\ndataSize=%1\nwidth=%2\nheight=%3\nformat=%4\nstamp=%5" ).arg( data.size() ).arg( width ).arg( height ).arg( format ).arg( stamp ), 0 );
	checkEchoCount();
}

void Client::on_kinect0_tilt_changed( double tilt )
{
	Q_EMIT log( QString("kinect0_tilt %1").arg( tilt ), 0 );
	checkEchoCount();
}

void Client::on_kinect1_tilt_changed( double tilt )
{
	Q_EMIT log( QString("kinect1_tilt %1").arg( tilt ), 0 );
	checkEchoCount();
}

void Client::on_kinect2_tilt_changed( double tilt )
{
	Q_EMIT log( QString("kinect2_tilt %1").arg( tilt ), 0 );
	checkEchoCount();
}

void Client::on_kinect3_tilt_changed( double tilt )
{
	Q_EMIT log( QString("kinect3_tilt %1").arg( tilt ), 0 );
	checkEchoCount();
}

void Client::on_kinect0_accel_changed( double x, double y, double z )
{
	Q_EMIT log( QString("kinect0_accel %1 %2 %3").arg( x ).arg( y ).arg( z ), 0 );
	checkEchoCount();
}

void Client::on_kinect1_accel_changed( double x, double y, double z )
{
	Q_EMIT log( QString("kinect1_accel %1 %2 %3").arg( x ).arg( y ).arg( z ), 0 );
	checkEchoCount();
}

void Client::on_kinect2_accel_changed( double x, double y, double z )
{
	Q_EMIT log( QString("kinect2_accel %1 %2 %3").arg( x ).arg( y ).arg( z ), 0 );
	checkEchoCount();
}

void Client::on_kinect3_accel_changed( double x, double y, double z )
{
	Q_EMIT log( QString("kinect3_accel %1 %2 %3").arg( x ).arg( y ).arg( z ), 0 );
	checkEchoCount();
}

void Client::on_process_status_changed( const rec::robotino::rpc::ProcessStatus& status )
{
	Q_EMIT log( QString( "process_status\nid: %1\nstate: %2\nerror_code: %3\nexit_code: %4\nexit_status: %5" )
		.arg( status.id ).arg( status.state ).arg( status.error_code ).arg( status.exit_code ).arg( status.exit_status ), 0 );
	checkEchoCount();
}

void Client::on_process_output_changed( const rec::robotino::rpc::ProcessOutput& output )
{
	Q_EMIT log( QString( "process_output\nid: %1\nchannel: %2\ndata: %3" )
		.arg( output.id ).arg( output.channel ).arg( QString( output.data ) ), 0 );
	checkEchoCount();
}

void Client::on_disconnect_api1_clients_changed()
{
	Q_EMIT log( "disconnect_api1_clients", 0 );
	checkEchoCount();
}

void Client::on_api1_setstate_received_changed()
{
	Q_EMIT log( "api1_setstate_received", 0 );
	checkEchoCount();
}

void Client::on_charger0_info_changed( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state )
{
	QString str = QString( "charger0_info\n"
							"  time: %1s\n"
							"  battery voltage: %2V\n"
							"  charging current: %3A\n"
							"  bat1 temp: %4�C\n"
							"  bat2 temp: %5�C\n"
							"  state #%6: %7" )
							.arg( time )
							.arg( batteryVoltage )
							.arg( chargingCurrent )
							.arg( bat1temp )
							.arg( bat2temp )
							.arg( state_number )
							.arg( state );
	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_charger0_version_changed( int major, int minor, int patch )
{
	QString str = QString( "charger0_version %1.%2.%3" ).arg( major ).arg( minor ).arg( patch );
	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_charger0_error_changed( unsigned int time, const QString& message )
{
	QString str = QString( "charger0_error %1 %2" ).arg( time ).arg( message );
	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_charger0_clear_error_changed()
{
	Q_EMIT log( "charger0_clear_error", 0 );
	checkEchoCount();
}

void Client::on_charger1_info_changed( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state )
{
	QString str = QString( "charger1_info\n"
							"  time: %1s\n"
							"  battery voltage: %2V\n"
							"  charging current: %3A\n"
							"  bat1 temp: %4�C\n"
							"  bat2 temp: %5�C\n"
							"  state #%6: %7" )
							.arg( time )
							.arg( batteryVoltage )
							.arg( chargingCurrent )
							.arg( bat1temp )
							.arg( bat2temp )
							.arg( state_number )
							.arg( state );
	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_charger1_version_changed( int major, int minor, int patch )
{
	QString str = QString( "charger1_version %1.%2.%3" ).arg( major ).arg( minor ).arg( patch );
	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_charger1_error_changed( unsigned int time, const QString& message )
{
	QString str = QString( "charger1_error %1 %2" ).arg( time ).arg( message );
	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_charger1_clear_error_changed()
{
	Q_EMIT log( "charger1_clear_error", 0 );
	checkEchoCount();
}

void Client::on_charger2_info_changed( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state )
{
	QString str = QString( "charger2_info\n"
							"  time: %1s\n"
							"  battery voltage: %2V\n"
							"  charging current: %3A\n"
							"  bat1 temp: %4�C\n"
							"  bat2 temp: %5�C\n"
							"  state #%6: %7" )
							.arg( time )
							.arg( batteryVoltage )
							.arg( chargingCurrent )
							.arg( bat1temp )
							.arg( bat2temp )
							.arg( state_number )
							.arg( state );
	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_charger2_version_changed( int major, int minor, int patch )
{
	QString str = QString( "charger2_version %1.%2.%3" ).arg( major ).arg( minor ).arg( patch );
	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_charger2_error_changed( unsigned int time, const QString& message )
{
	QString str = QString( "charger2_error %1 %2" ).arg( time ).arg( message );
	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_charger2_clear_error_changed()
{
	Q_EMIT log( "charger2_clear_error", 0 );
	checkEchoCount();
}

void Client::on_gripper_state_changed( int state )
{
	/*
	Gripper state:
	0 - Moving
	1 - Open
	2 - Closed
	*/

	QString stateStr;
	switch( state )
	{
	case 0:
		stateStr = "Moving";
		break;

	case 1:
		stateStr = "Open";
		break;

	case 2:
		stateStr = "Closed";
		break;

	default:
		stateStr = "Unknown";
		break;
	}

	Q_EMIT log( QString( "gripper_state %1" ).arg( stateStr ), 0 );
	checkEchoCount();
}

void Client::on_map_changed( const QByteArray& data,int w,int h, float res, float ox, float oy )
{
	/*
	ADD_MEMBER( width )
ADD_MEMBER( height )
ADD_MEMBER( resolution )
ADD_MEMBER( offsetx )
ADD_MEMBER( offsety )
	*/

	QString str = QString("map\nnumBytes %1\nwidth %2\nheight %3\nresolution %4\noffsetx %5\noffsety %6")
		.arg( data.size() )
		.arg( w )
		.arg( h )
		.arg( res )
		.arg( ox )
		.arg( oy );

	Q_EMIT log( str, 0 );
	
	if(logHandler->verbosity() > 0)
	{
		QFile file("map.png");
		if(file.open(QIODevice::WriteOnly))
		{
			file.write(data);
			Q_EMIT log("map.png written",0);
		}
		else
		{
			Q_EMIT log("Error writing map.png",0);
		}
	}
	
	
	checkEchoCount();
}

void Client::on_mapPlanner_changed(const QByteArray& data, int w, int h, float ox, float oy, float res)
{
	QString str = QString("mapPlanner\nnumBytes %1\nwidth %2\nheight %3\nresolution %4\noffsetx %5\noffsety %6")
		.arg(data.size())
		.arg(w)
		.arg(h)
		.arg(res)
		.arg(ox)
		.arg(oy);

	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_mapPlannerEdited_changed(const QByteArray& data, int w, int h, float ox, float oy, float res)
{
	QString str = QString("mapPlannerEdited\nnumBytes %1\nwidth %2\nheight %3\nresolution %4\noffsetx %5\noffsety %6")
		.arg(data.size())
		.arg(w)
		.arg(h)
		.arg(res)
		.arg(ox)
		.arg(oy);

	Q_EMIT log(str, 0);
	checkEchoCount();
}

JSON_TOPIC_FUNCTIONS(smartlicense)
JSON_TOPIC_FUNCTIONS(smartdockingvis)
JSON_TOPIC_FUNCTIONS(mclayout)
JSON_TOPIC_FUNCTIONS(mcstatus)
JSON_TOPIC_FUNCTIONS(pathnetwork)
JSON_TOPIC_FUNCTIONS(pathnetwork_edited)
JSON_TOPIC_FUNCTIONS(localizationMode)
JSON_TOPIC_FUNCTIONS(smartlog)
JSON_TOPIC_FUNCTIONS(smartnavigationplan)
JSON_TOPIC_FUNCTIONS(smartlocations)
JSON_TOPIC_FUNCTIONS(smartlocations_edited)
JSON_TOPIC_FUNCTIONS(smartstations)
JSON_TOPIC_FUNCTIONS(smartstations_edited)
JSON_TOPIC_FUNCTIONS(smartrobotinfo)
JSON_TOPIC_FUNCTIONS(smartmyrobotid)
JSON_TOPIC_FUNCTIONS(smartjoblist)
JSON_TOPIC_FUNCTIONS(mapDir)
JSON_TOPIC_FUNCTIONS(festool_charger_info)
JSON_TOPIC_FUNCTIONS(fleetcom_response)
JSON_WITH_DATA_TOPIC_FUNCTIONS(mapDir_request)
JSON_WITH_DATA_TOPIC_FUNCTIONS(mapDir_response)

void Client::on_sensors_changed( const QVector< QString >& names, const QVector< float >& values, const QVector< QString >& units )
{
	QString str = "sensors\n";
	for( int i=0; i<names.size() && i<values.size() && i<units.size(); ++i )
	{
		str += QString("%1 %2%3\n").arg( names[i] ).arg( values[i] ).arg( units[i] );
	}

	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_poseOnMap_changed(const QPointF& pos, double rot)
{
	QString str = QString("poseOnMap (x, y, r) = (%1, %2, %3)").arg(pos.x()).arg(pos.y()).arg(rot);
	Q_EMIT log( str, 0 );
	checkEchoCount();
}

void Client::on_fleetcom_request_changed( const QString& message )
{
	Q_EMIT log( "fleetcom_request: " + message, 0 );
	checkEchoCount();
}

void Client::on_set_pid_parameters_changed(unsigned int motor, float kp, float ki, float kd)
{
	Q_EMIT log(QString("set_pid_parameters for motor %1\nkp=%2\nki=%3\nkp=%4").arg(motor).arg(kp).arg(ki).arg(kd), 0);
	checkEchoCount();
}

void Client::on_json_message_with_data_changed(int topic, const QByteArray& jsonData, const QByteArray& data)
{
	QString topicAlias = json_message_with_data_topics_alias(topic);
	if(topicAlias.isEmpty())
	{
		topicAlias = QString("rec_robotino_rpc_json_message_with_data_topic_%1").arg(topic);
	}

	emit logWithInfo( QString("%1 data size:%2\n").arg(topicAlias).arg(data.size()), QString(jsonData), 0);
	
	if(logHandler->verbosity() > 0)
	{
		QString fileName = QString("%1/%2.data").arg(DATA_DIR).arg(topicAlias);
		QFile file(fileName);
		if (file.open(QIODevice::WriteOnly))
		{
			file.write(data);
			Q_EMIT log(QString("%1 written").arg(fileName), 0);
		}
		else
		{
			Q_EMIT log(QString("Error writing %1").arg(fileName), 0);
		}
	}

	checkEchoCount();
}

void Client::on_motor_debug_changed(int motor, const QVector<unsigned int>& times, const QVector<float>& speed_setpoints, const QVector<float>& speeds, const QVector<float>& currents, const QVector<float>& control_points)
{
	QString str = QString("motor_debug %1:").arg(motor);
	str += QString("\n Num Entries: %1 %2 %3 %4 %5").arg(times.size()).arg(speed_setpoints.size()).arg(speeds.size()).arg(currents.size()).arg(control_points.size());

	if (times.isEmpty() || times.size() != speed_setpoints.size() || times.size() != speeds.size() || times.size() != currents.size() || times.size() != control_points.size())
	{
		str += "\n Size mismatch";
	}
	else
	{
		for(int i=0;i<times.size(); ++i)
		{
			str += QString("\n t:%1 ss:%2 s:%3 c:%4 cp:%5").arg(times.at(i)).arg(speed_setpoints.at(i)).arg(speeds.at(i)).arg(currents.at(i)).arg(control_points.at(i));
		}
	}


	Q_EMIT log(str, 0);
	checkEchoCount();
}

void Client::on_start_motor_debug_changed(int motor)
{
	Q_EMIT log(QString("start_motor_debug %1").arg(motor), 0);
	checkEchoCount();
}

void Client::on_upload_program_changed(const QByteArray& jsonData, const QByteArray& data)
{
	QString str = QString("upload_program with size %1").arg(data.size());
	str += "\n" + jsonData;
	Q_EMIT log(str, 0);
	checkEchoCount();
}