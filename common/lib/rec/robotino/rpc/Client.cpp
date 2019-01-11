#include "rec/robotino/rpc/Client.h"
#include "rec/robotino/rpc/common.h"

#include <iostream>

#define STR_VALUE(arg) #arg

#define MAP_TOPICLISTENER_ENABLED( TOPICNAME ) \
	_enableMap[ STR_VALUE( TOPICNAME ) ] = &Client::set_##TOPICNAME##_enabled; \
	_enableMap[ STR_VALUE( TOPICNAME##_info ) ] = &Client::set_##TOPICNAME##_info_enabled;

#define MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED( TOPICNAME ) \
{\
	MAP_TOPICLISTENER_ENABLED(TOPICNAME) \
	_json_message_with_data_topics_enableMap[TOPICNAME##_num] = &Client::set_##TOPICNAME##_enabled; \
	QString topicName( TOPICNAME##_name ); \
	if(false == topicName.isEmpty()) \
	{ \
		_enableMap[ topicName ] = &Client::set_##TOPICNAME##_enabled; \
		QString topicInfoName = topicName + "_info";\
		_enableMap[ topicInfoName ] = &Client::set_##TOPICNAME##_info_enabled; \
		_json_message_with_data_topics_alias_map[TOPICNAME##_num] = topicName; \
		_json_message_with_data_topics_alias_reverse_map[topicName]=TOPICNAME##_num; \
	} \
}

#define STRING_TOPIC_FUNCTIONS( TOPICNAME ) \
	bool Client::set_##TOPICNAME(const QString& message) \
	{ \
	try \
		{ \
		PREPARE_TOPIC(rec_robotino_rpc_##TOPICNAME); \
		data.message() = message; \
		PUBLISH_TOPIC; \
		} \
		catch (const rec::rpc::Exception& e) \
		{ \
		Q_EMIT log(QString("Error set_%1: ").arg(#TOPICNAME) + e.what(), 1); \
		return false; \
		} \
		return true; \
	} \
	\
	BEGIN_TOPICLISTENER_DEFINITION(Client, rec_robotino_rpc_##TOPICNAME) \
	Q_EMIT TOPICNAME##_changed(data.message()); \
	END_TOPICLISTENER_DEFINITION \
	\
	BEGIN_TOPICINFOCHANGED_DEFINITION(Client, rec_robotino_rpc_##TOPICNAME) \
	Q_EMIT TOPICNAME##_info_changed(info); \
	END_TOPICINFOCHANGED_DEFINITION

#define JSON_TOPIC_FUNCTIONS( TOPICNAME ) \
	bool Client::set_##TOPICNAME(const QByteArray& jsonData) \
	{ \
		try \
		{ \
			PREPARE_TOPIC( rec_robotino_rpc_##TOPICNAME ); \
			data.data() = jsonData; \
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
	BEGIN_TOPICLISTENER_DEFINITION(Client, rec_robotino_rpc_##TOPICNAME ) \
		Q_EMIT TOPICNAME##_changed(data.data()); \
	END_TOPICLISTENER_DEFINITION \
	\
	BEGIN_TOPICINFOCHANGED_DEFINITION(Client, rec_robotino_rpc_##TOPICNAME ) \
		Q_EMIT TOPICNAME##_info_changed(info); \
	END_TOPICINFOCHANGED_DEFINITION

#define JSON_WITH_DATA_TOPIC_FUNCTIONS( TOPICNAME ) \
	bool Client::set_##TOPICNAME(const QByteArray& jsonData, const QByteArray& payloadData) \
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
	BEGIN_TOPICLISTENER_DEFINITION(Client, rec_robotino_rpc_##TOPICNAME ) \
		Q_EMIT TOPICNAME##_changed(data.jsonData(),data.data()); \
	END_TOPICLISTENER_DEFINITION \
	\
	BEGIN_TOPICINFOCHANGED_DEFINITION(Client, rec_robotino_rpc_##TOPICNAME ) \
		Q_EMIT TOPICNAME##_info_changed(info); \
	END_TOPICINFOCHANGED_DEFINITION

#define JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(TOPICNAME) \
	BEGIN_TOPICLISTENER_DEFINITION(Client, rec_robotino_rpc_##TOPICNAME) \
	Q_EMIT json_message_with_data_changed(rec_robotino_rpc_##TOPICNAME##_num, data.jsonData(), data.data()); \
	END_TOPICLISTENER_DEFINITION \
	\
	BEGIN_TOPICINFOCHANGED_DEFINITION(Client, rec_robotino_rpc_##TOPICNAME) \
	Q_EMIT json_message_with_data_info_changed(rec_robotino_rpc_##TOPICNAME##_num, info); \
	END_TOPICINFOCHANGED_DEFINITION

using namespace rec::robotino::rpc;

Client::Client( QObject* parent )
	: rec::rpc::Client( parent )
{
	rec::robotino::rpc::once();

	setExpectedGreeting( "Robotino RPC Server" );

	bool ok = true;

	ok &= (bool)connect( this, SIGNAL( connected() ), SLOT( on_connected() ) );
	ok &= (bool)connect( this, SIGNAL( disconnected( rec::rpc::ErrorCode ) ), SLOT( on_disconnected( rec::rpc::ErrorCode ) ) );
	ok &= (bool)connect( this, SIGNAL( stateChanged( QAbstractSocket::SocketState ) ), SLOT( on_stateChanged( QAbstractSocket::SocketState ) ) );
	ok &= (bool)connect( this, SIGNAL( error( QAbstractSocket::SocketError, const QString& ) ), SLOT( on_error( QAbstractSocket::SocketError, const QString& ) ) );

	Q_ASSERT( ok );

	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_image0 );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_camera0_control );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_camera0_settings );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_camera0_settings );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_camera0_capabilities );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_camera0_calibration );

	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_image1 );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_camera1_control );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_camera1_settings );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_camera1_settings );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_camera1_capabilities );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_camera1_calibration );

	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_image2 );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_camera2_control );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_camera2_settings );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_camera2_settings );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_camera2_capabilities );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_camera2_calibration );

	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_image3 );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_camera3_control );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_camera3_settings );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_camera3_settings );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_camera3_capabilities );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_camera3_calibration );

	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_motor0_setpoint );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_motor0_reset_position );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_motor0_mode );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_motor0_mode );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_motor1_setpoint );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_motor1_reset_position );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_motor1_mode );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_motor1_mode );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_motor2_setpoint );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_motor2_reset_position );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_motor2_mode );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_motor2_mode );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_motor3_setpoint );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_motor3_reset_position );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_motor3_mode );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_motor3_mode );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_motor_readings );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_motor_setpoints );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_omnidrive );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_odometry );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_odometry );
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_pose);
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_northstar );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_northstar_parameters );
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_gyroscope);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_gyroscope_ext);
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_emergency_bumper );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_emergency_bumper );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_display_text );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_display_backlight );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_display_buttons );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_display_vbar );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_display_hbar );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_display_progress );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_display_clear );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_parameters );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_ea_version );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_ea09_ip4address );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_ea09_ip4address );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_clients_connected );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_digital_input );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_analog_input );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_distance_sensors );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_bumper );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_battery );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_digital_output );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_digital_output_array );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_relay );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_relay_array );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_shutdown );
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_start_hotswap);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_start_motor_debug);
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_power_button );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_request_shutdown );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_pid_parameters );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_gripper_state );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_scan0 );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_scan1 );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_scan2 );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_scan3 );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_cbha_readings );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_cbha_set_pressure );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_cbha_set_compressors_enabled );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_cbha_set_water_drain_valve );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_cbha_set_gripper_valve1 );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_cbha_set_gripper_valve2 );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_grappler_store_positions );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_grappler_set_power );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_grappler_set_positions );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_grappler_servos );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_grappler_readings );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_grappler_toggle_torque );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_log );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_set_log_level );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_custom_message );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_custom_message0 );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_custom_message1 );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_custom_message2 );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_custom_message3 );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect0_set_tilt );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect1_set_tilt );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect2_set_tilt );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect3_set_tilt );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect0_tilt );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect1_tilt );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect2_tilt );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect3_tilt );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect0_accel );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect1_accel );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect2_accel );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect3_accel );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect0_set_led );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect1_set_led );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect2_set_led );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect3_set_led );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect0_led );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect1_led );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect2_led );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect3_led );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect0_set_video_format );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect1_set_video_format );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect2_set_video_format );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect3_set_video_format );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect0_video_format );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect1_video_format );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect2_video_format );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect3_video_format );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect0_set_depth_format );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect1_set_depth_format );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect2_set_depth_format );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect3_set_depth_format );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect0_depth_format );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect1_depth_format );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect2_depth_format );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect3_depth_format );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect0_depth );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect1_depth );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect2_depth );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect3_depth );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect0_video );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect1_video );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect2_video );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_kinect3_video );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_process_status );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_process_output );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_disconnect_api1_clients );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_api1_setstate_received );

	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_poseOnMap );

	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_initialPose );

	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_navGoal );

	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_charger0_info );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_charger1_info );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_charger2_info );

	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_charger0_version );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_charger1_version );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_charger2_version );

	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_charger0_error );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_charger1_error );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_charger2_error );

	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_charger0_clear_error );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_charger1_clear_error );
	MAP_TOPICLISTENER_ENABLED( rec_robotino_rpc_charger2_clear_error );

	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_sensors);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_motor_debug);

	/*Faxcctory 4*/
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_fleetcom_request);

	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_mclayout);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_mcstatus);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_pathnetwork);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_pathnetwork_edited);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_localizationMode);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_smartlog);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_smartnavigationplan);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_smartlocations);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_smartlocations_edited);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_smartstations);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_smartstations_edited);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_smartrobotinfo);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_smartmyrobotid);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_smartjoblist);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_smartlicense);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_smartdockingvis);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_festool_charger_info);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_mapDir);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_fleetcom_response);

	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_mapDir_request);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_mapDir_response);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_upload_program);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_map);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_mapPlanner);
	MAP_TOPICLISTENER_ENABLED(rec_robotino_rpc_mapPlannerEdited);
	
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_0);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_1);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_2);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_3);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_4);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_5);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_6);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_7);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_8);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_9);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_10);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_11);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_12);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_13);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_14);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_15);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_16);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_17);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_18);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_19);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_20);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_21);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_22);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_23);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_24);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_25);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_26);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_27);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_28);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_29);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_30);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_31);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_32);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_33);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_34);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_35);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_36);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_37);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_38);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_39);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_40);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_41);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_42);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_43);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_44);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_45);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_46);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_47);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_48);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_49);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_50);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_51);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_52);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_53);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_54);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_55);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_56);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_57);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_58);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_59);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_60);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_61);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_62);
	MAP_JSON_MESSAGE_WITH_DATA_TOPICS_ENABLED(rec_robotino_rpc_json_message_with_data_topic_63);
}

Client::~Client()
{
}

void Client::setAddress( const QString& address )
{
	QString str = address;

#if QT_VERSION>=0x040600 && not defined __APPLE__
	if( str == "$RPCD_SERVER_ADDRESS" )
	{
		str = QProcessEnvironment::systemEnvironment().value( "RPCD_SERVER_ADDRESS", "127.0.0.1" );
	}
#endif

	if( false == str.contains( ':' ) )
	{
		str.append( ":12080" );
	}

	rec::rpc::Client::setAddress( str );
}

QString Client::address() const
{
	QString str = rec::rpc::Client::address();
	str.remove( ":12080" );
	return str;
}

void Client::readParameters( const QString& path, const QString& userpath )
{
	_parameters_path = path;
	_user_parameters_path = userpath;

	QSettings param( _parameters_path, QSettings::IniFormat );
	switch( param.status() )
	{
	case QSettings::AccessError:
		Q_EMIT log( QString( "AccessError %1" ).arg( path ), 1 );
		break;

	case QSettings::FormatError:
		Q_EMIT log( QString( "FormatError %1" ).arg( path ), 1 );
		break;

	default:
		Q_EMIT log( QString( "Success reading %1" ).arg( path ), 1 );
	}

	QSettings userparam( _user_parameters_path, QSettings::IniFormat );
	switch( userparam.status() )
	{
	case QSettings::AccessError:
		Q_EMIT log( QString( "AccessError %1" ).arg( userpath ), 1 );
		break;

	case QSettings::FormatError:
		Q_EMIT log( QString( "FormatError %1" ).arg( userpath ), 1 );
		break;

	default:
		Q_EMIT log( QString( "Success reading %1" ).arg( userpath ), 1 );
	}

	QString str;
	QTextStream stream( &str );
	stream << '\n';

	QSet< QString > keys;
	Q_FOREACH( const QString& key, param.allKeys() )
	{
		_parameters[ key ] = param.value( key );
		keys << key;
	}
	Q_FOREACH( const QString& key, userparam.allKeys() )
	{
		_user_parameters[ key ] = userparam.value( key );
		keys << key;
	}

	Q_FOREACH( const QString& key, keys )
	{
		QVariant v;
		if( _user_parameters.contains( key ) )
		{
			v = _user_parameters.value( key );
			stream << "*" << key << " : " << v.toString() << '\n';
		}
		else
		{
			v = _parameters.value( key );
			stream << key << " : " << v.toString() << '\n';
		}
	}

	Q_EMIT log( str, 1 );
}

bool Client::setParameters( const QMap< QString, QVariant >& values )
{
	try
	{
		PREPARE( rec_robotino_rpc_set_parameters );
		param.ref() = values;
		INVOKE( true );
		return result;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( e.what(), 1 );
		return false;
	}
}

void Client::saveUserParameters()
{
	QSettings userparam( _user_parameters_path, QSettings::IniFormat );
	QMap< QString, QVariant >::const_iterator iter = _user_parameters.constBegin();
	while( _user_parameters.constEnd() != iter )
	{
		userparam.setValue( iter.key(), iter.value() );
		++iter;
	}
	userparam.sync();
}

bool Client::setParameter( const QString& key, const QVariant& value )
{
	QString tmp = key;

	if( key.startsWith( "~/" ) )
	{
		tmp.remove( "~/" );

		_user_parameters[ tmp ] =  value;
		saveUserParameters();

		tmp = name() + "/" + tmp;
	}

	try
	{
		PREPARE( rec_robotino_rpc_set_parameter );
		param.key() = tmp;
		param.value() = value;
		INVOKE( true );
		return result;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( e.what(), 1 );
		return false;
	}
}
QVariant Client::getLocalParameter( const QString& key, const QVariant& defaultValue )
{
	QString tmp = key;
	if( key.startsWith( "~/" ) )
	{
		tmp.remove( "~/" );
	}

	if( _user_parameters.contains( tmp ) )
	{
		return _user_parameters.value( tmp );
	}
		
	return _parameters.value( tmp, defaultValue );
}

QVariant Client::getParameter( const QString& key, const QVariant& defaultValue )
{
	if( key.startsWith( "~/" ) )
	{
		return getLocalParameter( key, defaultValue );
	}
	else
	{
		try
		{
			PREPARE( rec_robotino_rpc_get_parameter );
			param.ref() = key;
			INVOKE( true );
			if( result.value().isNull() )
			{
				return defaultValue;
			}
			else
			{
				return result.value();
			}
		}
		catch( const rec::rpc::Exception& e )
		{
			Q_EMIT log( QString( "Error getParameter %1: %2" ).arg( key ).arg( e.what() ), 1 );
			return defaultValue;
		}
	}
}

QMap< QString, QVariant > Client::getLocalParameters() const
{
	QMap< QString, QVariant > m = _parameters;
	QMap< QString, QVariant >::const_iterator iter = _user_parameters.constBegin();
	while( _user_parameters.constEnd() != iter )
	{
		m[ iter.key() ] = iter.value();
		++iter;
	}
	return m;
}

QMap< QString, QVariant > Client::getParameters()
{
	try
	{
		INVOKE_SIMPLE_EMPTY( rec_robotino_rpc_get_parameters, true );
		return result;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( e.what(), 1 );
		return QMap< QString, QVariant >();
	}
}

bool Client::containsParameter( const QString& key )
{
	if( key.startsWith( "~/" ) )
	{
		QString tmp = key;
		tmp.remove( "~/" );

		return _user_parameters.contains( tmp ) || _parameters.contains( tmp );
	}
	else
	{
		try
		{
			PREPARE( rec_robotino_rpc_contains_parameter );
			param.ref() = key;
			INVOKE( true );
			return result;
		}
		catch( const rec::rpc::Exception& e )
		{
			Q_EMIT log( e.what(), 1 );
			return false;
		}
	}
}

void Client::on_connected()
{
	Q_EMIT log( "Connected", 1 );
	Q_EMIT is_connected();

	QSet< QString > keys;
	Q_FOREACH( const QString& key, _parameters.keys() )
	{
		keys << key;
	}
	Q_FOREACH( const QString& key, _user_parameters.keys() )
	{
		keys << key;
	}

	QMap< QString, QVariant > pm;
	Q_FOREACH( const QString& key, keys )
	{
		QVariant v;
		if( _user_parameters.contains( key ) )
		{
			v = _user_parameters.value( key );
		}
		else
		{
			v = _parameters.value( key );
		}

		pm[ name() + "/" + key ] = v;
	}

	setParameters( pm );
}

void Client::on_disconnected( rec::rpc::ErrorCode error )
{
	Q_EMIT log( "Disconnected", 1 );
	Q_EMIT is_disconnected();
}

void Client::on_stateChanged( QAbstractSocket::SocketState state )
{
	Q_EMIT log( QString("State changed to %1").arg( state ), 1 );
}

void Client::on_error( QAbstractSocket::SocketError socketError, const QString& errorString )
{
	Q_EMIT log( errorString, 1 );
}

bool Client::set_image( unsigned int cameraNumber, const QByteArray& imageData, unsigned int width, unsigned int height, unsigned int step, const QString& format )
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

bool Client::set_image0( const QByteArray& imageData, unsigned int width, unsigned int height, unsigned int step, const QString& format )
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

bool Client::set_image1( const QByteArray& imageData, unsigned int width, unsigned int height, unsigned int step, const QString& format )
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

bool Client::set_image2( const QByteArray& imageData, unsigned int width, unsigned int height, unsigned int step, const QString& format )
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

bool Client::set_image3( const QByteArray& imageData, unsigned int width, unsigned int height, unsigned int step, const QString& format )
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

bool Client::set_camera_settings( unsigned int cameraNumber, unsigned int width, unsigned int height, const QString& format )
{
	switch( cameraNumber )
	{
	case 0:
		return set_camera0_settings( width, height, format );

	case 1:
		return set_camera1_settings( width, height, format );

	case 2:
		return set_camera2_settings( width, height, format );

	case 3:
		return set_camera3_settings( width, height, format );

	default:
		Q_EMIT log( "camera number out of range", 1 );
		return false;
	}
}

bool Client::set_camera0_settings( unsigned int width, unsigned int height, const QString& format )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_camera0_settings );
		data.width() = width;
		data.height() = height;
		data.format() = format;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_camera0_settings: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_camera1_settings( unsigned int width, unsigned int height, const QString& format )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_camera1_settings );
		data.width() = width;
		data.height() = height;
		data.format() = format;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_camera1_settings: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_camera2_settings( unsigned int width, unsigned int height, const QString& format )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_camera2_settings );
		data.width() = width;
		data.height() = height;
		data.format() = format;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_camera2_settings: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_camera3_settings( unsigned int width, unsigned int height, const QString& format )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_camera3_settings );
		data.width() = width;
		data.height() = height;
		data.format() = format;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_camera3_settings: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_set_camera_control( unsigned int cameraNumber, const QString& name, int value )
{
	switch( cameraNumber )
	{
	case 0:
		return set_set_camera0_control( name, value );

	case 1:
		return set_set_camera1_control( name, value );

	case 2:
		return set_set_camera2_control( name, value );

	case 3:
		return set_set_camera3_control( name, value );

	default:
		Q_EMIT log( "camera number out of range", 1 );
		return false;
	}
}

bool Client::set_set_camera0_control( const QString& name, int value )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_set_camera0_control );
		data.name() = name;
		data.value() = value;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_camera0_control: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_set_camera1_control( const QString& name, int value )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_set_camera1_control );
		data.name() = name;
		data.value() = value;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_camera1_control: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_set_camera2_control( const QString& name, int value )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_set_camera2_control );
		data.name() = name;
		data.value() = value;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_camera2_control: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_set_camera3_control( const QString& name, int value )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_set_camera3_control );
		data.name() = name;
		data.value() = value;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_camera3_control: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_set_camera_settings( unsigned int cameraNumber, unsigned int width, unsigned int height, const QString& format )
{
	switch( cameraNumber )
	{
	case 0:
		return set_set_camera0_settings( width, height, format );

	case 1:
		return set_set_camera1_settings( width, height, format );

	case 2:
		return set_set_camera2_settings( width, height, format );

	case 3:
		return set_set_camera3_settings( width, height, format );

	default:
		Q_EMIT log( "camera number out of range", 1 );
		return false;
	}
}

bool Client::set_set_camera0_settings( unsigned int width, unsigned int height, const QString& format )
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
		Q_EMIT log( QString( "Error set_set_camera0_settings: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_set_camera1_settings( unsigned int width, unsigned int height, const QString& format )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_set_camera1_settings );
		data.width() = width;
		data.height() = height;
		data.format() = format;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_set_camera1_settings: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_set_camera2_settings( unsigned int width, unsigned int height, const QString& format )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_set_camera2_settings );
		data.width() = width;
		data.height() = height;
		data.format() = format;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_set_camera2_settings: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_set_camera3_settings( unsigned int width, unsigned int height, const QString& format )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_set_camera3_settings );
		data.width() = width;
		data.height() = height;
		data.format() = format;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_set_camera3_settings: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_camera_capabilities( unsigned int cameraNumber, const QString& cameraName, const QMap<QString, QVector<QSize> >& resolutions, const QStringList& controls )
{
	switch( cameraNumber )
	{
	case 0:
		return set_camera0_capabilities( cameraName, resolutions, controls );

	case 1:
		return set_camera1_capabilities( cameraName, resolutions, controls );

	case 2:
		return set_camera2_capabilities( cameraName, resolutions, controls );

	case 3:
		return set_camera3_capabilities( cameraName, resolutions, controls );

	default:
		Q_EMIT log( "camera number out of range", 1 );
		return false;
	}
}

bool Client::set_camera0_capabilities( const QString& cameraName, const QMap<QString, QVector<QSize> >& resolutions, const QStringList& controls )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_camera0_capabilities );
		data.cameraName() = cameraName;
		data.resolutions() = resolutions;
		data.controls() = controls;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_camera0_capabilities: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_camera1_capabilities( const QString& cameraName, const QMap<QString, QVector<QSize> >& resolutions, const QStringList& controls )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_camera1_capabilities );
		data.cameraName() = cameraName;
		data.resolutions() = resolutions;
		data.controls() = controls;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_camera1_capabilities: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_camera2_capabilities( const QString& cameraName, const QMap<QString, QVector<QSize> >& resolutions, const QStringList& controls )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_camera2_capabilities );
		data.cameraName() = cameraName;
		data.resolutions() = resolutions;
		data.controls() = controls;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_camera2_capabilities: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_camera3_capabilities( const QString& cameraName, const QMap<QString, QVector<QSize> >& resolutions, const QStringList& controls )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_camera3_capabilities );
		data.cameraName() = cameraName;
		data.resolutions() = resolutions;
		data.controls() = controls;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_camera3_capabilities: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_camera_calibration( unsigned int cameraNumber, const QVector<double>& calibration )
{
	switch( cameraNumber )
	{
	case 0:
		return set_camera0_calibration( calibration );

	case 1:
		return set_camera1_calibration( calibration );

	case 2:
		return set_camera2_calibration( calibration );

	case 3:
		return set_camera3_calibration( calibration );

	default:
		Q_EMIT log( "camera number out of range", 1 );
		return false;
	}
}

bool Client::set_camera0_calibration( const QVector<double>& calibration )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_camera0_calibration, calibration );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_camera0_calibration: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_camera1_calibration( const QVector<double>& calibration )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_camera1_calibration, calibration );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_camera1_calibration: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_camera2_calibration( const QVector<double>& calibration )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_camera2_calibration, calibration );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_camera2_calibration: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_camera3_calibration( const QVector<double>& calibration )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_camera3_calibration, calibration );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_camera3_calibration: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_motor0_setpoint( float speed )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_motor0_setpoint, speed );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_motor0_setpoint: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_motor0_reset_position( int position )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_motor0_reset_position, position );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_motor0_reset_position: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_set_motor0_mode( const QString& mode )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_set_motor0_mode, mode );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_set_motor0_mode: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_motor1_setpoint( float speed )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_motor1_setpoint, speed );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_motor1_setpoint: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_motor1_reset_position( int position )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_motor1_reset_position, position );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_motor1_reset_position: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_set_motor1_mode( const QString& mode )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_set_motor1_mode, mode );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_set_motor1_mode: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_motor2_setpoint( float speed )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_motor2_setpoint, speed );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_motor2_setpoint: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_motor2_reset_position( int position )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_motor2_reset_position, position );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_motor2_reset_position: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_set_motor2_mode( const QString& mode )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_set_motor2_mode, mode );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_set_motor0_mode: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_motor3_setpoint( float speed )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_motor3_setpoint, speed );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_motor3_setpoint: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_motor3_reset_position( int position )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_motor3_reset_position, position );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_motor3_reset_position: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_set_motor3_mode( const QString& mode )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_set_motor3_mode, mode );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_set_motor3_mode: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_motor_setpoints( float m0, float m1, float m2 )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_motor_setpoints );
		data.m0() = m0;
		data.m1() = m1;
		data.m2() = m2;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_motor_setpoints: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_motor_readings( const QVector< float >& speeds, const QVector< int >& positions, const QVector< float >& currents, float time_delta )
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

bool Client::set_omnidrive( float vx, float vy, float omega )
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
		Q_EMIT log( QString( "Error set_omnidrive: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_odometry( double x, double y, double phi, float vx, float vy, float omega, unsigned int sequence )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_odometry );
		data.x() = x;
		data.y() = y;
		data.phi() = phi;
		data.vx() = vx;
		data.vy() = vy;
		data.omega() = omega;
		data.seq() = sequence;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_odometry: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_set_odometry( double x, double y, double phi )
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
		Q_EMIT log( QString( "Error set_set_odometry: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_pose(double x, double y, double phi, double errx, double erry, double errphi, unsigned int sequence)
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

bool Client::set_northstar( unsigned int sequenceNumber,
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

bool Client::set_set_northstar_parameters( unsigned int roomId, float ceilingCal )
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

bool Client::set_gyroscope( double phi, double rate )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_gyroscope );
		data.angle() = phi;
		data.rate() = rate;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_gyroscope: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_gyroscope_ext(double phi, double rate)
{
	try
	{
		PREPARE_TOPIC(rec_robotino_rpc_gyroscope_ext);
		data.angle() = phi;
		data.rate() = rate;
		PUBLISH_TOPIC;
	}
	catch (const rec::rpc::Exception& e)
	{
		Q_EMIT log(QString("Error set_gyroscope_ext: ") + e.what(), 1);
		return false;
	}
	return true;
}

bool Client::set_emergency_bumper( bool enable )
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_emergency_bumper, enable );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_emergency_bumper: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_set_emergency_bumper( bool enable )
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_set_emergency_bumper, enable );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_set_emergency_bumper: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_display_text( const QString& text, unsigned int row, unsigned int col, bool clear_before, bool clear_after )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_display_text );
		data.text() = text;
		data.row() = row;
		data.col() = col;
		data.clear_before() = clear_before;
		data.clear_after() = clear_after;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_display_text: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_display_backlight( bool on )
{
	try
	{
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_display_backlight, on );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_display_backlight: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_display_buttons( bool up, bool down, bool back, bool enter )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_display_buttons );
		data.up() = up;
		data.down() = down;
		data.back() = back;
		data.enter() = enter;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_display_buttons: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_display_vbar( float value, unsigned int col, unsigned int start_row, unsigned int end_row )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_display_vbar );
		data.value() = value;
		data.col() = col;
		data.start_row() = start_row;
		data.end_row() = end_row;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_display_vbar: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_display_hbar( float value, unsigned int row, unsigned int start_col, unsigned int end_col )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_display_hbar );
		data.value() = value;
		data.row() = row;
		data.start_col() = start_col;
		data.end_col() = end_col;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_display_hbar: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_display_progress( unsigned int step, unsigned int row )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_display_progress );
		data.step() = step;
		data.row() = row;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_display_progress: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::disconnectClient( const QHostAddress& address, quint16 port )
{
	try
	{		
		PREPARE( rec_robotino_rpc_disconnect_client );
		param.address() = address;
		param.port() = port;
		INVOKE( true );
		return result;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error disconnectClient: " ) + e.what(), 1 );
		return false;
	}
}

int Client::launchProcess( const QString& command, const QStringList& parameters, const QString& workingdirectory )
{
	try
	{		
		PREPARE( rec_robotino_rpc_process_launch );
		param.command() = command;
		param.parameters() = parameters;
		param.workingdirectory() = workingdirectory;
		INVOKE( true );
		return result;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error launchProcess: " ) + e.what(), 1 );
		return -1;
	}
}

int Client::terminateProcess( int pid )
{
	try
	{		
		PREPARE( rec_robotino_rpc_process_terminate );
		param.ref() = pid;
		INVOKE( true );
		return result;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error terminateProcess: " ) + e.what(), 1 );
		return -1;
	}
}

int Client::killProcess( int pid )
{
	try
	{		
		PREPARE( rec_robotino_rpc_process_kill );
		param.ref() = pid;
		INVOKE( true );
		return result;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error killProcess: " ) + e.what(), 1 );
		return -1;
	}
}

QVector< int > Client::getProcessIds()
{
	try
	{		
		PREPARE( rec_robotino_rpc_process_getids );
		INVOKE( true );
		return result;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error getProcessIds: " ) + e.what(), 1 );
		return QVector< int >();
	}
}

rec::robotino::rpc::ProcessStatus Client::getProcessStatus( int pid )
{
	try
	{		
		PREPARE( rec_robotino_rpc_process_getstatus );
		param.ref() = pid;
		INVOKE( true );
		return result;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error getProcessStatus: " ) + e.what(), 1 );
		return rec::robotino::rpc::ProcessStatus();
	}
}

bool Client::setTopicEnabled( const QString& topicName, bool enable )
{
	QString fullTopicName = "rec_robotino_rpc_" + topicName;
	
	EnableFunc funcPtr = _enableMap.value(fullTopicName);
	
	if(NULL == funcPtr)
	{
		QMap<QString,int>::const_iterator iter = _json_message_with_data_topics_alias_reverse_map.find(topicName);
		if(_json_message_with_data_topics_alias_reverse_map.constEnd() != iter )
		{
			int topicNumber = iter.value();
			fullTopicName = QString("rec_robotino_rpc_json_message_with_data_topic_%1").arg(topicNumber);
			funcPtr = _enableMap.value(fullTopicName);
		}
	}
	
	if(NULL == funcPtr)
	{
		Q_EMIT log( QString( "Unknown topic %1" ).arg( topicName ), 1 );
		return false;
	}

	try
	{
		((*this).*funcPtr)( enable );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( "Client error: " + e.getMessage(), 1 );
		return false;
	}

	return true;
}

bool Client::set_json_message_with_data_topic_Enabled(int topic, bool enable)
{
	try
	{
		if (_json_message_with_data_topics_enableMap.contains(topic))
		{
			EnableFunc funcPtr = _json_message_with_data_topics_enableMap[topic];
			((*this).*funcPtr)(enable);
		}
		else
		{
			Q_EMIT log(QString("Unknown json_message_with_data_topic %1").arg(topic), 1);
			return false;
		}
	}
	catch (const rec::rpc::Exception& e)
	{
		Q_EMIT log("Client error: " + e.getMessage(), 1);
		return false;
	}

	return true;
}

bool Client::set_api1_setstate_received()
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE_EMPTY( rec_robotino_rpc_api1_setstate_received );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_api1_setstate_received: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_disconnect_api1_clients()
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE_EMPTY( rec_robotino_rpc_disconnect_api1_clients );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_disconnect_api1_clients: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_display_clear()
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE_EMPTY( rec_robotino_rpc_display_clear );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_display_clear: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_ea09_ip4address( const QHostAddress& address, const QHostAddress& netmask )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_ea09_ip4address );
		data.address() = address;
		data.netmask() = netmask;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_ea09_ip4address: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_set_ea09_ip4address( const QHostAddress& address, const QHostAddress& netmask )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_set_ea09_ip4address );
		data.address() = address;
		data.netmask() = netmask;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_set_ea09_ip4address: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_ea_version( const QString& board, quint16 firmware_major, quint16 firmware_minor, quint16 firmware_patch )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_ea_version );
		data.board() = board;
		data.firmware_major() = firmware_major;
		data.firmware_minor() = firmware_minor;
		data.firmware_patch() = firmware_patch;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_ea_version: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_digital_input( const QVector< bool >& in )
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

bool Client::set_analog_input( const QVector< float >& in )
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

bool Client::set_distance_sensors( const QVector< float >& in )
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

bool Client::set_battery( float battery_voltage, float system_current, bool ext_power, int num_chargers, const QString& batteryType, bool batteryLow, int batteryLowShutdownCounter )
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

bool Client::set_bumper( bool bump )
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

bool Client::set_set_digital_output( unsigned int index, bool state )
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

bool Client::set_set_digital_output_array( const QVector< bool >& vec )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_set_digital_output_array );
		data.ref() = vec;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_set_digital_output_array: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_set_relay( unsigned int index, bool state )
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

bool Client::set_set_relay_array( const QVector< bool >& vec )
{
	try
	{
		PREPARE_TOPIC( rec_robotino_rpc_set_relay_array );
		data.ref() = vec;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_set_relay_array: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_set_shutdown( bool shutdown )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_set_shutdown, shutdown );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_set_shutdown: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::start_hotswap()
{
	try
	{
		PUBLISH_TOPIC_SIMPLE_EMPTY(rec_robotino_rpc_start_hotswap);
	}
	catch (const rec::rpc::Exception& e)
	{
		Q_EMIT log(QString("Error start_hotswap: ") + e.what(), 1);
		return false;
	}
	return true;
}


bool Client::start_motor_debug(int motor)
{
	try
	{
		PUBLISH_TOPIC_SIMPLE(rec_robotino_rpc_start_motor_debug, motor);
	}
	catch (const rec::rpc::Exception& e)
	{
		Q_EMIT log(QString("Error start_motor_debug: ") + e.what(), 1);
		return false;
	}
	return true;
}


bool Client::set_power_button( bool pressed )
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_power_button, pressed );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_power_button: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_request_shutdown( bool doit )
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

bool Client::set_set_pid_parameters( unsigned int motor, float kp, float ki, float kd )
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

bool Client::set_gripper_state( int state )
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

bool Client::set_scan( unsigned int laserNumber, QVector< float > ranges, QVector< float > intensities, QVariantMap parameters )
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

bool Client::set_scan0( QVector< float > ranges, QVector< float > intensities, QVariantMap parameters )
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

bool Client::set_scan1( QVector< float > ranges, QVector< float > intensities, QVariantMap parameters )
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

bool Client::set_scan2( QVector< float > ranges, QVector< float > intensities, QVariantMap parameters )
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

bool Client::set_scan3( QVector< float > ranges, QVector< float > intensities, QVariantMap parameters )
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

bool Client::set_cbha_readings( const QVector<float>& pressures, bool pressureSensor, const QVector<float>& stringPots, float foilPot )
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

bool Client::set_cbha_set_pressure( const QVector<float>& pressures )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_cbha_set_pressure, pressures );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_cbha_set_pressure: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_cbha_set_compressors_enabled( bool enabled )
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_cbha_set_compressors_enabled, enabled );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_cbha_set_compressors_enabled: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_cbha_set_water_drain_valve( bool enabled )
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_cbha_set_water_drain_valve, enabled );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_cbha_set_water_drain_valve: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_cbha_set_gripper_valve1( bool enabled )
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_cbha_set_gripper_valve1, enabled );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_cbha_set_gripper_valve1: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_cbha_set_gripper_valve2( bool enabled )
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_cbha_set_gripper_valve2, enabled );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_cbha_set_gripper_valve2: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_grappler_store_positions( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data )
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_grappler_store_positions, data );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_grappler_store_positions: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_grappler_set_power( int line, bool power )
{
	try
	{	
		PREPARE_TOPIC( rec_robotino_rpc_grappler_set_power );
		data.line() = line;
		data.power() = power;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_grappler_set_power: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_grappler_set_positions( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data )
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_grappler_set_positions, data );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_grappler_set_positions: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_grappler_servos( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data )
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

bool Client::set_grappler_readings( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data )
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

bool Client::set_grappler_toggle_torque()
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE_EMPTY( rec_robotino_rpc_grappler_toggle_torque );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_grappler_toggle_torque: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_log( const QString& message, int level )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_log );
		data.publisher() = QCoreApplication::applicationName();
		data.message() = message;
		data.level() = level;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& )
	{
		//we do not want to emit the log signal here, because it might be attached to set_log which would lead to a loop
		//Q_EMIT log( QString( "Error set_log: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_set_log_level( const QString& publisher, int level )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_set_log_level );
		data.publisher() = publisher;
		data.level() = level;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_set_log_level: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_custom_message( unsigned int id, const QByteArray& messageData )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_custom_message );
		data.id() = id;
		data.data() = messageData;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_custom_message: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_custom_message0( const QByteArray& messageData )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_custom_message0, messageData );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_custom_message0: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_custom_message1( const QByteArray& messageData )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_custom_message1, messageData );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_custom_message1: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_custom_message2( const QByteArray& messageData )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_custom_message2, messageData );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_custom_message2: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_custom_message3( const QByteArray& messageData )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_custom_message3, messageData );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_custom_message3: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect_set_tilt( unsigned int id, double tilt )
{
	switch( id )
	{
	case 0:
		return set_kinect0_set_tilt( tilt );

	case 1:
		return set_kinect1_set_tilt( tilt );

	case 2:
		return set_kinect2_set_tilt( tilt );

	case 3:
		return set_kinect3_set_tilt( tilt );
	}

	return false;
}

bool Client::set_kinect0_set_tilt( double tilt )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_kinect0_set_tilt, tilt );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect0_set_tilt: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect1_set_tilt( double tilt )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_kinect1_set_tilt, tilt );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect1_set_tilt: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect2_set_tilt( double tilt )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_kinect2_set_tilt, tilt );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect2_set_tilt: " ) + e.what(), 1 );
		return false;
	}
	return true;
}
bool Client::set_kinect3_set_tilt( double tilt )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_kinect3_set_tilt, tilt );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect3_set_tilt: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect_tilt( unsigned int id, double tilt )
{
	switch( id )
	{
	case 0:
		return set_kinect0_tilt( tilt );

	case 1:
		return set_kinect1_tilt( tilt );

	case 2:
		return set_kinect2_tilt( tilt );

	case 3:
		return set_kinect3_tilt( tilt );
	}

	return false;
}

bool Client::set_kinect0_tilt( double tilt )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_kinect0_tilt, tilt );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect0_tilt: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect1_tilt( double tilt )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_kinect1_tilt, tilt );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect1_tilt: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect2_tilt( double tilt )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_kinect2_tilt, tilt );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect2_tilt: " ) + e.what(), 1 );
		return false;
	}
	return true;
}
bool Client::set_kinect3_tilt( double tilt )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_kinect3_tilt, tilt );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect3_tilt: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect_accel( unsigned int id, double x, double y, double z )
{
	switch( id )
	{
	case 0:
		return set_kinect0_accel( x, y, z );

	case 1:
		return set_kinect1_accel( x, y, z );

	case 2:
		return set_kinect2_accel( x, y, z );

	case 3:
		return set_kinect3_accel( x, y, z );
	}

	return false;
}

bool Client::set_kinect0_accel( double x, double y, double z )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_kinect0_accel );
		data.x() = x;
		data.y() = y;
		data.z() = z;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect0_accel: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect1_accel( double x, double y, double z )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_kinect1_accel );
		data.x() = x;
		data.y() = y;
		data.z() = z;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect1_accel: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect2_accel( double x, double y, double z )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_kinect2_accel );
		data.x() = x;
		data.y() = y;
		data.z() = z;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect2_accel: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect3_accel( double x, double y, double z )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_kinect3_accel );
		data.x() = x;
		data.y() = y;
		data.z() = z;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect3_accel: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect_set_led( unsigned int id, unsigned int state )
{
	switch( id )
	{
	case 0:
		return set_kinect0_set_led( state );

	case 1:
		return set_kinect1_set_led( state );

	case 2:
		return set_kinect2_set_led( state );

	case 3:
		return set_kinect3_set_led( state );
	}

	return false;
}

bool Client::set_kinect0_set_led( unsigned int state )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_kinect0_set_led, state );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect0_set_led: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect1_set_led( unsigned int state )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_kinect1_set_led, state );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect1_set_led: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect2_set_led( unsigned int state )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_kinect2_set_led, state );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect2_set_led: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect3_set_led( unsigned int state )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_kinect3_set_led, state );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect3_set_led: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect_set_video_format( unsigned int id, unsigned int video_format )
{
	switch( id )
	{
	case 0:
		return set_kinect0_set_video_format( video_format );

	case 1:
		return set_kinect1_set_video_format( video_format );

	case 2:
		return set_kinect2_set_video_format( video_format );

	case 3:
		return set_kinect3_set_video_format( video_format );
	}

	return false;
}

bool Client::set_kinect0_set_video_format( unsigned int video_format )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_kinect0_set_video_format, video_format );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect0_set_video_format: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect1_set_video_format( unsigned int video_format )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_kinect1_set_video_format, video_format );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect1_set_video_format: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect2_set_video_format( unsigned int video_format )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_kinect2_set_video_format, video_format );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect2_set_video_format: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect3_set_video_format( unsigned int video_format )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_kinect3_set_video_format, video_format );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect3_set_video_format: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect_set_depth_format( unsigned int id, unsigned int depth_format )
{
	switch( id )
	{
	case 0:
		return set_kinect0_set_depth_format( depth_format );

	case 1:
		return set_kinect1_set_depth_format( depth_format );

	case 2:
		return set_kinect2_set_depth_format( depth_format );

	case 3:
		return set_kinect3_set_depth_format( depth_format );
	}

	return false;
}

bool Client::set_kinect0_set_depth_format( unsigned int depth_format )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_kinect0_set_depth_format, depth_format );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect0_set_depth_format: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect1_set_depth_format( unsigned int depth_format )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_kinect1_set_depth_format, depth_format );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect1_set_depth_format: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect2_set_depth_format( unsigned int depth_format )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_kinect2_set_depth_format, depth_format );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect2_set_depth_format: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect3_set_depth_format( unsigned int depth_format )
{
	try
	{	
		PUBLISH_TOPIC_SIMPLE( rec_robotino_rpc_kinect3_set_depth_format, depth_format );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect3_set_depth_format: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect_depth( unsigned int id, const QByteArray& data, const QByteArray& object_data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
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

bool Client::set_kinect0_depth( const QByteArray& depthData, const QByteArray& object_data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
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

bool Client::set_kinect1_depth( const QByteArray& depthData, const QByteArray& object_data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
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

bool Client::set_kinect2_depth( const QByteArray& depthData, const QByteArray& object_data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
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
				
bool Client::set_kinect3_depth( const QByteArray& depthData, const QByteArray& object_data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
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

bool Client::set_kinect_video( unsigned int id, const QByteArray& data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	switch( id )
	{
	case 0:
		return set_kinect0_video( data, width, height, format, stamp );

	case 1:
		return set_kinect1_video( data, width, height, format, stamp );

	case 2:
		return set_kinect2_video( data, width, height, format, stamp );

	case 3:
		return set_kinect3_video( data, width, height, format, stamp );
	}

	return false;
}

bool Client::set_kinect0_video( const QByteArray& videoData, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_kinect0_video );
		data.data() = videoData;
		data.width() = width;
		data.height() = height;
		data.format() = format;
		data.stamp() = stamp;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect0_video: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect1_video( const QByteArray& videoData, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_kinect1_video );
		data.data() = videoData;
		data.width() = width;
		data.height() = height;
		data.format() = format;
		data.stamp() = stamp;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect1_video: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_kinect2_video( const QByteArray& videoData, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_kinect2_video );
		data.data() = videoData;
		data.width() = width;
		data.height() = height;
		data.format() = format;
		data.stamp() = stamp;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect2_video: " ) + e.what(), 1 );
		return false;
	}
	return true;
}
				
bool Client::set_kinect3_video( const QByteArray& videoData, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_kinect3_video );
		data.data() = videoData;
		data.width() = width;
		data.height() = height;
		data.format() = format;
		data.stamp() = stamp;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_kinect3_video: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool  Client::set_poseOnMap( const QPointF& location,double rotation_deg)
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_poseOnMap );
		data.location() = location;
		data.rotation_deg() = rotation_deg;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_poseOnMap: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool  Client::set_initialPose( const QPointF& location,double rotation_deg)
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_initialPose );
		data.location() = location;
		data.rotation_deg() = rotation_deg;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_initialPose: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_navGoal( const QPointF& location,double rotation_deg)
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_navGoal );
		data.location() = location;
		data.rotation_deg() = rotation_deg;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_navGoal: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_charger0_clear_error()
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE_EMPTY( rec_robotino_rpc_charger0_clear_error );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_charger0_clear_error: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_charger1_clear_error()
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE_EMPTY( rec_robotino_rpc_charger1_clear_error );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_charger1_clear_error: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_charger2_clear_error()
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE_EMPTY( rec_robotino_rpc_charger2_clear_error );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_charger2_clear_error: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_charger0_get_version()
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE_EMPTY( rec_robotino_rpc_charger0_get_version );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_charger0_get_version: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_charger1_get_version()
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE_EMPTY( rec_robotino_rpc_charger1_get_version );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_charger1_get_version: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_charger2_get_version()
{
	try
	{		
		PUBLISH_TOPIC_SIMPLE_EMPTY( rec_robotino_rpc_charger2_get_version );
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_charger2_get_version: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_charger0_info( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_charger0_info );
		data.time() = time;
		data.batteryVoltage() = batteryVoltage;
		data.chargingCurrent() = chargingCurrent;
		data.bat1temp() = bat1temp;
		data.bat2temp() = bat2temp;
		data.state_number() = state_number;
		data.state() = state;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_charger0_info: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_charger1_info( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_charger1_info );
		data.time() = time;
		data.batteryVoltage() = batteryVoltage;
		data.chargingCurrent() = chargingCurrent;
		data.bat1temp() = bat1temp;
		data.bat2temp() = bat2temp;
		data.state_number() = state_number;
		data.state() = state;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_charger1_info: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_charger2_info( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_charger2_info );
		data.time() = time;
		data.batteryVoltage() = batteryVoltage;
		data.chargingCurrent() = chargingCurrent;
		data.bat1temp() = bat1temp;
		data.bat2temp() = bat2temp;
		data.state_number() = state_number;
		data.state() = state;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_charger2_info: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_charger0_version( int major, int minor, int patch )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_charger0_version );
		data.major() = major;
		data.minor() = minor;
		data.patch() = patch;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_charger0_version: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_charger1_version( int major, int minor, int patch )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_charger1_version );
		data.major() = major;
		data.minor() = minor;
		data.patch() = patch;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_charger1_version: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_charger2_version( int major, int minor, int patch )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_charger2_version );
		data.major() = major;
		data.minor() = minor;
		data.patch() = patch;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_charger2_version: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_charger0_error( unsigned int time, const QString& message )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_charger0_error );
		data.time() = time;
		data.message() = message;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_charger0_error: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_charger1_error( unsigned int time, const QString& message )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_charger1_error );
		data.time() = time;
		data.message() = message;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_charger1_error: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_charger2_error( unsigned int time, const QString& message )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_charger2_error );
		data.time() = time;
		data.message() = message;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_charger2_error: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_sensors( const QVector< QString >& names, const QVector< float >& values, const QVector< QString >& units )
{
	try
	{		
		PREPARE_TOPIC( rec_robotino_rpc_sensors );
		data.names() = names;
		data.values() = values;
		data.units() = units;
		PUBLISH_TOPIC;
	}
	catch( const rec::rpc::Exception& e )
	{
		Q_EMIT log( QString( "Error set_sensors: " ) + e.what(), 1 );
		return false;
	}
	return true;
}

bool Client::set_motor_debug(int motor, const QVector<unsigned int>& times, const QVector<float>& speed_setpoints, const QVector<float>& speeds, const QVector<float>& currents, const QVector<float>& control_points)
{
	try
	{
		PREPARE_TOPIC(rec_robotino_rpc_motor_debug);
		data.motor() = motor;
		data.times() = times;
		data.speed_setpoints() = speed_setpoints;
		data.speeds() = speeds;
		data.currents() = currents;
		data.control_points() = control_points;
		PUBLISH_TOPIC;
	}
	catch (const rec::rpc::Exception& e)
	{
		Q_EMIT log(QString("Error set_motor_debug: ") + e.what(), 1);
		return false;
	}
	return true;
}

/*Factory 4*******************/
bool Client::set_fleetcom_request(const QString& message)
{
	try
	{
		PREPARE_TOPIC(rec_robotino_rpc_fleetcom_request);
		data.message() = message;
		PUBLISH_TOPIC;
	}
	catch (const rec::rpc::Exception& e)
	{
		Q_EMIT log(QString("Error set_fleetcom_request: ") + e.what(), 1);
		return false;
	}
	return true;
}

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
JSON_TOPIC_FUNCTIONS(smartlicense)
JSON_TOPIC_FUNCTIONS(smartdockingvis)
JSON_TOPIC_FUNCTIONS(festool_charger_info)
JSON_TOPIC_FUNCTIONS(mapDir)
JSON_WITH_DATA_TOPIC_FUNCTIONS(mapDir_request)
JSON_WITH_DATA_TOPIC_FUNCTIONS(mapDir_response)
JSON_WITH_DATA_TOPIC_FUNCTIONS(upload_program)
JSON_TOPIC_FUNCTIONS(fleetcom_response)

JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_0)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_1)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_2)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_3)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_4)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_5)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_6)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_7)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_8)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_9)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_10)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_11)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_12)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_13)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_14)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_15)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_16)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_17)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_18)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_19)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_20)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_21)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_22)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_23)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_24)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_25)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_26)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_27)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_28)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_29)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_30)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_31)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_32)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_33)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_34)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_35)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_36)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_37)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_38)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_39)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_40)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_41)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_42)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_43)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_44)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_45)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_46)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_47)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_48)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_49)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_50)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_51)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_52)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_53)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_54)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_55)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_56)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_57)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_58)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_59)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_60)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_61)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_62)
JSON_WITH_DATA_TOPIC_ENUM_FUNCTIONS(json_message_with_data_topic_63)



bool Client::set_map(const QByteArray& map, int width, int height, float resolution, float offsetx, float offsety)
{
	try
	{
		PREPARE_TOPIC(rec_robotino_rpc_map);
		data.map() = map;
		data.width() = width;
		data.height() = height;
		data.resolution() = resolution;
		data.offsetx() = offsetx;
		data.offsety() = offsety;
		PUBLISH_TOPIC;
	}
	catch (const rec::rpc::Exception& e)
	{
		Q_EMIT log(QString("Error set_map: ") + e.what(), 1);
		return false;
	}
	return true;
}

bool Client::set_mapPlanner(const QByteArray& map, int width, int height, float resolution, float offsetx, float offsety)
{
	try
	{
		PREPARE_TOPIC(rec_robotino_rpc_mapPlanner);
		data.map() = map;
		data.width() = width;
		data.height() = height;
		data.resolution() = resolution;
		data.offsetx() = offsetx;
		data.offsety() = offsety;
		PUBLISH_TOPIC;
	}
	catch (const rec::rpc::Exception& e)
	{
		Q_EMIT log(QString("Error set_mapPlanner: ") + e.what(), 1);
		return false;
	}
	return true;
}

bool Client::set_mapPlannerEdited(const QByteArray& map, int width, int height, float resolution, float offsetx, float offsety)
{
	try
	{
		PREPARE_TOPIC(rec_robotino_rpc_mapPlannerEdited);
		data.map() = map;
		data.width() = width;
		data.height() = height;
		data.resolution() = resolution;
		data.offsetx() = offsetx;
		data.offsety() = offsety;
		PUBLISH_TOPIC;
	}
	catch (const rec::rpc::Exception& e)
	{
		Q_EMIT log(QString("Error set_mapPlannerEdited: ") + e.what(), 1);
		return false;
	}
	return true;
}

bool Client::set_json_message_with_data(int topic, const QByteArray& jsonData, const QByteArray& payloadData)
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

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_camera0_settings )
	Q_EMIT camera0_settings_changed( data.width(), data.height(), data.format() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_camera0_settings )
	Q_EMIT camera0_settings_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_camera0_settings )
	Q_EMIT set_camera0_settings_changed( data.width(), data.height(), data.format() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_camera0_settings )
	Q_EMIT set_camera0_settings_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_camera0_control )
	Q_EMIT set_camera0_control_changed( data.name(), data.value() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_camera0_control )
	Q_EMIT set_camera0_control_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_camera0_capabilities )
	Q_EMIT camera0_capabilities_changed( data.cameraName(), data.resolutions(), data.controls() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_camera0_capabilities )
	Q_EMIT camera0_capabilities_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_camera0_calibration )
	Q_EMIT camera0_calibration_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_camera0_calibration )
	Q_EMIT camera0_calibration_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_image0 )
	Q_EMIT image0_changed( data.data(), data.width(), data.height(), data.step(), data.format() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_image0 )
	Q_EMIT image0_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

//***************************************************************************

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_camera1_settings )
	Q_EMIT camera1_settings_changed( data.width(), data.height(), data.format() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_camera1_settings )
	Q_EMIT camera1_settings_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_camera1_settings )
	Q_EMIT set_camera1_settings_changed( data.width(), data.height(), data.format() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_camera1_settings )
	Q_EMIT set_camera1_settings_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_camera1_control )
	Q_EMIT set_camera1_control_changed( data.name(), data.value() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_camera1_control )
	Q_EMIT set_camera1_control_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_camera1_capabilities )
	Q_EMIT camera1_capabilities_changed( data.cameraName(), data.resolutions(), data.controls() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_camera1_capabilities )
	Q_EMIT camera1_capabilities_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_camera1_calibration )
	Q_EMIT camera1_calibration_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_camera1_calibration )
	Q_EMIT camera1_calibration_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_image1 )
	Q_EMIT image1_changed( data.data(), data.width(), data.height(), data.step(), data.format() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_image1 )
	Q_EMIT image1_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

//***************************************************************************

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_camera2_settings )
	Q_EMIT camera2_settings_changed( data.width(), data.height(), data.format() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_camera2_settings )
	Q_EMIT camera2_settings_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_camera2_settings )
	Q_EMIT set_camera2_settings_changed( data.width(), data.height(), data.format() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_camera2_settings )
	Q_EMIT set_camera2_settings_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_camera2_control )
	Q_EMIT set_camera2_control_changed( data.name(), data.value() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_camera2_control )
	Q_EMIT set_camera2_control_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_camera2_capabilities )
	Q_EMIT camera2_capabilities_changed( data.cameraName(), data.resolutions(), data.controls() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_camera2_capabilities )
	Q_EMIT camera2_capabilities_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_camera2_calibration )
	Q_EMIT camera2_calibration_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_camera2_calibration )
	Q_EMIT camera2_calibration_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_image2 )
	Q_EMIT image2_changed( data.data(), data.width(), data.height(), data.step(), data.format() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_image2 )
	Q_EMIT image2_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

//***************************************************************************

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_camera3_settings )
	Q_EMIT camera3_settings_changed( data.width(), data.height(), data.format() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_camera3_settings )
	Q_EMIT camera3_settings_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_camera3_settings )
	Q_EMIT set_camera3_settings_changed( data.width(), data.height(), data.format() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_camera3_settings )
	Q_EMIT set_camera3_settings_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_camera3_control )
	Q_EMIT set_camera3_control_changed( data.name(), data.value() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_camera3_control )
	Q_EMIT set_camera3_control_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_camera3_capabilities )
	Q_EMIT camera3_capabilities_changed( data.cameraName(), data.resolutions(), data.controls() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_camera3_capabilities )
	Q_EMIT camera3_capabilities_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_camera3_calibration )
	Q_EMIT camera3_calibration_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_camera3_calibration )
	Q_EMIT camera3_calibration_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_image3 )
	Q_EMIT image3_changed( data.data(), data.width(), data.height(), data.step(), data.format() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_image3 )
	Q_EMIT image3_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

//***************************************************************************

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_motor0_setpoint )
	Q_EMIT motor0_setpoint_changed( data.ref(), client );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_motor0_setpoint )
	Q_EMIT motor0_setpoint_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_motor0_reset_position )
	Q_EMIT motor0_reset_position_changed( data.ref(), client );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_motor0_reset_position )
	Q_EMIT motor0_reset_position_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_motor0_mode )
	Q_EMIT set_motor0_mode_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_motor0_mode )
	Q_EMIT set_motor0_mode_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_motor0_mode )
	Q_EMIT motor0_mode_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_motor0_mode )
	Q_EMIT motor0_mode_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_motor1_setpoint )
	Q_EMIT motor1_setpoint_changed( data.ref(), client );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_motor1_setpoint )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT motor1_setpoint_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_motor1_reset_position )
	Q_EMIT motor1_reset_position_changed( data.ref(), client );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_motor1_reset_position )
	Q_EMIT motor1_reset_position_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_motor1_mode )
	Q_EMIT set_motor1_mode_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_motor1_mode )
	Q_EMIT set_motor1_mode_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_motor1_mode )
	Q_EMIT motor1_mode_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_motor1_mode )
	Q_EMIT motor1_mode_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_motor2_setpoint )
	Q_EMIT motor2_setpoint_changed( data.ref(), client );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_motor2_setpoint )
	Q_EMIT motor2_setpoint_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_motor2_reset_position )
	Q_EMIT motor2_reset_position_changed( data.ref(), client );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_motor2_reset_position )
	Q_EMIT motor2_reset_position_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_motor2_mode )
	Q_EMIT set_motor2_mode_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_motor2_mode )
	Q_EMIT set_motor2_mode_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_motor2_mode )
	Q_EMIT motor2_mode_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_motor2_mode )
	Q_EMIT motor2_mode_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_motor3_setpoint )
	Q_EMIT motor3_setpoint_changed( data.ref(), client );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_motor3_setpoint )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT motor3_setpoint_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_motor3_reset_position )
	Q_EMIT motor3_reset_position_changed( data.ref(), client );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_motor3_reset_position )
	Q_EMIT motor3_reset_position_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_motor3_mode )
	Q_EMIT set_motor3_mode_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_motor3_mode )
	Q_EMIT set_motor3_mode_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_motor3_mode )
	Q_EMIT motor3_mode_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_motor3_mode )
	Q_EMIT motor3_mode_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_motor_setpoints )
	Q_EMIT motor_setpoints_changed( data.m0(), data.m1(), data.m2(), client );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_motor_setpoints )
	Q_EMIT motor_setpoints_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_motor_readings )
	Q_EMIT motor_readings_changed( data.speeds(), data.positions(), data.currents(), data.time_delta() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_motor_readings )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT motor_readings_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_omnidrive )
	Q_EMIT omnidrive_changed( data.vx(), data.vy(), data.omega(), client );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_omnidrive )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT omnidrive_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_odometry )
	Q_EMIT odometry_changed( data.x(), data.y(), data.phi(), data.vx(), data.vy(), data.omega(), data.seq() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_odometry )
	Q_EMIT odometry_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_odometry )
	Q_EMIT set_odometry_changed( data.x(), data.y(), data.phi() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_odometry )
	Q_EMIT set_odometry_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION(Client, rec_robotino_rpc_pose)
Q_EMIT pose_changed(data.x(), data.y(), data.phi(), data.errx(), data.erry(), data.errphi(), data.seq());
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION(Client, rec_robotino_rpc_pose)
Q_EMIT pose_info_changed(info);
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_northstar )
	Q_EMIT northstar_changed( data.sequenceNumber(), data.roomId(), data.numSpotsVisible(), data.posX(), data.posY(), data.posTheta(), data.magSpot0(), data.magSpot1() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_northstar )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT northstar_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_northstar_parameters )
	Q_EMIT set_northstar_parameters_changed( data.roomId(), data.ceilingCal() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_northstar_parameters )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT set_northstar_parameters_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_gyroscope )
	Q_EMIT gyroscope_changed( data.angle(), data.rate(), client );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_gyroscope )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT gyroscope_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION(Client, rec_robotino_rpc_gyroscope_ext)
Q_EMIT gyroscope_ext_changed(data.angle(), data.rate(), client);
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION(Client, rec_robotino_rpc_gyroscope_ext)
// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
Q_EMIT gyroscope_ext_info_changed(info);
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_emergency_bumper )
	Q_EMIT emergency_bumper_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_emergency_bumper )
	Q_EMIT emergency_bumper_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_emergency_bumper )
	Q_EMIT set_emergency_bumper_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_emergency_bumper )
	Q_EMIT set_emergency_bumper_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_display_text )
	Q_EMIT display_text_changed( data.text(), data.row(), data.col(), data.clear_before(), data.clear_after() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_display_text )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT display_text_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_display_backlight)
	Q_EMIT display_backlight_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_display_backlight )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT display_backlight_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_display_buttons )
	Q_EMIT display_buttons_changed( data.up(), data.down(), data.back(), data.enter() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_display_buttons )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT display_buttons_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_display_vbar )
	Q_EMIT display_vbar_changed( data.value(), data.col(), data.start_row(), data.end_row() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_display_vbar )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT display_vbar_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_display_hbar )
	Q_EMIT display_hbar_changed( data.value(), data.row(), data.start_col(), data.end_col() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_display_hbar )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT display_hbar_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_display_progress )
	Q_EMIT display_progress_changed( data.step(), data.row() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_display_progress )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT display_progress_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_display_clear )
	Q_EMIT display_clear_changed();
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_display_clear )
	Q_EMIT display_clear_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_parameters )
	Q_EMIT parameters_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_parameters )
	Q_EMIT parameters_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_ea_version )
	Q_EMIT ea_version_changed( data.board(), data.firmware_major(), data.firmware_minor(), data.firmware_patch() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_ea_version )
	Q_EMIT ea_version_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_ea09_ip4address )
	Q_EMIT ea09_ip4address_changed( data.address(), data.netmask() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_ea09_ip4address )
	Q_EMIT ea09_ip4address_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_ea09_ip4address )
	Q_EMIT set_ea09_ip4address_changed( data.address(), data.netmask() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_ea09_ip4address )
	Q_EMIT set_ea09_ip4address_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_clients_connected )
	Q_EMIT clients_connected_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_clients_connected )
	Q_EMIT clients_connected_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_digital_input )
	Q_EMIT digital_input_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_digital_input )
	Q_EMIT digital_input_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_analog_input )
	Q_EMIT analog_input_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_analog_input )
	Q_EMIT analog_input_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_distance_sensors )
	Q_EMIT distance_sensors_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_distance_sensors )
	Q_EMIT distance_sensors_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_bumper )
	Q_EMIT bumper_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_bumper )
	Q_EMIT bumper_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_battery )
	Q_EMIT battery_changed( data.battery_voltage(), data.system_current(), data.ext_power(), data.num_chargers(), data.battery_type(), data.battery_low(), data.battery_low_shutdown_counter() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_battery )
	Q_EMIT battery_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_digital_output )
	Q_EMIT set_digital_output_changed( data.index(), data.state() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_digital_output )
	Q_EMIT set_digital_output_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_digital_output_array )
	Q_EMIT set_digital_output_array_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_digital_output_array )
	Q_EMIT set_digital_output_array_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_relay )
	Q_EMIT set_relay_changed( data.index(), data.state() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_relay )
	Q_EMIT set_relay_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_relay_array )
	Q_EMIT set_relay_array_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_relay_array )
	Q_EMIT set_relay_array_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_shutdown )
	Q_EMIT set_shutdown_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_shutdown )
	Q_EMIT set_shutdown_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION(Client, rec_robotino_rpc_start_hotswap)
Q_EMIT start_hotswap_changed();
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION(Client, rec_robotino_rpc_start_hotswap)
Q_EMIT start_hotswap_info_changed(info);
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION(Client, rec_robotino_rpc_start_motor_debug)
Q_EMIT start_motor_debug_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION(Client, rec_robotino_rpc_start_motor_debug)
Q_EMIT start_motor_debug_info_changed(info);
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_power_button )
	Q_EMIT power_button_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_power_button )
	Q_EMIT power_button_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_request_shutdown )
	Q_EMIT request_shutdown_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_request_shutdown )
	Q_EMIT set_shutdown_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_pid_parameters )
	Q_EMIT set_pid_parameters_changed( data.motor(), data.kp(), data.ki(), data.kd() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_pid_parameters )
	Q_EMIT set_pid_parameters_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_gripper_state )
	Q_EMIT gripper_state_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_gripper_state )
	Q_EMIT gripper_state_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_scan0 )
	Q_EMIT scan0_changed( data.ranges(), data.intensities(), data.parameters() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_scan0 )
	Q_EMIT scan0_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_scan1 )
	Q_EMIT scan1_changed( data.ranges(), data.intensities(), data.parameters() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_scan1 )
	Q_EMIT scan1_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_scan2 )
	Q_EMIT scan2_changed( data.ranges(), data.intensities(), data.parameters() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_scan2 )
	Q_EMIT scan2_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_scan3 )
	Q_EMIT scan3_changed( data.ranges(), data.intensities(), data.parameters() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_scan3 )
	Q_EMIT scan3_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_cbha_readings )
	Q_EMIT cbha_readings_changed( data.pressures(), data.pressureSensor(), data.stringPots(), data.foilPot() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_cbha_readings )
	Q_EMIT cbha_readings_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_cbha_set_pressure )
	Q_EMIT cbha_set_pressure_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_cbha_set_pressure )
	Q_EMIT cbha_set_pressure_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_cbha_set_compressors_enabled )
	Q_EMIT cbha_set_compressors_enabled_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_cbha_set_compressors_enabled )
	Q_EMIT cbha_set_compressors_enabled_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_cbha_set_water_drain_valve )
	Q_EMIT cbha_set_water_drain_valve_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_cbha_set_water_drain_valve )
	Q_EMIT cbha_set_water_drain_valve_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_cbha_set_gripper_valve1 )
	Q_EMIT cbha_set_gripper_valve1_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_cbha_set_gripper_valve1 )
	Q_EMIT cbha_set_gripper_valve1_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_cbha_set_gripper_valve2 )
	Q_EMIT cbha_set_gripper_valve2_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_cbha_set_gripper_valve2 )
	Q_EMIT cbha_set_gripper_valve2_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_grappler_store_positions )
	Q_EMIT grappler_store_positions_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_grappler_store_positions )
	Q_EMIT grappler_store_positions_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_grappler_set_power )
	Q_EMIT grappler_set_power_changed( data.line(), data.power() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_grappler_set_power )
	Q_EMIT grappler_set_power_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_grappler_set_positions )
	Q_EMIT grappler_set_positions_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_grappler_set_positions )
	Q_EMIT grappler_set_positions_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_grappler_servos )
	Q_EMIT grappler_servos_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_grappler_servos )
	Q_EMIT grappler_servos_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_grappler_readings )
	Q_EMIT grappler_readings_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_grappler_readings )
	Q_EMIT grappler_readings_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_grappler_toggle_torque )
	Q_EMIT grappler_toggle_torque_changed();
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_grappler_toggle_torque )
	Q_EMIT grappler_toggle_torque_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_log )
	Q_EMIT log_changed( data.publisher(), data.message(), data.level() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_log )
	Q_EMIT log_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_set_log_level )
	Q_EMIT set_log_level_changed( data.publisher(), data.level() );
	if( data.publisher() == QCoreApplication::applicationName() )
	{
		Q_EMIT logLevelChanged( data.level() );
	}
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_set_log_level )
	Q_EMIT set_log_level_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_custom_message )
	Q_EMIT custom_message_changed( data.id(), data.data() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_custom_message )
	Q_EMIT custom_message_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_custom_message0 )
	Q_EMIT custom_message0_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_custom_message0 )
	Q_EMIT custom_message0_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_custom_message1 )
	Q_EMIT custom_message1_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_custom_message1 )
	Q_EMIT custom_message1_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_custom_message2 )
	Q_EMIT custom_message2_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_custom_message2 )
	Q_EMIT custom_message2_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_custom_message3 )
	Q_EMIT custom_message3_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_custom_message3 )
	Q_EMIT custom_message3_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect0_set_tilt )
	Q_EMIT kinect0_set_tilt_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect0_set_tilt )
	Q_EMIT kinect0_set_tilt_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect1_set_tilt )
	Q_EMIT kinect1_set_tilt_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect1_set_tilt )
	Q_EMIT kinect1_set_tilt_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect2_set_tilt )
	Q_EMIT kinect2_set_tilt_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect2_set_tilt )
	Q_EMIT kinect2_set_tilt_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect3_set_tilt )
	Q_EMIT kinect3_set_tilt_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect3_set_tilt )
	Q_EMIT kinect3_set_tilt_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect0_tilt )
	Q_EMIT kinect0_tilt_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect0_tilt )
	Q_EMIT kinect0_tilt_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect1_tilt )
	Q_EMIT kinect1_tilt_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect1_tilt )
	Q_EMIT kinect1_tilt_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect2_tilt )
	Q_EMIT kinect2_tilt_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect2_tilt )
	Q_EMIT kinect2_tilt_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect3_tilt )
	Q_EMIT kinect3_tilt_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect3_tilt )
	Q_EMIT kinect3_tilt_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect0_accel )
	Q_EMIT kinect0_accel_changed( data.x(), data.y(), data.z() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect0_accel )
	Q_EMIT kinect0_accel_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect1_accel )
	Q_EMIT kinect1_accel_changed( data.x(), data.y(), data.z() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect1_accel )
	Q_EMIT kinect1_accel_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect2_accel )
	Q_EMIT kinect2_accel_changed( data.x(), data.y(), data.z() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect2_accel )
	Q_EMIT kinect2_accel_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect3_accel )
	Q_EMIT kinect3_accel_changed( data.x(), data.y(), data.z() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect3_accel )
	Q_EMIT kinect3_accel_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect0_set_led )
	Q_EMIT kinect0_set_led_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect0_set_led )
	Q_EMIT kinect0_set_led_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect1_set_led )
	Q_EMIT kinect1_set_led_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect1_set_led )
	Q_EMIT kinect1_set_led_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect2_set_led )
	Q_EMIT kinect2_set_led_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect2_set_led )
	Q_EMIT kinect2_set_led_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect3_set_led )
	Q_EMIT kinect3_set_led_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect3_set_led )
	Q_EMIT kinect3_set_led_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect0_led )
	Q_EMIT kinect0_led_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect0_led )
	Q_EMIT kinect0_led_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect1_led )
	Q_EMIT kinect1_led_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect1_led )
	Q_EMIT kinect1_led_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect2_led )
	Q_EMIT kinect2_led_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect2_led )
	Q_EMIT kinect2_led_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect3_led )
	Q_EMIT kinect3_led_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect3_led )
	Q_EMIT kinect3_led_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect0_set_video_format )
	Q_EMIT kinect0_set_video_format_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect0_set_video_format )
	Q_EMIT kinect0_set_video_format_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect1_set_video_format )
	Q_EMIT kinect1_set_video_format_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect1_set_video_format )
	Q_EMIT kinect1_set_video_format_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect2_set_video_format )
	Q_EMIT kinect2_set_video_format_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect2_set_video_format )
	Q_EMIT kinect2_set_video_format_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect3_set_video_format )
	Q_EMIT kinect3_set_video_format_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect3_set_video_format )
	Q_EMIT kinect3_set_video_format_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect0_video_format )
	Q_EMIT kinect0_video_format_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect0_video_format )
	Q_EMIT kinect0_video_format_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect1_video_format )
	Q_EMIT kinect1_video_format_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect1_video_format )
	Q_EMIT kinect1_video_format_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect2_video_format )
	Q_EMIT kinect2_video_format_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect2_video_format )
	Q_EMIT kinect2_video_format_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect3_video_format )
	Q_EMIT kinect3_video_format_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect3_video_format )
	Q_EMIT kinect3_video_format_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect0_set_depth_format )
	Q_EMIT kinect0_set_depth_format_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect0_set_depth_format )
	Q_EMIT kinect0_set_depth_format_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect1_set_depth_format )
	Q_EMIT kinect1_set_depth_format_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect1_set_depth_format )
	Q_EMIT kinect1_set_depth_format_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect2_set_depth_format )
	Q_EMIT kinect2_set_depth_format_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect2_set_depth_format )
	Q_EMIT kinect2_set_depth_format_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect3_set_depth_format )
	Q_EMIT kinect3_set_depth_format_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect3_set_depth_format )
	Q_EMIT kinect3_set_depth_format_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect0_depth_format )
	Q_EMIT kinect0_depth_format_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect0_depth_format )
	Q_EMIT kinect0_depth_format_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect1_depth_format )
	Q_EMIT kinect1_depth_format_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect1_depth_format )
	Q_EMIT kinect1_depth_format_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect2_depth_format )
	Q_EMIT kinect2_depth_format_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect2_depth_format )
	Q_EMIT kinect2_depth_format_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect3_depth_format )
	Q_EMIT kinect3_depth_format_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect3_depth_format )
	Q_EMIT kinect3_depth_format_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect0_depth )
	Q_EMIT kinect0_depth_changed( data.data(), data.object_data(), data.width(), data.height(), data.format(), data.stamp() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect0_depth )
	Q_EMIT kinect0_depth_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect1_depth )
	Q_EMIT kinect1_depth_changed( data.data(), data.object_data(), data.width(), data.height(), data.format(), data.stamp() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect1_depth )
	Q_EMIT kinect1_depth_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect2_depth )
	Q_EMIT kinect2_depth_changed( data.data(), data.object_data(), data.width(), data.height(), data.format(), data.stamp() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect2_depth )
	Q_EMIT kinect2_depth_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect3_depth )
	Q_EMIT kinect3_depth_changed( data.data(), data.object_data(), data.width(), data.height(), data.format(), data.stamp() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect3_depth )
	Q_EMIT kinect3_depth_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect0_video )
	Q_EMIT kinect0_video_changed( data.data(), data.width(), data.height(), data.format(), data.stamp() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect0_video )
	Q_EMIT kinect0_video_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect1_video )
	Q_EMIT kinect1_video_changed( data.data(), data.width(), data.height(), data.format(), data.stamp() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect1_video )
	Q_EMIT kinect1_video_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect2_video )
	Q_EMIT kinect2_video_changed( data.data(), data.width(), data.height(), data.format(), data.stamp() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect2_video )
	Q_EMIT kinect2_video_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_kinect3_video )
	Q_EMIT kinect3_video_changed( data.data(), data.width(), data.height(), data.format(), data.stamp() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_kinect3_video )
	Q_EMIT kinect3_video_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_process_status )
	Q_EMIT process_status_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_process_status )
	Q_EMIT process_status_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_process_output )
	Q_EMIT process_output_changed( data.ref() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_process_output )
	Q_EMIT process_output_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_disconnect_api1_clients )
	Q_EMIT disconnect_api1_clients_changed();
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_disconnect_api1_clients )
	Q_EMIT disconnect_api1_clients_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_api1_setstate_received )
	Q_EMIT api1_setstate_received_changed();
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_api1_setstate_received )
	Q_EMIT api1_setstate_received_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_map )
Q_EMIT map_changed(  data.map(), data.width(), data.height(),data.resolution(), data.offsetx(), data.offsety()  );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_map )
	Q_EMIT map_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_mapPlanner )
	Q_EMIT mapPlanner_changed(data.map(), data.width(), data.height(), data.resolution(), data.offsetx(), data.offsety());
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_mapPlanner )
	Q_EMIT mapPlanner_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION(Client, rec_robotino_rpc_mapPlannerEdited)
Q_EMIT mapPlannerEdited_changed(data.map(), data.width(), data.height(), data.resolution(), data.offsetx(), data.offsety());
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION(Client, rec_robotino_rpc_mapPlannerEdited)
Q_EMIT mapPlannerEdited_info_changed(info);
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_poseOnMap )
Q_EMIT poseOnMap_changed( data.location(),data.rotation_deg() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_poseOnMap )
	Q_EMIT poseOnMap_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_initialPose )
Q_EMIT initialPose_changed( data.location(),data.rotation_deg() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_initialPose )
	Q_EMIT initialPose_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_navGoal )
Q_EMIT navGoal_changed( data.location(),data.rotation_deg() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_navGoal )
	Q_EMIT navGoal_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

//************Charger
BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_charger0_info )
Q_EMIT charger0_info_changed( data.time(), data.batteryVoltage(), data.chargingCurrent(), data.bat1temp(), data.bat2temp(), data.state_number(), data.state() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_charger0_info )
	Q_EMIT charger0_info_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_charger1_info )
Q_EMIT charger1_info_changed( data.time(), data.batteryVoltage(), data.chargingCurrent(), data.bat1temp(), data.bat2temp(), data.state_number(), data.state() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_charger1_info )
	Q_EMIT charger1_info_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_charger2_info )
Q_EMIT charger2_info_changed( data.time(), data.batteryVoltage(), data.chargingCurrent(), data.bat1temp(), data.bat2temp(), data.state_number(), data.state() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_charger2_info )
	Q_EMIT charger2_info_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_charger0_version )
Q_EMIT charger0_version_changed( data.major(), data.minor(), data.patch() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_charger0_version )
	Q_EMIT charger0_version_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_charger1_version )
Q_EMIT charger1_version_changed( data.major(), data.minor(), data.patch() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_charger1_version )
	Q_EMIT charger1_version_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_charger2_version )
Q_EMIT charger2_version_changed( data.major(), data.minor(), data.patch() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_charger2_version )
	Q_EMIT charger2_version_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_charger0_error )
Q_EMIT charger0_error_changed( data.time(), data.message() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_charger0_error )
	Q_EMIT charger0_error_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_charger1_error )
Q_EMIT charger1_error_changed( data.time(), data.message() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_charger1_error )
	Q_EMIT charger1_error_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_charger2_error )
Q_EMIT charger2_error_changed( data.time(), data.message() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_charger2_error )
	Q_EMIT charger2_error_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_charger0_clear_error )
Q_EMIT charger0_clear_error_changed();
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_charger0_clear_error )
	Q_EMIT charger0_clear_error_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_charger1_clear_error )
Q_EMIT charger1_clear_error_changed();
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_charger1_clear_error )
	Q_EMIT charger1_clear_error_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_charger2_clear_error )
Q_EMIT charger2_clear_error_changed();
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_charger2_clear_error )
	Q_EMIT charger2_clear_error_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_sensors )
	Q_EMIT sensors_changed( data.names(), data.values(), data.units() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_sensors )
	Q_EMIT sensors_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION(Client, rec_robotino_rpc_motor_debug)
Q_EMIT motor_debug_changed(data.motor(), data.times(), data.speed_setpoints(), data.speeds(), data.currents(), data.control_points() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION(Client, rec_robotino_rpc_motor_debug)
Q_EMIT motor_debug_info_changed(info);
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Client, rec_robotino_rpc_fleetcom_request )
	Q_EMIT fleetcom_request_changed( data.message() );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Client, rec_robotino_rpc_fleetcom_request )
	Q_EMIT fleetcom_request_info_changed( info );
END_TOPICINFOCHANGED_DEFINITION


