#include "rec/robotino/rpc/Server.h"
#include "rec/robotino/rpc/common.h"

#include "rec/rpc/Exception.h"

#include <QtCore>

#define JSON_WITH_DATA_TOPIC_FUNCTIONS( TOPICNAME ) \
	bool Server::set_##TOPICNAME(const QByteArray& jsonData, const QByteArray& payloadData) \
				{ \
		try \
								{ \
			PREPARE_TOPIC( rec_robotino_rpc_##TOPICNAME ); \
			data.jsonData() = jsonData; \
			data.data() = payloadData; \
			PUBLISH_TOPIC; \
								} \
		catch (const rec::rpc::Exception& e) \
								{ \
			Q_EMIT log(QString("Error set_%1: ").arg( #TOPICNAME ) + e.what(), 1); \
			return false; \
								} \
		return true; \
				} \
	\
	BEGIN_TOPICLISTENER_DEFINITION(Server, rec_robotino_rpc_##TOPICNAME ) \
		Q_EMIT TOPICNAME##_changed(data.jsonData(),data.data()); \
	END_TOPICLISTENER_DEFINITION \
	\
	BEGIN_TOPICINFOCHANGED_DEFINITION(Server, rec_robotino_rpc_##TOPICNAME ) \
		Q_EMIT TOPICNAME##_info_changed(info); \
	END_TOPICINFOCHANGED_DEFINITION

using namespace rec::robotino::rpc;

Server::Server( QObject* parent )
	: rec::rpc::Server( parent )
{
	rec::robotino::rpc::once();

	_name = qApp->applicationName();

			// Register the RPC functions to allow clients to invoke them.

	beginAddTopicGroup();

	REGISTER_FUNCTION( rec_robotino_rpc_set_parameter );
	REGISTER_FUNCTION( rec_robotino_rpc_set_parameters );
	REGISTER_FUNCTION( rec_robotino_rpc_get_parameter );
	REGISTER_FUNCTION( rec_robotino_rpc_get_parameters );
	REGISTER_FUNCTION( rec_robotino_rpc_contains_parameter );

	REGISTER_FUNCTION( rec_robotino_rpc_disconnect_client );

	REGISTER_FUNCTION( rec_robotino_rpc_process_launch );
	REGISTER_FUNCTION( rec_robotino_rpc_process_kill );
	REGISTER_FUNCTION( rec_robotino_rpc_process_terminate );
	REGISTER_FUNCTION( rec_robotino_rpc_process_getids );
	REGISTER_FUNCTION( rec_robotino_rpc_process_getstatus );

	ADD_ENQUEUEDTOPIC( rec_robotino_rpc_process_status );
	ADD_ENQUEUEDTOPIC( rec_robotino_rpc_process_output );

	ADD_PERMANENT_TOPIC( rec_robotino_rpc_camera0_settings, 0 );
	ADD_TOPIC( rec_robotino_rpc_set_camera0_settings, 0 );
	ADD_TOPIC( rec_robotino_rpc_set_camera0_control, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_camera0_capabilities, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_camera0_calibration, 0 );

	ADD_PERMANENT_TOPIC( rec_robotino_rpc_camera1_settings, 0 );
	ADD_TOPIC( rec_robotino_rpc_set_camera1_settings, 0 );
	ADD_TOPIC( rec_robotino_rpc_set_camera1_control, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_camera1_capabilities, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_camera1_calibration, 0 );

	ADD_PERMANENT_TOPIC( rec_robotino_rpc_camera2_settings, 0 );
	ADD_TOPIC( rec_robotino_rpc_set_camera2_settings, 0 );
	ADD_TOPIC( rec_robotino_rpc_set_camera2_control, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_camera2_capabilities, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_camera2_calibration, 0 );

	ADD_PERMANENT_TOPIC( rec_robotino_rpc_camera3_settings, 0 );
	ADD_TOPIC( rec_robotino_rpc_set_camera3_settings, 0 );
	ADD_TOPIC( rec_robotino_rpc_set_camera3_control, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_camera3_capabilities, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_camera3_calibration, 0 );

	ADD_TOPIC( rec_robotino_rpc_motor0_setpoint, 0 );
	ADD_TOPIC( rec_robotino_rpc_motor0_reset_position, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_motor0_mode, 0 );
	ADD_TOPIC( rec_robotino_rpc_set_motor0_mode, 0 );

	ADD_TOPIC( rec_robotino_rpc_motor1_setpoint, 0 );
	ADD_TOPIC( rec_robotino_rpc_motor1_reset_position, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_motor1_mode, 0 );
	ADD_TOPIC( rec_robotino_rpc_set_motor1_mode, 0 );

	ADD_TOPIC( rec_robotino_rpc_motor2_setpoint, 0 );
	ADD_TOPIC( rec_robotino_rpc_motor2_reset_position, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_motor2_mode, 0 );
	ADD_TOPIC( rec_robotino_rpc_set_motor2_mode, 0 );
	
	ADD_TOPIC( rec_robotino_rpc_motor3_setpoint, 0 );
	ADD_TOPIC( rec_robotino_rpc_motor3_reset_position, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_motor3_mode, 0 );
	ADD_TOPIC( rec_robotino_rpc_set_motor3_mode, 0 );

	ADD_TOPIC( rec_robotino_rpc_motor_setpoints, 0 );

	ADD_PERMANENT_TOPIC( rec_robotino_rpc_motor_readings, 0 );

	ADD_TOPIC( rec_robotino_rpc_omnidrive, 0 );

	ADD_PERMANENT_TOPIC( rec_robotino_rpc_odometry, 0 );
	ADD_TOPIC( rec_robotino_rpc_set_odometry, 0 );
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_pose, 0);
	ADD_TOPIC(rec_robotino_rpc_northstar, 0);
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_set_northstar_parameters, 0 );
	ADD_TOPIC(rec_robotino_rpc_gyroscope, 0);
	ADD_TOPIC(rec_robotino_rpc_gyroscope_ext, 0);
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_emergency_bumper, 0 );
	ADD_TOPIC( rec_robotino_rpc_set_emergency_bumper, 0 );
	ADD_ENQUEUEDTOPIC( rec_robotino_rpc_display_text );
	ADD_TOPIC( rec_robotino_rpc_display_backlight, 0 );
	ADD_TOPIC( rec_robotino_rpc_display_buttons, 0 );
	ADD_TOPIC( rec_robotino_rpc_display_vbar, 0 );
	ADD_TOPIC( rec_robotino_rpc_display_hbar, 0 );
	ADD_TOPIC( rec_robotino_rpc_display_progress, 0 );
	ADD_TOPIC( rec_robotino_rpc_display_clear, 0 );

	ADD_PERMANENT_TOPIC( rec_robotino_rpc_digital_input, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_analog_input, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_distance_sensors, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_bumper, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_battery, 0 );
	ADD_ENQUEUEDTOPIC( rec_robotino_rpc_set_digital_output );
	ADD_TOPIC( rec_robotino_rpc_set_digital_output_array, 0 );
	ADD_ENQUEUEDTOPIC( rec_robotino_rpc_set_relay );
	ADD_TOPIC( rec_robotino_rpc_set_relay_array, 0 );
	ADD_TOPIC( rec_robotino_rpc_set_shutdown, 0 );
	ADD_TOPIC(rec_robotino_rpc_start_hotswap, 0);
	ADD_TOPIC(rec_robotino_rpc_start_motor_debug, 0);
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_power_button, 0 );
	ADD_TOPIC( rec_robotino_rpc_request_shutdown, 0 );
	ADD_ENQUEUEDTOPIC( rec_robotino_rpc_set_pid_parameters );
	ADD_TOPIC( rec_robotino_rpc_gripper_state, 0 );

	ADD_TOPIC( rec_robotino_rpc_cbha_readings, 0 );
	ADD_TOPIC( rec_robotino_rpc_cbha_set_pressure, 0 );
	ADD_TOPIC( rec_robotino_rpc_cbha_set_compressors_enabled, 0 );
	ADD_TOPIC( rec_robotino_rpc_cbha_set_water_drain_valve, 0 );
	ADD_TOPIC( rec_robotino_rpc_cbha_set_gripper_valve1, 0 );
	ADD_TOPIC( rec_robotino_rpc_cbha_set_gripper_valve2, 0 );

	ADD_TOPIC( rec_robotino_rpc_grappler_store_positions, 0 );
	ADD_TOPIC( rec_robotino_rpc_grappler_set_power, 0 );
	ADD_TOPIC( rec_robotino_rpc_grappler_set_positions, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_grappler_servos, 0 );
	ADD_TOPIC( rec_robotino_rpc_grappler_readings, 0 );
	ADD_TOPIC( rec_robotino_rpc_grappler_toggle_torque, 0 );

	ADD_PERMANENT_SERVERONLY_TOPIC( rec_robotino_rpc_current_controller, 0 );
	ADD_PERMANENT_SERVERONLY_TOPIC( rec_robotino_rpc_clients_connected, 0 );

	ADD_PERMANENT_TOPIC( rec_robotino_rpc_ea09_ip4address, 0 );
	ADD_TOPIC( rec_robotino_rpc_set_ea09_ip4address, 0 );

	ADD_PERMANENT_TOPIC( rec_robotino_rpc_ea_version, 0 );

	ADD_PERMANENT_SERVERONLY_TOPIC( rec_robotino_rpc_parameters, 0 );

	ADD_ENQUEUEDTOPIC( rec_robotino_rpc_log );
	ADD_ENQUEUEDTOPIC( rec_robotino_rpc_set_log_level );

	ADD_ENQUEUEDTOPIC( rec_robotino_rpc_custom_message );

	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect0_set_tilt, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect1_set_tilt, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect2_set_tilt, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect3_set_tilt, 0 );
	ADD_TOPIC( rec_robotino_rpc_kinect0_tilt, 0 );
	ADD_TOPIC( rec_robotino_rpc_kinect1_tilt, 0 );
	ADD_TOPIC( rec_robotino_rpc_kinect2_tilt, 0 );
	ADD_TOPIC( rec_robotino_rpc_kinect3_tilt, 0 );
	ADD_TOPIC( rec_robotino_rpc_kinect0_accel, 0 );
	ADD_TOPIC( rec_robotino_rpc_kinect1_accel, 0 );
	ADD_TOPIC( rec_robotino_rpc_kinect2_accel, 0 );
	ADD_TOPIC( rec_robotino_rpc_kinect3_accel, 0 );
	ADD_TOPIC( rec_robotino_rpc_kinect0_set_led, 0 );
	ADD_TOPIC( rec_robotino_rpc_kinect1_set_led, 0 );
	ADD_TOPIC( rec_robotino_rpc_kinect2_set_led, 0 );
	ADD_TOPIC( rec_robotino_rpc_kinect3_set_led, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect0_led, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect1_led, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect2_led, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect3_led, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect0_set_video_format, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect1_set_video_format, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect2_set_video_format, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect3_set_video_format, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect0_video_format, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect1_video_format, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect2_video_format, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect3_video_format, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect0_set_depth_format, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect1_set_depth_format, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect2_set_depth_format, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect3_set_depth_format, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect0_depth_format, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect1_depth_format, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect2_depth_format, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_kinect3_depth_format, 0 );

	ADD_TOPIC( rec_robotino_rpc_disconnect_api1_clients, 0 );
	ADD_TOPIC( rec_robotino_rpc_api1_setstate_received, 0 );

	ADD_PERMANENT_TOPIC( rec_robotino_rpc_poseOnMap, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_initialPose, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_navGoal, 0 );

	ADD_TOPIC( rec_robotino_rpc_charger0_info, 0 );
	ADD_TOPIC( rec_robotino_rpc_charger1_info, 0 );
	ADD_TOPIC( rec_robotino_rpc_charger2_info, 0 );

	ADD_PERMANENT_TOPIC( rec_robotino_rpc_charger0_version, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_charger1_version, 0 );
	ADD_PERMANENT_TOPIC( rec_robotino_rpc_charger2_version, 0 );

	ADD_TOPIC( rec_robotino_rpc_charger0_error, 0 );
	ADD_TOPIC( rec_robotino_rpc_charger1_error, 0 );
	ADD_TOPIC( rec_robotino_rpc_charger2_error, 0 );

	ADD_TOPIC( rec_robotino_rpc_charger0_clear_error, 0 );
	ADD_TOPIC( rec_robotino_rpc_charger1_clear_error, 0 );
	ADD_TOPIC( rec_robotino_rpc_charger2_clear_error, 0 );

	ADD_TOPIC(rec_robotino_rpc_sensors, 0);
	ADD_TOPIC(rec_robotino_rpc_motor_debug, 0);

	/*Factory 4*************************/
	ADD_ENQUEUEDTOPIC(rec_robotino_rpc_fleetcom_request);

	ADD_PERMANENT_TOPIC(rec_robotino_rpc_mclayout, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_mcstatus, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_pathnetwork, 0);
	ADD_TOPIC(rec_robotino_rpc_pathnetwork_edited, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_localizationMode, 0);
	ADD_ENQUEUEDTOPIC(rec_robotino_rpc_smartlog);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_smartnavigationplan, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_smartlocations, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_smartstations, 0);
	ADD_TOPIC(rec_robotino_rpc_smartlocations_edited, 0);
	ADD_TOPIC(rec_robotino_rpc_smartstations_edited, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_smartrobotinfo, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_smartmyrobotid, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_smartjoblist, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_smartlicense, 0);
	ADD_TOPIC(rec_robotino_rpc_smartdockingvis, 0);
	ADD_TOPIC(rec_robotino_rpc_festool_charger_info, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_mapDir, 0);
	ADD_ENQUEUEDTOPIC(rec_robotino_rpc_fleetcom_response);

	ADD_TOPIC(rec_robotino_rpc_mapDir_request, 0);
	ADD_TOPIC(rec_robotino_rpc_mapDir_response, 0);
	ADD_TOPIC(rec_robotino_rpc_upload_program, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_map, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_mapPlanner, 0);

	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_0, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_1, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_2, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_3, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_4, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_5, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_6, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_7, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_8, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_9, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_10, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_11, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_12, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_13, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_14, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_15, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_16, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_17, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_18, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_19, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_20, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_21, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_22, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_23, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_24, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_25, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_26, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_27, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_28, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_29, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_30, 0);
	ADD_TOPIC(rec_robotino_rpc_json_message_with_data_topic_31, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_json_message_with_data_topic_32, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_json_message_with_data_topic_33, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_json_message_with_data_topic_34, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_json_message_with_data_topic_35, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_json_message_with_data_topic_36, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_json_message_with_data_topic_37, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_json_message_with_data_topic_38, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_json_message_with_data_topic_39, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_json_message_with_data_topic_40, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_json_message_with_data_topic_41, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_json_message_with_data_topic_42, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_json_message_with_data_topic_43, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_json_message_with_data_topic_44, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_json_message_with_data_topic_45, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_json_message_with_data_topic_46, 0);
	ADD_PERMANENT_TOPIC(rec_robotino_rpc_json_message_with_data_topic_47, 0);
	ADD_ENQUEUEDTOPIC(rec_robotino_rpc_json_message_with_data_topic_48);
	ADD_ENQUEUEDTOPIC(rec_robotino_rpc_json_message_with_data_topic_49);
	ADD_ENQUEUEDTOPIC(rec_robotino_rpc_json_message_with_data_topic_50);
	ADD_ENQUEUEDTOPIC(rec_robotino_rpc_json_message_with_data_topic_51);
	ADD_ENQUEUEDTOPIC(rec_robotino_rpc_json_message_with_data_topic_52);
	ADD_ENQUEUEDTOPIC(rec_robotino_rpc_json_message_with_data_topic_53);
	ADD_ENQUEUEDTOPIC(rec_robotino_rpc_json_message_with_data_topic_54);
	ADD_ENQUEUEDTOPIC(rec_robotino_rpc_json_message_with_data_topic_55);
	ADD_ENQUEUEDTOPIC(rec_robotino_rpc_json_message_with_data_topic_56);
	ADD_ENQUEUEDTOPIC(rec_robotino_rpc_json_message_with_data_topic_57);
	ADD_ENQUEUEDTOPIC(rec_robotino_rpc_json_message_with_data_topic_58);
	ADD_ENQUEUEDTOPIC(rec_robotino_rpc_json_message_with_data_topic_59);
	ADD_ENQUEUEDTOPIC(rec_robotino_rpc_json_message_with_data_topic_60);
	ADD_ENQUEUEDTOPIC(rec_robotino_rpc_json_message_with_data_topic_61);
	ADD_ENQUEUEDTOPIC(rec_robotino_rpc_json_message_with_data_topic_62);
	ADD_ENQUEUEDTOPIC(rec_robotino_rpc_json_message_with_data_topic_63);

	ADD_TOPIC(rec_robotino_rpc_mapPlannerEdited, 0);

	endAddTopicGroup();

	setGreeting( "Robotino RPC Server" );

	bool success = true;

	success &= (bool)connect( this, SIGNAL( listening() ), SLOT( on_listening() ) );
	Q_ASSERT( success );

	success &= (bool)connect( this, SIGNAL( closed() ), SLOT( on_closed() ) );
	Q_ASSERT( success );

	success &= (bool)connect( this, SIGNAL( serverError( QAbstractSocket::SocketError, const QString& ) ), SLOT( on_serverError( QAbstractSocket::SocketError, const QString& ) ) );
	Q_ASSERT( success );

	success &= (bool)connect( this, SIGNAL( clientConnected( const rec::rpc::ClientInfo& ) ), SLOT( on_clientConnected( const rec::rpc::ClientInfo& ) ) );
	Q_ASSERT( success );

	success &= (bool)connect( this, SIGNAL( clientDisconnected( const rec::rpc::ClientInfo& ) ), SLOT( on_clientDisconnected( const rec::rpc::ClientInfo& ) ) );
	Q_ASSERT( success );
	
	//_clients.append( rec::rpc::ClientInfo( QHostAddress(), 0, "server" ) );
}

Server::~Server()
{
}

void Server::init()
{
	beginAddTopicGroup();

	ADD_TOPIC( rec_robotino_rpc_image0, getParameter( "~/SHM/image0", 0x2400FF ).toInt() ); //1024*768*3+256
	ADD_TOPIC( rec_robotino_rpc_image1, getParameter( "~/SHM/image1", 0xE10FF ).toInt() ); //640*480*3+256
	ADD_TOPIC( rec_robotino_rpc_image2, getParameter( "~/SHM/image2", 0xE10FF ).toInt() ); //640*480*3+256
	ADD_TOPIC( rec_robotino_rpc_image3, getParameter( "~/SHM/image3", 0xE10FF ).toInt() ); //640*480*3+256

	ADD_TOPIC( rec_robotino_rpc_scan0, getParameter( "~/SHM/scan0", 0x1FFF ).toInt() );
	ADD_TOPIC( rec_robotino_rpc_scan1, getParameter( "~/SHM/scan1", 0x1FFF ).toInt() );
	ADD_TOPIC( rec_robotino_rpc_scan2, getParameter( "~/SHM/scan2", 0x1FFF ).toInt() );
	ADD_TOPIC( rec_robotino_rpc_scan3, getParameter( "~/SHM/scan3", 0x1FFF ).toInt() );

	ADD_TOPIC( rec_robotino_rpc_custom_message0, getParameter( "~/SHM/message0", 0 ).toInt() );
	ADD_TOPIC( rec_robotino_rpc_custom_message1, getParameter( "~/SHM/message1", 0 ).toInt() );
	ADD_TOPIC( rec_robotino_rpc_custom_message2, getParameter( "~/SHM/message2", 0 ).toInt() );
	ADD_TOPIC( rec_robotino_rpc_custom_message3, getParameter( "~/SHM/message3", 0 ).toInt() );

	ADD_TOPIC( rec_robotino_rpc_kinect0_depth, getParameter( "~/SHM/kinect0_depth", 640*480*2+640*480*12+256 ).toInt() );
	ADD_TOPIC( rec_robotino_rpc_kinect1_depth, getParameter( "~/SHM/kinect1_depth", 0 ).toInt() );
	ADD_TOPIC( rec_robotino_rpc_kinect2_depth, getParameter( "~/SHM/kinect2_depth", 0 ).toInt() );
	ADD_TOPIC( rec_robotino_rpc_kinect3_depth, getParameter( "~/SHM/kinect3_depth", 0 ).toInt() );

	ADD_TOPIC( rec_robotino_rpc_kinect0_video, getParameter( "~/SHM/kinect0_video", 640*480*3+256 ).toInt() );
	ADD_TOPIC( rec_robotino_rpc_kinect1_video, getParameter( "~/SHM/kinect1_video", 0 ).toInt() );
	ADD_TOPIC( rec_robotino_rpc_kinect2_video, getParameter( "~/SHM/kinect2_video", 0 ).toInt() );
	ADD_TOPIC( rec_robotino_rpc_kinect3_video, getParameter( "~/SHM/kinect3_video", 0 ).toInt() );

	endAddTopicGroup();
}

void Server::readParameters( const QString& path, const QString& userpath )
{
	_local_parameters_path = path;
	_local_user_parameters_path = userpath;

	QSettings param( _local_parameters_path, QSettings::IniFormat );
	switch( param.status() )
	{
	case QSettings::AccessError:
		Q_EMIT log( QString( "AccessError %1" ).arg( path ) );
		break;

	case QSettings::FormatError:
		Q_EMIT log( QString( "FormatError %1" ).arg( path ) );
		break;

	default:
		Q_EMIT log( QString( "Success reading %1" ).arg( path ) );
	}

	QSettings userparam( _local_user_parameters_path, QSettings::IniFormat );
	switch( userparam.status() )
	{
	case QSettings::AccessError:
		Q_EMIT log( QString( "AccessError %1" ).arg( userpath ) );
		break;

	case QSettings::FormatError:
		Q_EMIT log( QString( "FormatError %1" ).arg( userpath ) );
		break;

	default:
		Q_EMIT log( QString( "Success reading %1" ).arg( userpath ) );
	}

	QString str;
	QTextStream stream( &str );
	stream << '\n';

	QSet< QString > keys;
	Q_FOREACH( const QString& key, param.allKeys() )
	{
		_local_parameters[ key ] = param.value( key );
		keys << key;
	}
	Q_FOREACH( const QString& key, userparam.allKeys() )
	{
		_local_user_parameters[ key ] = userparam.value( key );
		keys << key;
	}

	Q_FOREACH( const QString& key, keys )
	{
		QVariant v;
		if( _local_user_parameters.contains( key ) )
		{
			v = _local_user_parameters.value( key );
			stream << "*" << key << " : " << v.toString() << '\n';
		}
		else
		{
			v = _local_parameters.value( key );
			stream << key << " : " << v.toString() << '\n';
		}
	
		_parameters[ _name + "/" + key ] = v;
	}

	publishParameters();

	Q_EMIT log( str );
}

void Server::setParameters( const QMap< QString, QVariant >& values )
{
	rec_robotino_rpc_parameters_t::const_iterator iter = values.constBegin();
	while( values.constEnd() != iter )
	{
		_parameters[ iter.key() ] = iter.value();
		++iter;
	}

	publishParameters();
}

void Server::saveUserParameters()
{
	if( _local_user_parameters_path.isEmpty() )
	{
		return;
	}

	QSettings userparam( _local_user_parameters_path, QSettings::IniFormat );
	QMap< QString, QVariant >::const_iterator iter = _local_user_parameters.constBegin();
	while( _local_user_parameters.constEnd() != iter )
	{
		userparam.setValue( iter.key(), iter.value() );
		++iter;
	}
	userparam.sync();
}

void Server::setParameter( const QString& key, const QVariant& value )
{
	QString tmp = key;

	if( key.startsWith( "~/" ) )
	{
		tmp.remove( "~/" );

		_local_user_parameters[ key ] =  value;
		saveUserParameters();

		tmp = _name + "/" + tmp;
	}

	_parameters[ tmp ] = value;
	publishParameters();
}

QVariant Server::getParameter( const QString& key, const QVariant& defaultValue ) const
{
	if( key.startsWith( "~/" ) )
	{
		QString tmp = key;
		tmp.remove( "~/" );

		if( _local_user_parameters.contains( tmp ) )
		{
			return _local_user_parameters.value( key );
		}
		
		return _local_parameters.value( tmp, defaultValue );
	}

	return _parameters.value( key, defaultValue );
}

QMap< QString, QVariant > Server::getParameters() const
{
	return _parameters;
}

bool Server::containsParameter( const QString& key )
{
	if( key.startsWith( "~/" ) )
	{
		QString tmp = key;
		tmp.remove( "~/" );

		return _local_parameters.contains( tmp ) || _local_user_parameters.contains( tmp );
	}

	return _parameters.contains( key );
}

void Server::publishParameters()
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_parameters, _parameters );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error publishParameters: " ) + e.what() );
	}
}

bool Server::set_motor0_setpoint( float speed )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_motor0_setpoint, speed );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_motor0_setpoint: " ) + e.what() );
		return false;
	}
	return true;
}

bool Server::set_motor1_setpoint( float speed )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_motor1_setpoint, speed );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_motor1_setpoint: " ) + e.what() );
		return false;
	}
	return true;
}
bool Server::set_motor2_setpoint( float speed )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_motor2_setpoint, speed );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_motor2_setpoint: " ) + e.what() );
		return false;
	}
	return true;
}

bool Server::set_motor3_setpoint( float speed )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_motor3_setpoint, speed );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_motor3_setpoint: " ) + e.what() );
		return false;
	}
	return true;
}

bool Server::set_set_camera0_settings( unsigned int width, unsigned int height, const QString& format )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_set_camera0_settings );
		data.width() = width;
		data.height() = height;
		data.format() = format;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_set_camera0_settings: " ) + e.what() );
		return false;
	}
	return true;
}

bool Server::set_odometry( double x, double y, double phi )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_odometry );
		data.x() = x;
		data.y() = y;
		data.phi() = phi;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_odometry: " ) + e.what() );
		return false;
	}
	return true;
}

bool Server::set_set_odometry( double x, double y, double phi )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_set_odometry );
		data.x() = x;
		data.y() = y;
		data.phi() = phi;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_set_odometry: " ) + e.what() );
		return false;
	}
	return true;
}

bool Server::set_pose(double x, double y, double phi, double errx, double erry, double errphi, unsigned int sequence)
{
	try
	{
		PREPARE_TOPIC(rec_robotino_rpc_pose);
		data.x() = x;
		data.y() = y;
		data.phi() = phi;
		data.errx() = errx;
		data.erry() = erry;
		data.errphi() = errphi;
		data.seq() = sequence;
		PUBLISH_TOPIC;
	}
	catch (const rec::rpc::Exception& e)
	{
		Q_EMIT log(QString("Error set_pose: ") + e.what(), 1);
		return false;
	}
	return true;
}

bool Server::set_omnidrive( float vx, float vy, float omega )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_omnidrive );
		data.vx() = vx;
		data.vy() = vy;
		data.omega() = omega;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_omnidrive: " ) + e.what() );
		return false;
	}
	return true;
}

bool Server::set_set_digital_output( unsigned int index, bool state )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_set_digital_output );
		data.index() = index;
		data.state() = state;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_set_digital_output: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_set_relay( unsigned int index, bool state )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_set_relay );
		data.index() = index;
		data.state() = state;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_set_digital_output: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_set_pid_parameters( unsigned int motor, float kp, float ki, float kd )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_set_pid_parameters );
		data.motor() = motor;
		data.kp() = kp;
		data.ki() = ki;
		data.kd() = kd;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_set_pid_parameters: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_northstar( unsigned int sequenceNumber,
							unsigned int roomId,
							unsigned int numSpotsVisible,
							float posX,
							float posY,
							float posTheta,
							unsigned int magSpot0,
							unsigned int magSpot1 )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_northstar );
		data.sequenceNumber() = sequenceNumber;
		data.roomId() = roomId;
		data.numSpotsVisible() = numSpotsVisible;
		data.posX() = posX;
		data.posY() = posY;
		data.posTheta() = posTheta;
		data.magSpot0() = magSpot0;
		data.magSpot1() = magSpot1;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_northstar: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_set_northstar_parameters( unsigned int roomId, float ceilingCal )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_set_northstar_parameters );
		data.roomId() = roomId;
		data.ceilingCal() = ceilingCal;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_set_northstar_parameters: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_request_shutdown( bool doit )
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_request_shutdown, doit );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_request_shutdown: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_log( const QString& message, int level )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_log );
		data.publisher() = QCoreApplication::applicationName();
		data.message() = message;
		data.level() = level;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_log: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_process_status( const rec::robotino::rpc::ProcessStatus& status )
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_process_status, status );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_process_status: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_process_output( const rec::robotino::rpc::ProcessOutput& output )
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_process_output, output );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_process_status: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_set_motor3_mode( const QString& mode )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_set_motor3_mode, mode );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_set_motor0_mode: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_image( unsigned int cameraNumber, const QByteArray& imageData, unsigned int width, unsigned int height, unsigned int step, const QString& format )
{
	switch( cameraNumber )
	{
	case 0:
		return set_image0( imageData, width, height, step, format );

	case 1:
		return set_image1( imageData, width, height, step, format );

	case 2:
		return set_image2( imageData, width, height, step, format );

	case 3:
		return set_image3( imageData, width, height, step, format );

	default:
		Q_EMIT log( "camera number out of range", 1 );
		return false;
	}
}

bool Server::set_image0( const QByteArray& imageData, unsigned int width, unsigned int height, unsigned int step, const QString& format )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_image0 );
		data.data() = imageData;
		data.width() = width;
		data.height() = height;
		data.step() = step;
		data.format() = format;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_image0: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_image1( const QByteArray& imageData, unsigned int width, unsigned int height, unsigned int step, const QString& format )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_image1 );
		data.data() = imageData;
		data.width() = width;
		data.height() = height;
		data.step() = step;
		data.format() = format;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_image1: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_image2( const QByteArray& imageData, unsigned int width, unsigned int height, unsigned int step, const QString& format )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_image2 );
		data.data() = imageData;
		data.width() = width;
		data.height() = height;
		data.step() = step;
		data.format() = format;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_image2: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_image3( const QByteArray& imageData, unsigned int width, unsigned int height, unsigned int step, const QString& format )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_image3 );
		data.data() = imageData;
		data.width() = width;
		data.height() = height;
		data.step() = step;
		data.format() = format;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_image3: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_motor_readings( const QVector< float >& speeds, const QVector< int >& positions, const QVector< float >& currents, float time_delta )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_motor_readings );
		data.speeds() = speeds;
		data.positions() = positions;
		data.currents() = currents;
		data.time_delta() = time_delta;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_motor_readings: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_distance_sensors( const QVector< float >& in )
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_distance_sensors, in );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_distance_sensors: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_bumper( bool bump )
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_bumper, bump );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_bumper: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_scan( unsigned int laserNumber, QVector< float > ranges, QVector< float > intensities, QVariantMap parameters )
{
	switch( laserNumber )
	{
	case 0:
		return set_scan0( ranges, intensities, parameters );

	case 1:
		return set_scan1( ranges, intensities, parameters );

	case 2:
		return set_scan2( ranges, intensities, parameters );

	case 3:
		return set_scan3( ranges, intensities, parameters );

	default:
		Q_EMIT log( "laserNumber out of range", 1 );
		return false;
	}
}

bool Server::set_scan0( QVector< float > ranges, QVector< float > intensities, QVariantMap parameters )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_scan0 );
		data.ranges() = ranges;
		data.intensities() = intensities;
		data.parameters() = parameters;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_scan0: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_scan1( QVector< float > ranges, QVector< float > intensities, QVariantMap parameters )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_scan1 );
		data.ranges() = ranges;
		data.intensities() = intensities;
		data.parameters() = parameters;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_scan1: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_scan2( QVector< float > ranges, QVector< float > intensities, QVariantMap parameters )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_scan2 );
		data.ranges() = ranges;
		data.intensities() = intensities;
		data.parameters() = parameters;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_scan2: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_scan3( QVector< float > ranges, QVector< float > intensities, QVariantMap parameters )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_scan3 );
		data.ranges() = ranges;
		data.intensities() = intensities;
		data.parameters() = parameters;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_scan3: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_kinect_depth( unsigned int id, const QByteArray& data, const QByteArray& object_data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	switch( id )
	{
	case 0:
		return set_kinect0_depth( data, object_data, width, height, format, stamp );

	case 1:
		return set_kinect1_depth( data, object_data, width, height, format, stamp );

	case 2:
		return set_kinect2_depth( data, object_data, width, height, format, stamp );

	case 3:
		return set_kinect3_depth( data, object_data, width, height, format, stamp );
	}

	return false;
}

bool Server::set_kinect0_depth( const QByteArray& depthData, const QByteArray& object_data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_kinect0_depth );
		data.data() = depthData;
		data.object_data() = object_data;
		data.width() = width;
		data.height() = height;
		data.format() = format;
		data.stamp() = stamp;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect0_depth: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_kinect1_depth( const QByteArray& depthData, const QByteArray& object_data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_kinect1_depth );
		data.data() = depthData;
		data.object_data() = object_data;
		data.width() = width;
		data.height() = height;
		data.format() = format;
		data.stamp() = stamp;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect1_depth: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_kinect2_depth( const QByteArray& depthData, const QByteArray& object_data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_kinect2_depth );
		data.data() = depthData;
		data.object_data() = object_data;
		data.width() = width;
		data.height() = height;
		data.format() = format;
		data.stamp() = stamp;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect2_depth: " ) + e.what(), 1 );
		return false;
	}
	return true;
}
				
bool Server::set_kinect3_depth( const QByteArray& depthData, const QByteArray& object_data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_kinect3_depth );
		data.data() = depthData;
		data.object_data() = object_data;
		data.width() = width;
		data.height() = height;
		data.format() = format;
		data.stamp() = stamp;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect3_depth: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_digital_input( const QVector< bool >& in )
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_digital_input, in );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_digital_input: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_analog_input( const QVector< float >& in )
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_analog_input, in );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_analog_input: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_battery(float battery_voltage, float system_current, bool ext_power, int num_chargers, const QString& batteryType, bool batteryLow, int batteryLowShutdownCounter)
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_battery );
		data.battery_voltage() = battery_voltage;
		data.system_current() = system_current;
		data.ext_power() = ext_power;
		data.num_chargers() = num_chargers;
		data.battery_type() = batteryType;
		data.battery_low() = batteryLow;
		data.battery_low_shutdown_counter() = batteryLowShutdownCounter;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_battery: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_gripper_state( int state )
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_gripper_state, state );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_gripper_state: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_grappler_servos( const QVector< rec::robotino::rpc::GrapplerServoInfo >& data )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_grappler_servos, data );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_grappler_servos: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_grappler_readings( const QVector< rec::robotino::rpc::GrapplerServoInfo >& data )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_grappler_readings, data );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_grappler_readings: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_cbha_readings( const QVector< float >& pressures, bool pressureSensor, const QVector< float >& stringPots, float foilPot )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_cbha_readings );
		data.pressures() = pressures;
		data.pressureSensor() = pressureSensor;
		data.stringPots() = stringPots;
		data.foilPot() = foilPot;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_cbha_readings: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Server::set_json_message_with_data(int topic, const QByteArray& jsonData, const QByteArray& payloadData)
{
	try
	{
		QString topicStr = QString("rec_robotino_rpc_json_message_with_data_topic_%1").arg(topic);
		QByteArray latin1Str = topicStr.toLatin1();
		const char* topicName = latin1Str.constData();
		topicrec_robotino_rpc_json_message_with_data_topic_0DataPtr dataPtr = rec::rpc::detail::createSerializable< topicrec_robotino_rpc_json_message_with_data_topic_0Data >(); \
		topicrec_robotino_rpc_json_message_with_data_topic_0Data& data = *dataPtr;
		data.jsonData() = jsonData;
		data.data() = payloadData;
		PUBLISH_TOPIC;
	}
	catch (const rec::rpc::Exception& e)
	{
		Q_EMIT log(QString("Error set_%1 for topic %2: ").arg("set_json_message_with_data").arg(topic) + e.what(), 1);
		return false;
	}
	return true;
}

void Server::on_listening()
{
	Q_EMIT log( QString( "Listening on port %1" ).arg( serverPort() ) );
}

void Server::on_closed()
{
	Q_EMIT log( QString( "Server closed" ) );
}

void Server::on_serverError( QAbstractSocket::SocketError error, const QString& errorString )
{
	Q_EMIT log( errorString );
}

void Server::on_clientConnected( const rec::rpc::ClientInfo& info )
{
	_clients.append( info );

	PREPARE_TOPIC( rec_robotino_rpc_clients_connected );
	data.ref() = _clients;
	PUBLISH_TOPIC;
}

void Server::on_clientDisconnected( const rec::rpc::ClientInfo& info )
{
	Q_EMIT log( QString( "Client disconnected %1 %2:%3" ).arg( info.name ).arg( info.address.toString() ).arg( info.port ), 1 );

	QVector< rec::rpc::ClientInfo >::iterator iter = _clients.begin();
	while( _clients.end() != iter )
	{
		if( info == *iter )
		{
			_clients.erase( iter );
			break;
		}
		++iter;
	}

	PREPARE_TOPIC( rec_robotino_rpc_clients_connected );
	data.ref() = _clients;
	PUBLISH_TOPIC;
}

QString Server::clientName( const QHostAddress& address, quint16 port ) const
{
	if( address.isNull() && 0 == port )
	{
		return "server";
	}

	Q_FOREACH( const rec::rpc::ClientInfo& info, _clients )
	{
		if( address == info.address && port == info.port )
		{
			return info.name;
		}
	}
	Q_ASSERT( false );
	return QString::null;
}

BEGIN_FUNCTION_DEFINITION( Server, rec_robotino_rpc_set_parameter )
	setParameter( param.key(), param.value() );
	result = true;
END_FUNCTION_DEFINITION

BEGIN_FUNCTION_DEFINITION( Server, rec_robotino_rpc_set_parameters )
	setParameters( param.ref() );
	result = true;
END_FUNCTION_DEFINITION

BEGIN_FUNCTION_DEFINITION( Server, rec_robotino_rpc_get_parameter )
	result = getParameter( param.ref() );
END_FUNCTION_DEFINITION

BEGIN_FUNCTION_DEFINITION( Server, rec_robotino_rpc_get_parameters )
	result = _parameters;
END_FUNCTION_DEFINITION

BEGIN_FUNCTION_DEFINITION( Server, rec_robotino_rpc_contains_parameter )
	result = _local_parameters.contains( param.ref() );
END_FUNCTION_DEFINITION

BEGIN_FUNCTION_DEFINITION( Server, rec_robotino_rpc_disconnect_client )
	disconnectClient( param.address(), param.port() );
	result = true;
END_FUNCTION_DEFINITION

BEGIN_FUNCTION_DEFINITION( Server, rec_robotino_rpc_process_launch )
	result = launchProcess( param.command(), param.parameters(), param.workingdirectory() );
END_FUNCTION_DEFINITION

BEGIN_FUNCTION_DEFINITION( Server, rec_robotino_rpc_process_kill )
	result = killProcess( param.ref() );
END_FUNCTION_DEFINITION

BEGIN_FUNCTION_DEFINITION( Server, rec_robotino_rpc_process_terminate )
	result = terminateProcess( param.ref() );
END_FUNCTION_DEFINITION

BEGIN_FUNCTION_DEFINITION( Server, rec_robotino_rpc_process_getids )
	result = getProcessIds();
END_FUNCTION_DEFINITION

BEGIN_FUNCTION_DEFINITION( Server, rec_robotino_rpc_process_getstatus )
	result = getProcessStatus( param.ref() );
END_FUNCTION_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_image0 )
	Q_EMIT image0_changed( data.data(), data.width(), data.height(), data.step(), data.format() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_image0 )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT image0_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_camera0_settings )
	Q_EMIT camera0_settings_changed( data.width(), data.height(), data.format() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_camera0_settings )
	Q_EMIT camera0_settings_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_camera0_capabilities )
	Q_EMIT camera0_capabilities_changed( data.resolutions() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_camera0_capabilities )
	Q_EMIT camera0_capabilities_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_motor_readings )
	Q_EMIT motor_readings_changed( data.speeds(), data.positions(), data.currents(), data.time_delta() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_motor_readings )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT motor_readings_info_changed( 2, info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_omnidrive )
		Q_EMIT omnidrive_changed( data.vx(), data.vy(), data.omega() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_omnidrive )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT omnidrive_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_odometry )
	Q_EMIT odometry_changed( data.x(), data.y(), data.phi() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_odometry )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT odometry_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_set_odometry )
	Q_EMIT set_odometry_changed( data.x(), data.y(), data.phi() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_set_odometry )
	Q_EMIT set_odometry_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION(Server, rec_robotino_rpc_pose)
	Q_EMIT pose_changed(data.x(), data.y(), data.phi(), data.errx(), data.erry(), data.errphi(), data.seq());
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION(Server, rec_robotino_rpc_pose)
	Q_EMIT pose_info_changed(info);
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_northstar )
	Q_EMIT northstar_changed( data.sequenceNumber(), data.roomId(), data.numSpotsVisible(), data.posX(), data.posY(), data.posTheta(), data.magSpot0(), data.magSpot1() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_northstar )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT northstar_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_set_northstar_parameters )
	Q_EMIT set_northstar_parameters_changed( data.roomId(), data.ceilingCal() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_set_northstar_parameters )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT set_northstar_parameters_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_gyroscope )
	Q_EMIT gyroscope_changed( data.angle(), data.rate() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_gyroscope )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT gyroscope_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_emergency_bumper )
	Q_EMIT emergency_bumper_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_emergency_bumper )
	Q_EMIT emergency_bumper_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_set_emergency_bumper )
	Q_EMIT set_emergency_bumper_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_set_emergency_bumper )
	Q_EMIT set_emergency_bumper_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_display_text )
	Q_EMIT display_text_changed( data.text(), data.row(), data.col(), data.clear_before(), data.clear_after() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_display_text )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT display_text_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_display_backlight)
	Q_EMIT display_backlight_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_display_backlight )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT display_backlight_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_display_buttons )
	Q_EMIT display_buttons_changed( data.up(), data.down(), data.back(), data.enter() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_display_buttons )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT display_buttons_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_display_vbar )
	Q_EMIT display_vbar_changed( data.value(), data.col(), data.start_row(), data.end_row() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_display_vbar )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT display_vbar_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_display_hbar )
	Q_EMIT display_hbar_changed( data.value(), data.row(), data.start_col(), data.end_col() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_display_hbar )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT display_hbar_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_display_progress )
	Q_EMIT display_progress_changed( data.step(), data.row() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_display_progress )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT display_progress_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_display_clear )
	Q_EMIT display_clear_changed();
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_display_clear )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT display_clear_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_ea_version )
	Q_EMIT ea_version_changed( data.board(), data.firmware_major(), data.firmware_minor(), data.firmware_patch() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_ea_version )
	Q_EMIT ea_version_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_ea09_ip4address )
	Q_EMIT ea09_ip4address_changed( data.address(), data.netmask() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_ea09_ip4address )
	Q_EMIT ea09_ip4address_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_set_ea09_ip4address )
	Q_EMIT set_ea09_ip4address_changed( data.address(), data.netmask() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_set_ea09_ip4address )
	Q_EMIT set_ea09_ip4address_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_digital_input )
	Q_EMIT digital_input_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_digital_input )
	Q_EMIT digital_input_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_analog_input )
	Q_EMIT analog_input_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_analog_input )
	Q_EMIT analog_input_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_distance_sensors )
	Q_EMIT distance_sensors_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_distance_sensors )
	Q_EMIT distance_sensors_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_bumper )
	Q_EMIT bumper_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_bumper )
	Q_EMIT bumper_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_battery )
	Q_EMIT battery_changed( data.battery_voltage(), data.system_current() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_battery )
	Q_EMIT battery_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_set_digital_output )
	Q_EMIT set_digital_output_changed( data.index(), data.state() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_set_digital_output )
	Q_EMIT set_digital_output_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_set_digital_output_array )
	Q_EMIT set_digital_output_array_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_set_digital_output_array )
	Q_EMIT set_digital_output_array_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_set_relay )
	Q_EMIT set_relay_changed( data.index(), data.state() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_set_relay )
	Q_EMIT set_relay_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_set_relay_array )
	Q_EMIT set_relay_array_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_set_relay_array )
	Q_EMIT set_relay_array_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_gripper_state )
	Q_EMIT gripper_state_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_gripper_state )
	Q_EMIT gripper_state_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_power_button )
	Q_EMIT power_button_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_power_button )
	Q_EMIT power_button_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_set_log_level )
	Q_EMIT set_log_level_changed( data.publisher(), data.level() );
	if( data.publisher() == QCoreApplication::applicationName() )
	{
		Q_EMIT logLevelChanged( data.level() );
	}
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_motor_setpoints )
	Q_EMIT motor_setpoints_changed( data.m0(), data.m1(), data.m2(), client );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_motor_setpoints )
	Q_EMIT motor_setpoints_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_motor0_setpoint )
	Q_EMIT motor0_setpoint_changed( data.ref(), client );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_motor0_setpoint )
	Q_EMIT motor0_setpoint_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_set_motor0_mode )
	Q_EMIT set_motor0_mode_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_set_motor0_mode )
	Q_EMIT set_motor0_mode_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_motor0_mode )
	Q_EMIT motor0_mode_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_motor0_mode )
	Q_EMIT motor0_mode_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_motor1_setpoint )
	Q_EMIT motor1_setpoint_changed( data.ref(), client );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_motor1_setpoint )
	Q_EMIT motor1_setpoint_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_set_motor1_mode )
	Q_EMIT set_motor1_mode_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_set_motor1_mode )
	Q_EMIT set_motor1_mode_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_motor1_mode )
	Q_EMIT motor1_mode_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_motor1_mode )
	Q_EMIT motor1_mode_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_motor2_setpoint )
	Q_EMIT motor2_setpoint_changed( data.ref(), client );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_motor2_setpoint )
	Q_EMIT motor2_setpoint_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_set_motor2_mode )
	Q_EMIT set_motor2_mode_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_set_motor2_mode )
	Q_EMIT set_motor2_mode_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_motor2_mode )
	Q_EMIT motor2_mode_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_motor2_mode )
	Q_EMIT motor2_mode_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_motor3_setpoint )
	Q_EMIT motor3_setpoint_changed( data.ref(), client );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_motor3_setpoint )
	Q_EMIT motor3_setpoint_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_set_motor3_mode )
	Q_EMIT set_motor3_mode_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_set_motor3_mode )
	Q_EMIT set_motor3_mode_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_motor3_mode )
	Q_EMIT motor3_mode_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_motor3_mode )
	Q_EMIT motor3_mode_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_grappler_toggle_torque )
	Q_EMIT grappler_toggle_torque_changed();
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_grappler_toggle_torque )
	Q_EMIT grappler_toggle_torque_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_grappler_set_power )
	Q_EMIT grappler_set_power_changed( data.line(), data.power() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_grappler_set_power )
	Q_EMIT grappler_set_power_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_grappler_set_positions )
	Q_EMIT grappler_set_positions_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_grappler_set_positions )
	Q_EMIT grappler_set_positions_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_cbha_set_pressure )
	Q_EMIT cbha_set_pressure_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_cbha_set_pressure )
	Q_EMIT cbha_set_pressure_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_cbha_set_compressors_enabled )
	Q_EMIT cbha_set_compressors_enabled_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_cbha_set_compressors_enabled )
	Q_EMIT cbha_set_compressors_enabled_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_cbha_set_water_drain_valve )
	Q_EMIT cbha_set_water_drain_valve_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_cbha_set_water_drain_valve )
	Q_EMIT cbha_set_water_drain_valve_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_cbha_set_gripper_valve1 )
	Q_EMIT cbha_set_gripper_valve1_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_cbha_set_gripper_valve1 )
	Q_EMIT cbha_set_gripper_valve1_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, rec_robotino_rpc_cbha_set_gripper_valve2 )
	Q_EMIT cbha_set_gripper_valve2_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_cbha_set_gripper_valve2 )
	Q_EMIT cbha_set_gripper_valve2_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_kinect0_depth )
	Q_EMIT kinect0_depth_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_kinect1_depth )
	Q_EMIT kinect1_depth_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_kinect2_depth )
	Q_EMIT kinect2_depth_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, rec_robotino_rpc_kinect3_depth )
	Q_EMIT kinect3_depth_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

JSON_WITH_DATA_TOPIC_FUNCTIONS(upload_program)