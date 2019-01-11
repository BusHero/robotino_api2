//  Copyright (C) 2004-2013, Robotics Equipment Corporation GmbH

//Copyright (c) ...
//
//REC Robotics Equipment Corporation GmbH, Planegg, Germany. All rights reserved.
//Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//
//THIS SOFTWARE IS PROVIDED BY REC ROBOTICS EQUIPMENT CORPORATION GMBH �AS IS� AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL REC ROBOTICS EQUIPMENT CORPORATION GMBH
//BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
//GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
//LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//Copyright (c) ...
//
//REC Robotics Equipment Corporation GmbH, Planegg, Germany. Alle Rechte vorbehalten.
//Weiterverbreitung und Verwendung in nichtkompilierter oder kompilierter Form, mit oder ohne Ver�nderung, sind unter den folgenden Bedingungen zul�ssig:
//1) Weiterverbreitete nichtkompilierte Exemplare m�ssen das obige Copyright, diese Liste der Bedingungen und den folgenden Haftungsausschluss im Quelltext enthalten.
//2) Weiterverbreitete kompilierte Exemplare m�ssen das obige Copyright, diese Liste der Bedingungen und den folgenden Haftungsausschluss in der Dokumentation und/oder anderen Materialien, die mit dem Exemplar verbreitet werden, enthalten.
//
//DIESE SOFTWARE WIRD VON REC ROBOTICS EQUIPMENT CORPORATION GMBH OHNE JEGLICHE SPEZIELLE ODER IMPLIZIERTE GARANTIEN ZUR VERF�GUNG GESTELLT, DIE UNTER
//ANDEREM EINSCHLIESSEN: DIE IMPLIZIERTE GARANTIE DER VERWENDBARKEIT DER SOFTWARE F�R EINEN BESTIMMTEN ZWECK. AUF KEINEN FALL IST REC ROBOTICS EQUIPMENT CORPORATION GMBH
//F�R IRGENDWELCHE DIREKTEN, INDIREKTEN, ZUF�LLIGEN, SPEZIELLEN, BEISPIELHAFTEN ODER FOLGESCH�DEN (UNTER ANDEREM VERSCHAFFEN VON ERSATZG�TERN ODER -DIENSTLEISTUNGEN;
//EINSCHR�NKUNG DER NUTZUNGSF�HIGKEIT; VERLUST VON NUTZUNGSF�HIGKEIT; DATEN; PROFIT ODER GESCH�FTSUNTERBRECHUNG), WIE AUCH IMMER VERURSACHT UND UNTER WELCHER VERPFLICHTUNG
//AUCH IMMER, OB IN VERTRAG, STRIKTER VERPFLICHTUNG ODER UNERLAUBTER HANDLUNG (INKLUSIVE FAHRL�SSIGKEIT) VERANTWORTLICH, AUF WELCHEM WEG SIE AUCH IMMER DURCH DIE BENUTZUNG
//DIESER SOFTWARE ENTSTANDEN SIND, SOGAR, WENN SIE AUF DIE M�GLICHKEIT EINES SOLCHEN SCHADENS HINGEWIESEN WORDEN SIND.

#ifndef _REC_ROBOTINO_API2_WORKERTHREAD_HPP_
#define _REC_ROBOTINO_API2_WORKERTHREAD_HPP_

#include <QtCore>
#include <QtNetwork>

#include "rec/robotino/rpc/GrapplerServoInfo.h"
#include "rec/rpc/ClientInfo.h"

#include "rec/robotino/api2/ComId.h"
#include "rec/robotino/api2/comevent/Event.h"

#include "JsonTopicReaders.hpp"

namespace rec
{
	namespace robotino
	{
		namespace api2
		{
			class Com;
			class Client;

			class MotorReader;
			class ParameterReader;
			class BumperReader;
			class AnalogInputReader;
			class DigitalInputReader;
			class DistanceSensorsReader;
			class PowerManagementReader;
			class CameraReader;
			class CompactBHAReader;
			class ElectricalGripperReader;
			class GrapplerReader;
			class LaserRangeFinderReader;
			class NorthStarReader;
			class OdometryReader;
			class PoseReader;
			class KinectReader;
			class NavGoalReader;
			class ChargerReader;
			class PowerButtonReader;
			class CustomMessageReader;
			class GyroscopeReader;
			class GyroscopeExtReader;
			class Factory4Reader;
			class DigitalOutputReaderReader;
			class CustomVisReader;
			class SmartLogReader;
			class FestoolChargerReader;
			class MotorDebugReader;
			
			class WorkerThread : public QThread
			{
				Q_OBJECT
			public:
				const ComId comid;

				static const unsigned int numCameras = 4;
				static const unsigned int numRangefinders = 4;
				static const unsigned int numKinects = 4;
				static const unsigned int numChargers = 3;

				/**
				* Retrieves the ComImpl instance corresponding to the given comID
				* @return	The instance. Never returns NULL.
				* @throws	RobotinoException if there is no instance with the given comID
				*/
				static WorkerThread* instance( const ComId& id );

				static void initQt();

				static void shutdown();
				
				WorkerThread( Com* com );

				~WorkerThread();

				void connectToServer( bool isBlocking );
				void disconnectFromServer();
				bool isConnected() const;
				void setAutoReconnectEnabled( bool enable );
				void setAddress( const QString& address );
				const char* address() const;
				bool isLocalConnection() const;
				void setLocalIPCEnabled( bool enabled );


				void setName( const QString& name );

				void setMultiThreadedSerializationEnabled( bool enabled );

				unsigned int elapsed() const;


				void processEvents();
				void processComEvents();
				void processGrapplerEvents();
				void processCameraEvents( unsigned int number );
				void processScanEvents( unsigned int number );
				void processBumperEvents();
				void processAnalogInputEvents();
				void processDigitalInputEvents();
				void processDistanceSensorsEvents();
				void processPowerManagementEvents();
				void processKinectEvents( unsigned int number );
				void processChargerEvents();
				void processPowerButtonEvents();
				void processCustomMessageEvents();
				void processGyroscopeEvents();
				void processGyroscopeExtEvents();
				void processSmartDockingVisEvents();
				void processFactory4Events();
				void processSmartNavigationPlannerEvents();
				void processSmartCdlEvents();
				void processCustomVisEvents();
				void processSmartPersonTrackerEvents();
				void processSmartLogEvents();
				void processFestoolChargerEvents();

				void addMotorReader( MotorReader* m );
				void removeMotorReader( MotorReader* m );

				void addMotorDebugReader(MotorDebugReader* m);
				void removeMotorDebugReader(MotorDebugReader* m);

				void addParameterReader( ParameterReader* m );
				void removeParameterReader( ParameterReader* m );

				void addBumperReader( BumperReader* r );
				void removeBumperReader( BumperReader* r );

				void addAnalogInputReader( AnalogInputReader* r );
				void removeAnalogInputReader( AnalogInputReader* r );

				void addDigitalInputReader( DigitalInputReader* r );
				void removeDigitalInputReader( DigitalInputReader* r );

				void addDistanceSensorsReader( DistanceSensorsReader* r );
				void removeDistanceSensorsReader( DistanceSensorsReader* r );

				void addPowerManagementReader( PowerManagementReader* r );
				void removePowerManagementReader( PowerManagementReader* r );

				void addCameraReader( CameraReader* m, unsigned int number );
				void removeCameraReader( CameraReader* m );

				void addCompactBHAReader( CompactBHAReader* r );
				void removeCompactBHAReader( CompactBHAReader* r );

				void addElectricalGripperReader( ElectricalGripperReader* r );
				void removeElectricalGripperReader( ElectricalGripperReader* r );

				void addGrapplerReader( GrapplerReader* r );
				void removeGrapplerReader( GrapplerReader* r );

				void addLaserRangeFinderReader( LaserRangeFinderReader* r, unsigned int number );
				void removeLaserRangeFinderReader( LaserRangeFinderReader* r );

				void addNorthStarReader( NorthStarReader* r );
				void removeNorthStarReader( NorthStarReader* r );

				void addOdometryReader( OdometryReader* r );
				void removeOdometryReader( OdometryReader* r );

				void addPoseReader(PoseReader* r);
				void removePoseReader(PoseReader* r);

				void addKinectReader( KinectReader* r, unsigned int number );
				void removeKinectReader( KinectReader* r );
				
				void addNavGoalReader( NavGoalReader* r );
				void removeNavGoalReader( NavGoalReader* r );

				void addChargerReader( ChargerReader* r, unsigned int number );
				void removeChargerReader( ChargerReader* r );

				void addPowerButtonReader( PowerButtonReader* r );
				void removePowerButtonReader( PowerButtonReader* r );

				void addCustomMessageReader( CustomMessageReader* r );
				void removeCustomMessageReader( CustomMessageReader* r );

				void addGyroscopeReader( GyroscopeReader* r );
				void removeGyroscopeReader( GyroscopeReader* r );

				void addGyroscopeExtReader(GyroscopeExtReader* r);
				void removeGyroscopeExtReader(GyroscopeExtReader* r);

				void addSmartDockingVisReader(SmartDockingVisReader* r);
				void removeSmartDockingVisReader(SmartDockingVisReader* r);

				void addFactory4Reader(Factory4Reader* r);
				void removeFactory4Reader(Factory4Reader* r);

				void addDigitalOutputReaderReader(DigitalOutputReaderReader* r);
				void removeDigitalOutputReaderReader(DigitalOutputReaderReader* r);

				void addSmartNavigationPlannerReader(SmartNavigationPlannerReader* r);
				void removeSmartNavigationPlannerReader(SmartNavigationPlannerReader* r);

				void addSmartLogReader(SmartLogReader* r);
				void removeSmartLogReader(SmartLogReader* r);

				void addFestoolChargerReader(FestoolChargerReader* r);
				void removeFestoolChargerReader(FestoolChargerReader* r);

				void addSmartCdlReader(SmartCdlReader* r);
				void removeSmartCdlReader(SmartCdlReader* r);
				
				void addCustomVisReader(CustomVisReader* r);
				void removeCustomVisReader(CustomVisReader* r);

				void addSmartPersonTrackerReader(SmartPersonTrackerReader* r);
				void removeSmartPersonTrackerReader(SmartPersonTrackerReader* r);

				void addUploadProgramReader(UploadProgramReader* r){}
				void removeUploadProgramReader(UploadProgramReader* r){}

				void set_omnidrive( float vx, float vy, float omega );

				void set_motor0_setpoint( float speed );
				void motor0_reset_position( int position );
				void set_set_motor0_mode( const QString& mode );
				void set_motor1_setpoint( float speed );
				void motor1_reset_position( int position );
				void set_set_motor1_mode( const QString& mode );
				void set_motor2_setpoint( float speed );
				void motor2_reset_position( int position );
				void set_set_motor2_mode( const QString& mode );
				void set_motor3_setpoint( float speed );
				void motor3_reset_position( int position );
				void set_set_motor3_mode( const QString& mode );

				void set_motor_setpoints( float m0, float m1, float m2 );

				void set_set_pid_parameters( unsigned int motor, float kp, float ki, float kd );

				void set_cbha_set_pressure( const QVector<float>& pressures );
				void set_cbha_set_compressors_enabled( bool enabled );
				void set_cbha_set_water_drain_valve( bool open );
				void set_cbha_set_gripper_valve1( bool open );
				void set_cbha_set_gripper_valve2( bool open );

				void set_grappler_set_power( int line, bool power );
				void set_grappler_set_positions( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data );
				void set_grappler_toggle_torque();

				void set_set_camera_settings( unsigned int cameraNumber, unsigned int width, unsigned int height, const QString& format );
				void set_set_camera_control( unsigned int cameraNumber, const QString& name, int value );

				void set_image( unsigned int cameraNumber,
									const QByteArray& data,
									unsigned int width,
									unsigned int height,
									unsigned int step,
									const QString& format );

				void set_set_digital_output( unsigned int index, bool state );
				void set_set_digital_output_array( const QVector< bool >& );
				void set_set_relay( unsigned int index, bool state );
				void set_set_relay_array( const QVector< bool >& );
				void set_set_shutdown( bool shutdown );

				void set_set_northstar_parameters( unsigned int roomId, float ceilingCal );
				bool set_set_odometry( double x, double y, double phi, int timeout );

				void set_kinect_set_tilt( unsigned int id, double tilt );
				void set_kinect_set_led( unsigned int id, unsigned int state );
				void set_kinect_set_depth_format( unsigned int id, unsigned int format );
				void set_kinect_set_video_format( unsigned int id, unsigned int format );

				void charger_clear_error( unsigned int number );

				void set_shutdown();

				void set_custom_message( unsigned int id, const QByteArray& data );

				void start_hotswap();

				void start_motor_debug(int motor);

				void set_upload_program(const QByteArray& infoData, const QByteArray& data);

				/*Factory 4*/
				void set_fleetcom_request(const QString& message);
				void set_mapDir_request(const QByteArray& infoData, const QByteArray& data);
				void set_pathnetwork_edited(const QByteArray& pathnetwork);
				void set_smartlocations_edited(const QByteArray& locations);
				void set_smartstations_edited(const QByteArray& stations);
				void set_mapPlannerEdited(const QByteArray& image);
				void set_json_message_with_data(int topic, const QByteArray& jsonData, const QByteArray& data);


			Q_SIGNALS:
				void set_omnidrive_signal( float, float, float );
				void setTopicEnabled_signal(const QString&, bool);
				void setJsonWithDataTopicEnabled_signal(int, bool);
				void set_motor_setpoints_signal( float, float, float );
				void set_motor0_setpoint_signal( float speed );
				void motor0_reset_position_signal( int position );
				void set_set_motor0_mode_signal( const QString& mode );
				void set_motor1_setpoint_signal( float speed );
				void motor1_reset_position_signal( int position );
				void set_set_motor1_mode_signal( const QString& mode );
				void set_motor2_setpoint_signal( float speed );
				void motor2_reset_position_signal( int position );
				void set_set_motor2_mode_signal( const QString& mode );
				void set_motor3_setpoint_signal( float speed );
				void motor3_reset_position_signal( int position );
				void set_set_motor3_mode_signal( const QString& mode );
				void set_cbha_set_pressure_signal( const QVector<float>& pressures );
				void set_cbha_set_compressors_enabled_signal( bool enabled );
				void set_cbha_set_water_drain_valve_signal( bool open );
				void set_cbha_set_gripper_valve1_signal( bool open );
				void set_cbha_set_gripper_valve2_signal( bool open );
				void set_grappler_set_power_signal( int line, bool power );
				void set_grappler_set_positions_signal( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data );
				void set_grappler_toggle_torque_signal();
				void set_set_camera_settings_signal( unsigned int, unsigned int, unsigned int, const QString& );
				void set_set_camera_control_signal( unsigned int, const QString&, int );
				void set_set_digital_output_signal( unsigned int, bool );
				void set_set_digital_output_array_signal( const QVector< bool >& );
				void set_set_relay_signal( unsigned int, bool );
				void set_set_relay_array_signal( const QVector< bool >& );
				void set_set_shutdown_signal( bool );
				void set_set_pid_parameters_signal( unsigned int, float, float, float );
				void set_set_northstar_parameters_signal( unsigned int, float );
				void set_set_odometry_signal( double, double, double );
				void set_kinect_set_tilt_signal( unsigned int id, double tilt );
				void set_kinect_set_led_signal( unsigned int id, unsigned int state );
				void set_kinect_set_depth_format_signal( unsigned int id, unsigned int format );
				void set_kinect_set_video_format_signal( unsigned int id, unsigned int format );

				void charger0_clear_error_signal();
				void charger1_clear_error_signal();
				void charger2_clear_error_signal();

				void set_shutdown_signal( bool );


				void set_custom_message_signal( unsigned int id, const QByteArray& data );

				void set_image_signal( unsigned int cameraNumber,
									const QByteArray& data,
									unsigned int width,
									unsigned int height,
									unsigned int step,
									const QString& format );

				void set_fleetcom_request_signal(const QString& message);
				void set_mapDir_request_signal(const QByteArray& infoData, const QByteArray& data);
				void set_pathnetwork_edited_signal(const QByteArray& pathnetwork);
				void set_smartlocations_edited_signal(const QByteArray& locations);
				void set_smartstations_edited_signal(const QByteArray& stations);
				void set_mapPlannerEdited_signal(const QByteArray& image, int /*width*/, int /*height*/, float /*resolution*/, float /*offsetx*/, float /*offsety*/);

				void set_json_message_with_data_signal(int topic, const QByteArray& jsonData, const QByteArray& data);

				void start_hotswap_signal();

				void start_motor_debug_signal(int motor);

				void set_upload_program_signal(const QByteArray& infoData, const QByteArray& data);

			private Q_SLOTS:
				void on_is_connected();
				void on_is_disconnected();
				void on_error( QAbstractSocket::SocketError socketError, const QString& errorString );
				void on_log( const QString&, int level );

				void on_parameters_changed( const QMap< QString, QVariant >& values );
				void on_motor_readings_changed( const QVector< float >& speeds, const QVector< int >& positions, const QVector< float >& currents, float time_delta );
				void on_digital_input_changed( const QVector< bool >& );
				void on_analog_input_changed( const QVector< float >& );
				void on_distance_sensors_changed( const QVector< float >& );
				void on_bumper_changed( bool );
				void on_battery_changed( float battery_voltage, float system_current, bool ext_power, int num_chargers, const QString& batteryType, bool batteryLow, int batteryLowShutdownCounter );
				
				void on_image0_changed( const QByteArray& data,
									unsigned int width,
									unsigned int height,
									unsigned int step,
									const QString& format );
				void on_camera0_settings_changed( unsigned int width, unsigned int height, const QString& format );
				void on_camera0_capabilities_changed( const QString& cameraName, const QMap<QString, QVector<QSize> >& capabilities, const QStringList& controls );
				void on_camera0_calibration_changed( const QVector<double>& calibration );

				void on_image1_changed( const QByteArray& data,
									unsigned int width,
									unsigned int height,
									unsigned int step,
									const QString& format );
				void on_camera1_settings_changed( unsigned int width, unsigned int height, const QString& format );
				void on_camera1_capabilities_changed( const QString& cameraName, const QMap<QString, QVector<QSize> >& capabilities, const QStringList& controls );
				void on_camera1_calibration_changed( const QVector<double>& calibration );

				void on_image2_changed( const QByteArray& data,
									unsigned int width,
									unsigned int height,
									unsigned int step,
									const QString& format );
				void on_camera2_settings_changed( unsigned int width, unsigned int height, const QString& format );
				void on_camera2_capabilities_changed( const QString& cameraName, const QMap<QString, QVector<QSize> >& capabilities, const QStringList& controls );
				void on_camera2_calibration_changed( const QVector<double>& calibration );

				void on_image3_changed( const QByteArray& data,
									unsigned int width,
									unsigned int height,
									unsigned int step,
									const QString& format );
				void on_camera3_settings_changed( unsigned int width, unsigned int height, const QString& format );
				void on_camera3_capabilities_changed( const QString& cameraName, const QMap<QString, QVector<QSize> >& capabilities, const QStringList& controls );
				void on_camera3_calibration_changed( const QVector<double>& calibration );

				void on_cbha_readings_changed( const QVector<float>& pressures, bool pressureSensor, const QVector<float>& stringPots, float foilPot );

				void on_gripper_state_changed( int state );

				void on_grappler_store_positions_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data );
				void on_grappler_servos_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data );
				void on_grappler_readings_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data );
				void on_grappler_toggle_torque_changed();

				void on_scan0_changed( const QVector< float >& ranges, const QVector< float >& intensities, const QVariantMap& parameters );

				void on_scan1_changed( const QVector< float >& ranges, const QVector< float >& intensities, const QVariantMap& parameters );

				void on_scan2_changed( const QVector< float >& ranges, const QVector< float >& intensities, const QVariantMap& parameters );

				void on_scan3_changed( const QVector< float >& ranges, const QVector< float >& intensities, const QVariantMap& parameters );

				void on_northstar_changed( unsigned int sequenceNumber,
										unsigned int roomId,
										unsigned int numSpotsVisible,
										float posX,
										float posY,
										float posTheta,
										unsigned int magSpot0,
										unsigned int magSpot1 );

				void on_odometry_changed( double, double, double, float, float, float, unsigned int );
				void on_pose_changed(double x, double y, double phi, double errx, double erry, double errphi, unsigned int sequence);

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

				void on_charger0_info_changed( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state );
				void on_charger0_version_changed( int major, int minor, int patch );
				void on_charger0_error_changed( unsigned int time, const QString& message );
				void on_charger1_info_changed( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state );
				void on_charger1_version_changed( int major, int minor, int patch );
				void on_charger1_error_changed( unsigned int time, const QString& message );
				void on_charger2_info_changed( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state );
				void on_charger2_version_changed( int major, int minor, int patch );
				void on_charger2_error_changed( unsigned int time, const QString& message );

				void on_power_button_changed( bool pressed );

				void on_custom_message_changed( unsigned int id, const QByteArray& data );

				void on_gyroscope_changed(double phi, double rate, const rec::rpc::ClientInfo&);
				void on_gyroscope_ext_changed(double phi, double rate, const rec::rpc::ClientInfo&);

				void on_map_changed(const QByteArray&, int, int, float, float, float);
				void on_mapPlanner_changed(const QByteArray&, int, int, float, float, float);

				void on_fleetcom_response_changed(const QByteArray& data);

				void on_mclayout_changed(const QByteArray& data);
				void on_mcstatus_changed(const QByteArray& data);
				void on_pathnetwork_changed(const QByteArray& data);
				void on_localizationMode_changed(const QByteArray& data);
				void on_smartlog_changed(const QByteArray& data);
				void on_festool_charger_info_changed(const QByteArray& data);
				void on_smartnavigationplan_changed(const QByteArray& jsonData);
				void on_smartrobotinfo_changed(const QByteArray& jsonData);
				void on_smartlocations_changed(const QByteArray& jsonData);
				void on_smartstations_changed(const QByteArray& jsonData);
				void on_smartmyrobotid_changed(const QByteArray& jsonData);
				void on_smartjoblist_changed(const QByteArray& jsonData);
				void on_smartlicense_changed(const QByteArray& jsonData);
				void on_smartdockingvis_changed(const QByteArray& jsonData);
				void on_mapDir_changed(const QByteArray& jsonData);
				void on_mapDir_response_changed(const QByteArray& jsonData, const QByteArray& data);

				void on_json_message_with_data_changed(int topic, const QByteArray& jsonData, const QByteArray& data);

				void on_set_digital_output_changed(unsigned int index, bool state);
				void on_set_digital_output_array_changed(const QVector< bool >&);

				void on_motor_debug_changed(int motor, const QVector<unsigned int>& times, const QVector<float>& speed_setpoints, const QVector<float>& speeds, const QVector<float>& currents, const QVector<float>& control_points);

			private:
				void run();

				void enableFactory4Topic(bool enable);
				
				static QMap< ComId, WorkerThread* >* _instances;
				static QMutex* _instancesMutex;

				QList<MotorReader*> _motorReaders;
				QList<MotorDebugReader*> _motorDebugReaders;
				QList<ParameterReader*> _parameterReaders;
				QList<BumperReader*> _bumperReaders;
				QList<AnalogInputReader*> _analogInputReaders;
				QList<DigitalInputReader*> _digitalInputReaders;
				QList<DistanceSensorsReader*> _distanceSensorsReaders;
				QList<PowerManagementReader*> _powerManagementReaders;
				QList<CameraReader*> _cameraReaders[numCameras];
				QList<CompactBHAReader*> _compactBHAReaders;
				QList<ElectricalGripperReader*> _electricalGripperReaders;
				QList<GrapplerReader*> _grapplerReaders;
				QList<LaserRangeFinderReader*> _laserRangeFinderReaders[numRangefinders];
				QList<NorthStarReader*> _northStarReaders;
				QList<OdometryReader*> _odometryReaders;
				QList<PoseReader*> _poseReaders;
				QList<KinectReader*> _kinectReaders[numKinects];
				QList<NavGoalReader*> _navGoalReaders;
				QList<ChargerReader*> _chargerReaders[numChargers];
				QList<PowerButtonReader*> _powerButtonReaders;
				QList<CustomMessageReader*> _customMessageReaders;
				QList<GyroscopeReader*> _gyroscopeReaders;
				QList<GyroscopeExtReader*> _gyroscopeExtReaders;
				QList<SmartDockingVisReader*> _SmartDockingVisReaders;
				QList<Factory4Reader*> _Factory4Readers;
				QList<DigitalOutputReaderReader*> _DigitalOutputReaderReaders;
				QList<SmartNavigationPlannerReader*> _SmartNavigationPlannerReaders;
				QList<SmartCdlReader*> _SmartCdlReaders;
				QList<CustomVisReader*> _CustomVisReaders;
				QList<SmartPersonTrackerReader*> _SmartPersonTrackerReaders;
				QList<SmartLogReader*> _SmartLogReaders;
				QList<FestoolChargerReader*> _FestoolChargerReaders;


				Com* _com;
				rec::robotino::api2::Client* _client;

				QSemaphore _startSemaphore;

				QMutex _mutex;
				QList<comevent::Event*> _events;
				unsigned int _numBumperEvents;
				unsigned int _numAnalogInputEvents;
				unsigned int _numDigitalInputEvents;
				unsigned int _numDistanceSensorsEvents;
				unsigned int _numBatteryEvents;
				unsigned int _numMotorReadingsEvent;
				unsigned int _numMotorDebugEvents;
				unsigned int _numParameterEvent;
				unsigned int _numImageEvent[numCameras];
				unsigned int _numCameraCapabilitiesEvent[numCameras];
				unsigned int _numCameraSettingsEvent[numCameras];
				unsigned int _numCameraCalibrationsEvent[numCameras];
				unsigned int _numCompactBHAReadingsEvent;
				unsigned int _numElectricalGrippersEvent;
				unsigned int _numGrapplerStorePositionsEvent;
				unsigned int _numGrapplerReadingsEvent;
				unsigned int _numGrapplerServosEvent;
				unsigned int _numGrapplerToggleTorqueEvent;
				unsigned int _numScanEvents[numRangefinders];
				unsigned int _numNorthStarEvents;
				unsigned int _numOdometryEvents;
				unsigned int _numPoseEvents;
				unsigned int _numDepthEvents[numKinects];
				unsigned int _numVideoEvents[numKinects];
				unsigned int _numTiltEvents[numKinects];
				unsigned int _numAccelEvents[numKinects];
				unsigned int _numChargerInfoEvents[numChargers];
				unsigned int _numChargerVersionEvents[numChargers];
				unsigned int _numChargerErrorEvents[numChargers];
				unsigned int _numPowerButtonEvents;
				unsigned int _numCustomMessageEvents;
				unsigned int _numGyroscopeEvents;
				unsigned int _numGyroscopeExtEvents;
				unsigned int _numDigitalOutputReaderEvents;

				/*Factory 4******************************/
				unsigned int _num_mclayout_events;
				unsigned int _num_mcstatus_events;
				unsigned int _num_pathnetwork_events;
				unsigned int _num_localizationMode_events;
				unsigned int _num_smartlog_events;
				unsigned int _num_smartnavigationplan_events;
				unsigned int _num_smartlocations_events;
				unsigned int _num_smartstations_events;
				unsigned int _num_smartrobotinfo_events;
				unsigned int _num_smartmyrobotid_events;
				unsigned int _num_smartjoblist_events;
				unsigned int _num_smartlicense_events;
				unsigned int _num_smartdockingvis_events;
				unsigned int _num_mapDir_events;
				unsigned int _num_mapDirWithData_events;
				unsigned int _num_fleetcom_response_events;
				unsigned int _num_mapDir_response_events;
				unsigned int _num_map_events;
				unsigned int _num_mapPlanner_events;
				unsigned int _num_smartNavigationPlanner_events;
				unsigned int _num_smartNavigationPlanner_response_events;
				unsigned int _num_smartNavigationCostMap_events;
				unsigned int _num_smartCdl_events;
				unsigned int _num_customVis_events;
				unsigned int _num_smartPersonTracker_events;
				unsigned int _num_smartLog_events;
				unsigned int _num_festool_charger_info_events;
				
				QMutex _odometrySetMutex;
				QWaitCondition _odometrySetCondition;
				unsigned int _odometrySequence;
			};
		}
	}
}

#endif //_REC_ROBOTINO_API2_WORKERTHREAD_HPP_
