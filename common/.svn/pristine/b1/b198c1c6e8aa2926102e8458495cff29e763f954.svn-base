#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "rec/robotino/rpc/Client.h"

class Client : public rec::robotino::rpc::Client
{
	Q_OBJECT
public:
	Client( int timeout );

	bool publish( const QStringList& parameters );
	
	void setEchoCount(int echoCount) { _echoMaxCount = echoCount; }

Q_SIGNALS:
  void logError(QString,int);
  void logWithInfo(QString,QString,int);

private Q_SLOTS:
	void on_timeout();

	void on_publish_timer_timeout();

	void on_connected();
	void on_disconnected();

	void on_camera0_settings_changed( unsigned int width, unsigned int height, const QString& format );
	void on_camera0_settings_info_changed( const rec::rpc::ClientInfoSet& );

	void on_set_camera0_control_changed( const QString& name, int value );
	void on_set_camera0_control_info_changed( const rec::rpc::ClientInfoSet& );

	void on_set_camera0_settings_changed( unsigned int width, unsigned int height, const QString& format );
	void on_set_camera0_settings_info_changed( const rec::rpc::ClientInfoSet& );

	void on_camera0_capabilities_changed( const QString& cameraName, const QMap<QString, QVector<QSize> >& capabilities, const QStringList& controls );
	void on_camera0_capabilities_info_changed( const rec::rpc::ClientInfoSet& );

	void on_camera0_calibration_changed( const QVector<double>& calibration );


	void on_image0_changed( const QByteArray& data,
		unsigned int width,
		unsigned int height,
		unsigned int step,
		const QString& format );
	void on_image0_info_changed( const rec::rpc::ClientInfoSet& );

	void on_motor0_setpoint_changed( float speed, const rec::rpc::ClientInfo& );
	void on_motor0_setpoint_info_changed( const rec::rpc::ClientInfoSet& );

	void on_motor0_reset_position_changed( int position, const rec::rpc::ClientInfo& );
	void on_motor0_reset_position_info_changed( const rec::rpc::ClientInfoSet& );

	void on_set_motor0_mode_changed( const QString& mode );
	void on_set_motor0_mode_info_changed(  const rec::rpc::ClientInfoSet& );

	void on_motor1_setpoint_changed( float speed, const rec::rpc::ClientInfo& );
	void on_motor1_setpoint_info_changed( const rec::rpc::ClientInfoSet& );

	void on_motor1_reset_position_changed( int position, const rec::rpc::ClientInfo& );
	void on_motor1_reset_position_info_changed( const rec::rpc::ClientInfoSet& );

	void on_set_motor1_mode_changed( const QString& mode );
	void on_set_motor1_mode_info_changed( const rec::rpc::ClientInfoSet& );

	void on_motor2_setpoint_changed( float speed, const rec::rpc::ClientInfo& );
	void on_motor2_setpoint_info_changed( const rec::rpc::ClientInfoSet& );

	void on_motor2_reset_position_changed( int position, const rec::rpc::ClientInfo& );
	void on_motor2_reset_position_info_changed( const rec::rpc::ClientInfoSet& );

	void on_set_motor2_mode_changed( const QString& mode );
	void on_set_motor2_mode_info_changed( const rec::rpc::ClientInfoSet& );

	void on_motor3_setpoint_changed( float speed, const rec::rpc::ClientInfo& );
	void on_motor3_setpoint_info_changed( const rec::rpc::ClientInfoSet& );

	void on_motor3_reset_position_changed( int position, const rec::rpc::ClientInfo& );
	void on_motor3_reset_position_info_changed( const rec::rpc::ClientInfoSet& );

	void on_set_motor3_mode_changed( const QString& mode );
	void on_set_motor3_mode_info_changed( const rec::rpc::ClientInfoSet& );

	void on_motor_setpoints_changed( float m0, float m1, float m2, const rec::rpc::ClientInfo& );

	void on_motor_readings_changed( const QVector< float >& speeds, const QVector< int >& positions, const QVector< float >& currents, float time_delta );
	void on_motor_readings_info_changed( const rec::rpc::ClientInfoSet& );

	void on_omnidrive_changed( float vx, float vy, float omega, const rec::rpc::ClientInfo& );
	void on_omnidrive_info_changed( const rec::rpc::ClientInfoSet& );

	void on_odometry_changed( double x, double y, double phi, float vx, float vy, float omega, unsigned int sequence );
	void on_odometry_info_changed( const rec::rpc::ClientInfoSet& );

	void on_set_odometry_changed( double x, double y, double phi );

	void on_pose_changed(double x, double y, double phi, double errx, double erry, double errphi, unsigned int sequence);

	void on_northstar_changed( unsigned int sequenceNumber,
		unsigned int roomId,
		unsigned int numSpotsVisible,
		float posX,
		float posY,
		float posTheta,
		unsigned int magSpot0,
		unsigned int magSpot1 );
	void on_northstar_info_changed( const rec::rpc::ClientInfoSet& );

	void on_set_northstar_parameters_changed( unsigned int roomId, float ceilingCal );
	void on_set_northstar_parameters_info_changed( const rec::rpc::ClientInfoSet& );

	void on_gyroscope_changed( double phi, double rate, const rec::rpc::ClientInfo& );
	void on_gyroscope_info_changed( const rec::rpc::ClientInfoSet& );

	void on_gyroscope_ext_changed(double phi, double rate, const rec::rpc::ClientInfo&);
	void on_gyroscope_ext_info_changed(const rec::rpc::ClientInfoSet&);

	void on_emergency_bumper_changed( bool enable );
	void on_emergency_bumper_info_changed( const rec::rpc::ClientInfoSet& );

	void on_set_emergency_bumper_changed( bool enable );
	void on_set_emergency_bumper_info_changed( const rec::rpc::ClientInfoSet& );

	void on_display_text_changed( const QString& text, unsigned int row, unsigned int col, bool clear_before, bool clear_after );
	void on_display_text_info_changed( const rec::rpc::ClientInfoSet& );

	void on_display_backlight_changed( bool on );
	void on_display_backlight_info_changed( const rec::rpc::ClientInfoSet& );

	void on_display_buttons_changed( bool up, bool down, bool back, bool enter );
	void on_display_buttons_info_changed( const rec::rpc::ClientInfoSet& );

	void on_display_vbar_changed( float value, unsigned int col, unsigned int start_row, unsigned int end_row );
	void on_display_vbar_info_changed( const rec::rpc::ClientInfoSet& );

	void on_display_hbar_changed( float value, unsigned int row, unsigned int start_col, unsigned int end_col );
	void on_display_hbar_info_changed( const rec::rpc::ClientInfoSet& );

	void on_display_progress_changed( unsigned int step, unsigned int row );
	void on_display_progress_info_changed( const rec::rpc::ClientInfoSet& );

	void on_display_clear_changed();
	void on_display_clear_info_changed( const rec::rpc::ClientInfoSet& );

	void on_parameters_changed( const QMap< QString, QVariant >& values );
	void on_parameters_info_changed( const rec::rpc::ClientInfoSet& );

	void on_current_controller_changed( const rec::rpc::ClientInfo& );
	void on_current_controller_info_changed( const rec::rpc::ClientInfoSet& );

	void on_ea_version_changed( const QString& board, quint16 firmware_major, quint16 firmware_minor, quint16 firmware_patch );
	void on_ea_version_info_changed( const rec::rpc::ClientInfoSet& );

	void on_ea09_ip4address_changed( const QHostAddress& address, const QHostAddress& netmask );
	void on_ea09_ip4address_info_changed( const rec::rpc::ClientInfoSet& );

	void on_set_ea09_ip4address_changed( const QHostAddress& address, const QHostAddress& netmask );
	void on_set_ea09_ip4address_info_changed( const rec::rpc::ClientInfoSet& );

	void on_clients_connected_changed( const QVector< rec::rpc::ClientInfo >& clients );
	void on_clients_connected_info_changed( const rec::rpc::ClientInfoSet& );

	void on_digital_input_changed( const QVector< bool >& );
	void on_digital_input_info_changed( const rec::rpc::ClientInfoSet& );

	void on_analog_input_changed( const QVector< float >& );
	void on_analog_input_info_changed( const rec::rpc::ClientInfoSet& );

	void on_distance_sensors_changed( const QVector< float >& );
	void on_distance_sensors_info_changed( const rec::rpc::ClientInfoSet& );

	void on_bumper_changed( bool );
	void on_bumper_info_changed( const rec::rpc::ClientInfoSet& );

	void on_battery_changed( float battery_voltage, float system_current, bool ext_power, int num_chargers, const QString& batteryType, bool batteryLow, int batteryLowShutdownCounter );
	void on_battery_info_changed( const rec::rpc::ClientInfoSet& );

	void on_set_digital_output_changed( unsigned int index, bool state );
	void on_set_digital_output_info_changed( const rec::rpc::ClientInfoSet& );

	void on_set_digital_output_array_changed( const QVector<bool>& );

	void on_set_relay_changed( unsigned int index, bool state );
	void on_set_relay_info_changed( const rec::rpc::ClientInfoSet& );

	void on_set_relay_array_changed( const QVector<bool>& );

	void on_set_shutdown_changed( bool shutdown );
	void on_set_shutdown_info_changed( const rec::rpc::ClientInfoSet& );

	void on_power_button_changed( bool pressed );
	void on_power_button_info_changed( const rec::rpc::ClientInfoSet& );

	void on_request_shutdown_changed( bool pressed );
	void on_request_shutdown_info_changed( const rec::rpc::ClientInfoSet& );

	void on_scan0_changed( const QVector< float >& ranges,	const QVector< float >& intensities, const QVariantMap& parameters );
	void on_scan0_info_changed( const rec::rpc::ClientInfoSet& );

	void on_scan1_changed( const QVector< float >& ranges,	const QVector< float >& intensities, const QVariantMap& parameters );
	void on_scan1_info_changed( const rec::rpc::ClientInfoSet& );

	void on_scan2_changed( const QVector< float >& ranges,	const QVector< float >& intensities, const QVariantMap& parameters );
	void on_scan2_info_changed( const rec::rpc::ClientInfoSet& );

	void on_scan3_changed( const QVector< float >& ranges,	const QVector< float >& intensities, const QVariantMap& parameters );
	void on_scan3_info_changed( const rec::rpc::ClientInfoSet& );

	void on_cbha_readings_changed( const QVector<float>& pressures, bool pressureSensor, const QVector<float>& stringPots, float foilPot );

	void on_cbha_set_pressure_changed( const QVector<float>& pressures );

	void on_grappler_store_positions_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data );

	void on_grappler_set_power_changed( int line, bool power );

	void on_grappler_set_positions_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data );

	void on_grappler_servos_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data );

	void on_grappler_readings_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data );

	void on_grappler_toggle_torque_changed();

	void on_log_changed( const QString& publisher, const QString& message, int level );

	void on_set_log_level_changed( const QString& publisher, int level );

	void on_custom_message_changed( unsigned int id, const QByteArray& data );
	void on_custom_message0_changed( const QByteArray& data );
	void on_custom_message1_changed( const QByteArray& data );
	void on_custom_message2_changed( const QByteArray& data );
	void on_custom_message3_changed( const QByteArray& data );

	void on_kinect0_depth_changed( const QByteArray& data, const QByteArray& object_data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp );
	void on_kinect1_depth_changed( const QByteArray& data, const QByteArray& object_data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp );
	void on_kinect2_depth_changed( const QByteArray& data, const QByteArray& object_data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp );
	void on_kinect3_depth_changed( const QByteArray& data, const QByteArray& object_data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp );

	void on_kinect0_video_changed( const QByteArray& data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp );
	void on_kinect1_video_changed( const QByteArray& data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp );
	void on_kinect2_video_changed( const QByteArray& data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp );
	void on_kinect3_video_changed( const QByteArray& data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp );

	void on_kinect0_tilt_changed( double tilt );
	void on_kinect1_tilt_changed( double tilt );
	void on_kinect2_tilt_changed( double tilt );
	void on_kinect3_tilt_changed( double tilt );

	void on_kinect0_accel_changed( double x, double y, double z );
	void on_kinect1_accel_changed( double x, double y, double z );
	void on_kinect2_accel_changed( double x, double y, double z );
	void on_kinect3_accel_changed( double x, double y, double z );

	void on_process_status_changed( const rec::robotino::rpc::ProcessStatus& status );

	void on_process_output_changed( const rec::robotino::rpc::ProcessOutput& output );

	void on_disconnect_api1_clients_changed();
	void on_api1_setstate_received_changed();

	void on_charger0_info_changed( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state );
	void on_charger0_version_changed( int major, int minor, int patch );
	void on_charger0_error_changed( unsigned int time, const QString& message );
	void on_charger0_clear_error_changed();

	void on_charger1_info_changed( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state );
	void on_charger1_version_changed( int major, int minor, int patch );
	void on_charger1_error_changed( unsigned int time, const QString& message );
	void on_charger1_clear_error_changed();

	void on_charger2_info_changed( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state );
	void on_charger2_version_changed( int major, int minor, int patch );
	void on_charger2_error_changed( unsigned int time, const QString& message );
	void on_charger2_clear_error_changed();

	void on_gripper_state_changed( int state );

	void on_sensors_changed( const QVector< QString >& names, const QVector< float >& values, const QVector< QString >& units );
		
	void on_poseOnMap_changed(const QPointF&, double);
	
	void on_set_pid_parameters_changed(unsigned int motor, float kp, float ki, float kd);

	void on_smartlicense_changed(const QByteArray& jsonData);
	void on_smartdockingvis_changed(const QByteArray& jsonData);
	
	/*Factory 4*/
	void on_mclayout_changed(const QByteArray& jsonData);
	void on_mcstatus_changed(const QByteArray& jsonData);
	void on_pathnetwork_changed(const QByteArray& jsonData);
	void on_pathnetwork_edited_changed(const QByteArray& jsonData);
	void on_localizationMode_changed(const QByteArray& jsonData);
	void on_smartlog_changed(const QByteArray& jsonData);
	void on_smartnavigationplan_changed(const QByteArray& jsonData);
	void on_smartlocations_changed(const QByteArray& jsonData);
	void on_smartlocations_edited_changed(const QByteArray& jsonData);
	void on_smartstations_changed(const QByteArray& jsonData);
	void on_smartstations_edited_changed(const QByteArray& jsonData);
	void on_smartrobotinfo_changed(const QByteArray& jsonData);
	void on_smartmyrobotid_changed(const QByteArray& jsonData);
	void on_smartjoblist_changed(const QByteArray& jsonData);
	void on_mapDir_changed(const QByteArray& jsonData);
	void on_festool_charger_info_changed(const QByteArray& jsonData);
	void on_fleetcom_response_changed(const QByteArray& jsonData);
	void on_fleetcom_request_changed(const QString& message);
	void on_mapDir_response_changed(const QByteArray& jsonData, const QByteArray& data);
	void on_mapDir_request_changed(const QByteArray& jsonData, const QByteArray& data);

	void on_map_changed(const QByteArray&, int, int, float, float, float);
	void on_mapPlanner_changed(const QByteArray&, int width, int height, float resolution, float offsetx, float offsety);
	void on_mapPlannerEdited_changed(const QByteArray&, int width, int height, float resolution, float offsetx, float offsety);
	
	void on_json_message_with_data_changed(int topic, const QByteArray& jsonData, const QByteArray& data);

	void on_motor_debug_changed(int motor, const QVector<unsigned int>& times, const QVector<float>& speed_setpoints, const QVector<float>& speeds, const QVector<float>& currents, const QVector<float>& control_points);
	void on_start_motor_debug_changed(int motor);

	void on_upload_program_changed(const QByteArray& jsonData, const QByteArray& data);

private:
	QString _topicName;
	QStringList _publish_parameters;
	QTimer* _publish_timer;
	int _echoCount;
	int _echoMaxCount;
	
	void checkEchoCount();
	void publish_json_message_with_data(QString topicName);
};

#endif //_CLIENT_H_
