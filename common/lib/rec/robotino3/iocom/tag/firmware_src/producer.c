#include "producer.h"
#include "tagenum.h"
#include "com.h"
#include "can.h"
#include "version.h"
#include "fpga.h"
#include "CAN.h"
#include "charger.h"
#include "motor.h"
#include "util.h"

#include <string.h>
#include <stdio.h>

extern char* com_get_string_buffer( void );
extern int charger_isp_getkey (void);

extern void adc_distance( unsigned char sensor, float* value );
extern float adc_motor_current( unsigned char motor );
extern void adc_io( unsigned char channel, float* value );
extern void adc_sys_current( float* value );
extern void adc_gpain( unsigned int channel, float* value );

//extern const unsigned char motor_mapping[4];
extern float motor_kp[4];
extern float motor_kp_default[4];
extern float motor_ki[4];
extern float motor_ki_default[4];
extern float motor_minaccel[4];
extern float motor_maxaccel[4];
extern const float motor_saftey_max_speed;

extern unsigned char cover_pwr_btn_is_pressed( void );
extern unsigned char power_ok_state( void );

extern unsigned char io_bumper( void );
extern unsigned char io_sus_s3( void );
extern unsigned char io_sus_s4( void );
extern unsigned char io_sus_s5( void );
extern unsigned char io_thrm( void );
extern unsigned char io_thrmtrip( void );
extern unsigned char io_is_ext_power( void );
extern unsigned char io_charger_exists( unsigned char number );
extern unsigned char io_nrst( void );
extern unsigned char io_boot( void );
extern unsigned char io_num_charger( void );
extern unsigned char io_is_ext_power( void );
extern unsigned char io_version0( void );
extern unsigned char io_version1( void );

extern float odometry_x;
extern float odometry_y;
extern float odometry_phi;

extern unsigned char config_reset_signal;

void produce_error( void );
void produce_config_reset( void );
void produce_charger_info( unsigned char chargerID );
void produce_charger_event( unsigned char chargerID );
void produce_charger_version( unsigned char chargerID );
void produce_charger_error( unsigned char chargerID );
void produce_power_source_readings( void );
void produce_power_button( void );
void produce_motor_debug( void );
void produce_festool_charger_info( void );

extern const unsigned short tlv_producer_payload_length;
extern void tlv_producer_read_buffer( unsigned char** p, unsigned short* length );
void tlv_producer_reset( void );
extern unsigned char tlv_producer_check_overrun( unsigned char size );
extern void tlv_producer_write_payload( unsigned char ch );
extern void tlv_producer_write_payload_uint16( unsigned short value );
extern void tlv_producer_write_payload_int16( signed short value );
extern void tlv_producer_write_payload_uint32( unsigned int value );
extern void tlv_producer_write_payload_int32( signed int value );
extern void tlv_producer_write_payload_float( float value );

void producer_read_buffer( unsigned char** p, unsigned short* length )
{
	static unsigned int pb_charger_info_time[3] = {0,0,0};
	static unsigned char charger_version_seq[3] = {0,0,0};
	static unsigned int festool_charger_time = 0;

	int i=0;

	if( 0 == tlv_producer_payload_length )
	{
		*length = 0;
		return;
	}
	//printf( "producer_payload_length %d\n\r", producer_payload_length );

	if( config_reset_signal )
	{
		config_reset_signal = 0;
		produce_config_reset();
	}

	for( i=0; i<3; ++i )
	{
		if( charger_version_array[i].seq != charger_version_seq[i] )
		{
			charger_version_seq[i] = charger_version_array[i].seq;
			produce_charger_version( i );
		}

		if( pb_charger_info_array[i].time != pb_charger_info_time[i] )
		{
			pb_charger_info_time[i] = pb_charger_info_array[i].time;
			produce_charger_info( i );
		}
	}
	
	if( festool_charger_time != festool_charger_info.time )
	{
		festool_charger_time = festool_charger_info.time;
		produce_festool_charger_info();
	}

	produce_motor_debug();
	produce_power_source_readings();
	produce_power_button();

	tlv_producer_read_buffer( p, length );
}

void producer_reset( void )
{
	tlv_producer_reset();
}

void produce_motor_debug( void )
{
	static motor_debug_data dbg;
	static int numPkgsRemaining;
	static int length;
	
	numPkgsRemaining = motor_get_debug_data(&dbg);

	if(-1 == numPkgsRemaining)
	{
		return;
	}
	
	if(numPkgsRemaining>30)
	{
		numPkgsRemaining = 30;
	}
	
	length = 6+8+8*numPkgsRemaining;
	
	if( tlv_producer_check_overrun( 2+length ) )
	{
		return;
	}
	
	tlv_producer_write_payload( TAG_MOTOR_DEBUG );
	tlv_producer_write_payload(length);
	tlv_producer_write_payload(motor_currentMonitor());
	tlv_producer_write_payload_uint32(dbg.time);
	tlv_producer_write_payload(motor_time_delta);
	
	while(numPkgsRemaining>=0)
	{
		tlv_producer_write_payload_int16(dbg.speed_setpoint);
		tlv_producer_write_payload_int16(dbg.speed);
		tlv_producer_write_payload_uint16(dbg.current);
		tlv_producer_write_payload_int16(dbg.control_point);
		
		if(numPkgsRemaining>0) motor_get_debug_data(&dbg);
		
		--numPkgsRemaining;
	}
}

void produce_festool_charger_info( void )
{
	#define BUFFERSIZE 64
	static char buffer[BUFFERSIZE];
	unsigned char buflength;
	unsigned char ch;
	unsigned char i;
	
	for(buflength=0;buflength<BUFFERSIZE;++buflength)
	{
		int ret = charger_isp_getkey();
		if(-1==ret)
		{
			break;
		}
		buffer[buflength] = ret;
	}
	
	if( tlv_producer_check_overrun( 2+18+buflength ) )
	{
		return;
	}
	
	tlv_producer_write_payload( TAG_FESTOOL_CHARGER_INFO );
	tlv_producer_write_payload( 18+buflength );
	
	tlv_producer_write_payload_uint32(festool_charger_info.time);
	
	ch = 0;
	for(i=0; i<4; ++i)
	{
		if(festool_charger_info.akku_load[i])
		{
			setBit(ch,i);
		}
		
		if(festool_charger_info.akku_on[i])
		{
			setBit(ch,(i+4));
		}
	}
	tlv_producer_write_payload( ch );
	
	ch=0;
	if(festool_charger_info.ext_power) setBit(ch,0);
	if(festool_charger_info.charger_present) setBit(ch,1);
	tlv_producer_write_payload( ch );
	
	for(i=0; i<4; ++i)
	{
		tlv_producer_write_payload_uint16(festool_charger_info.batteryVoltage[i]);
	}
	
	for(i=0; i<4; ++i)
	{
		tlv_producer_write_payload(festool_charger_info.batteryCapacity[i]);
	}
	
	for(i=0; i<buflength; ++i)
	{
		tlv_producer_write_payload(buffer[i]);
	}
}

void produce_hw_version( void )
{
	if( tlv_producer_check_overrun( 7 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_HW_VERSION );
	tlv_producer_write_payload( 5 );
	if( io_version0() )
	{
		tlv_producer_write_payload( '4' );
	}
	else
	{
		tlv_producer_write_payload( '3' );
	}
	tlv_producer_write_payload( '.' );
	tlv_producer_write_payload( '0' );
	tlv_producer_write_payload( '.' );
	tlv_producer_write_payload( '0' );
}

void produce_sw_version( void )
{
	int i;
	int versionstrlen = strlen( versionstr );
	if( tlv_producer_check_overrun( 2 + versionstrlen ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_SW_VERSION );
	tlv_producer_write_payload( versionstrlen );
	for( i=0; i<versionstrlen; ++i )
	{
		tlv_producer_write_payload( versionstr[i] );
	}
}

void produce_distance_sensor_readings( void )
{
	unsigned char i;
	float value;

	if( tlv_producer_check_overrun( 38 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_DISTANCE_SENSOR_READINGS );
	tlv_producer_write_payload( 36 );

	for( i=0; i<9; ++i )
	{
		adc_distance( i, &value );
		tlv_producer_write_payload_float( value );
	}
}

void produce_adc_parameters( void )
{
	if( tlv_producer_check_overrun( 7 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_ADC_PARAMETERS );
	tlv_producer_write_payload( 5 );
	tlv_producer_write_payload( 12 );
	tlv_producer_write_payload_float( 3.14f );
}

void produce_all_motor_speeds( void )
{
	unsigned char i;
	signed short speed;

	if( tlv_producer_check_overrun( 10 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_ALL_MOTOR_SPEEDS );
	tlv_producer_write_payload( 8 );

	for( i=0; i<4; ++i )
	{
		//speed = fpga_motor_velocity( motor_mapping[i] );
		fpga_motor_velocity( i, &speed );
		tlv_producer_write_payload_int16( speed );
	}
}

void produce_all_motor_positions( void )
{
	unsigned char i;
	signed int pos;

	if( tlv_producer_check_overrun( 18 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_ALL_MOTOR_POSITIONS );
	tlv_producer_write_payload( 16 );
	for( i=0; i<4; ++i )
	{
		//tlv_producer_write_payload_int32( fpga_motor_position( motor_mapping[i] ) );
		fpga_motor_position( i, &pos );
		tlv_producer_write_payload_int32( pos );
	}
}

void produce_all_pid_parameters( void )
{
	unsigned char motor;

	if( tlv_producer_check_overrun( 50 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_ALL_MOTOR_PID_PARAMETERS );
	tlv_producer_write_payload( 48 );
	for( motor=0; motor<4; ++motor )
	{
		//unsigned char motor = motor_mapping[i];
		float kp = motor_kp[motor];
		float ki = motor_ki[motor];
		if( kp < 0 )
		{
			kp = motor_kp_default[motor];
		}
		if( ki < 0 )
		{
			ki = motor_ki_default[motor];
		}
		tlv_producer_write_payload_float( kp );
		tlv_producer_write_payload_float( ki );
		tlv_producer_write_payload_float( 0.0f );
	}
}

void produce_odometry( void )
{
	float x;
	float y;
	float phi;

	if( tlv_producer_check_overrun( 14 ) )
	{
		return;
	}

	x = odometry_x;
	y = odometry_y;
	phi = odometry_phi;

	tlv_producer_write_payload( TAG_ODOMETRY );
	tlv_producer_write_payload( 12 );
	tlv_producer_write_payload_float( x );
	tlv_producer_write_payload_float( y );
	tlv_producer_write_payload_float( phi );
}

void produce_all_motor_current_readings( void )
{
	unsigned char motor;
	float value;

	if( tlv_producer_check_overrun( 18 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_ALL_MOTOR_CURRENT_READINGS );
	tlv_producer_write_payload( 16 );
	for( motor=0; motor<4; ++motor )
	{
		value = adc_motor_current( motor );
		tlv_producer_write_payload_float( value );
	}
}

void produce_all_analog_inputs( void )
{
	unsigned char i;
	float value;

	if( tlv_producer_check_overrun( 34 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_ALL_ANALOG_INPUTS );
	tlv_producer_write_payload( 32 );
	for( i=0; i<8; ++i )
	{
		adc_io( i, &value );
		tlv_producer_write_payload_float( value );
	}
}

void produce_all_digital_inputs( void )
{
	unsigned char val;

	if( tlv_producer_check_overrun( 3 ) )
	{
		return;
	}

	fpga_io_din( &val );

	tlv_producer_write_payload( TAG_ALL_DIGITAL_INPUTS );
	tlv_producer_write_payload( 1 );
	tlv_producer_write_payload( val );
}

void produce_bumper( void )
{
	if( tlv_producer_check_overrun( 3 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_BUMPER );
	tlv_producer_write_payload( 1 );
	tlv_producer_write_payload( io_bumper() );
}

void produce_power_button( void )
{
	if( tlv_producer_check_overrun( 3 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_POWER_BUTTON );
	tlv_producer_write_payload( 1 );
	tlv_producer_write_payload( cover_pwr_btn_is_pressed() );
}

void produce_fpga_power( void )
{
	unsigned char val;

	if( tlv_producer_check_overrun( 3 ) )
	{
		return;
	}

	fpga_power_status( &val );

	tlv_producer_write_payload( TAG_FPGA_POWER );
	tlv_producer_write_payload( 1 );
	tlv_producer_write_payload( val );
}

void produce_pwr_ok_state( void )
{
	if( tlv_producer_check_overrun( 3 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_PWR_OK_STATE );
	tlv_producer_write_payload( 1 );
	tlv_producer_write_payload( power_ok_state() );
}

void produce_com_express_states( void )
{
	if( tlv_producer_check_overrun( 7 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_COM_EXPRESS_STATES );
	tlv_producer_write_payload( 5 );
	tlv_producer_write_payload( io_sus_s3() );
	tlv_producer_write_payload( io_sus_s4() );
	tlv_producer_write_payload( io_sus_s5() );
	tlv_producer_write_payload( io_thrm() );
	tlv_producer_write_payload( io_thrmtrip() );
}

void produce_all_motor_readings( void )
{
	unsigned char i;
	float value;
	signed short speed;
	signed int pos;

	if( tlv_producer_check_overrun( 42 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_ALL_MOTOR_READINGS );
	tlv_producer_write_payload( 40 );

	for( i=0; i<4; ++i )
	{
		fpga_motor_velocity( i, &speed );
		tlv_producer_write_payload_int16( speed );
	}
	for( i=0; i<4; ++i )
	{
		fpga_motor_position( i, &pos );
		tlv_producer_write_payload_int32( pos );
	}
	for( i=0; i<4; ++i )
	{
		value = adc_motor_current( i );
		tlv_producer_write_payload_float( value );
	}
}

void produce_ip_address( void )
{
	if( tlv_producer_check_overrun( 10 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_IP_ADDRESS );
	tlv_producer_write_payload( 8 );
	tlv_producer_write_payload_uint32( 0x7F000001 );
	tlv_producer_write_payload_uint32( 0xFFFF0000 );
}

void produce_get_saftey_velocity( void )
{
	if( tlv_producer_check_overrun( 6 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_SAFTEY_VELOCITY );
	tlv_producer_write_payload( 4 );
	tlv_producer_write_payload_float( motor_saftey_max_speed );
}

void produce_power_source_readings( void )
{
	unsigned char num_charger=0;
	float current;

	if( tlv_producer_check_overrun( 18 ) )
	{
		return;
	}

	num_charger = io_num_charger();
	adc_sys_current( &current );

	tlv_producer_write_payload( TAG_POWER_SOURCE_READINGS );
	tlv_producer_write_payload( 16 );
	tlv_producer_write_payload_float( charger_max_batteryVoltage() ); //Power source voltage 
	tlv_producer_write_payload_float( current ); //Power source current 
	tlv_producer_write_payload( io_is_ext_power() ); //External power supply connected
	tlv_producer_write_payload( num_charger ); //Power source temperature 
	tlv_producer_write_payload( (num_charger>1?1:0) ); //battery type 0:Pb 1:NiMH
	tlv_producer_write_payload( charger_is_battery_low() ); //State of charge 
	tlv_producer_write_payload_uint32( charger_battery_low_count() ); //Error
}

void produce_motor_accel_limits( unsigned char motor )
{
	if( tlv_producer_check_overrun( 11 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_MOTOR_ACCEL_LIMITS );
	tlv_producer_write_payload( 9 );
	tlv_producer_write_payload( motor );
	tlv_producer_write_payload_float( 1000.0f * motor_minaccel[motor] );
	tlv_producer_write_payload_float( 1000.0f * motor_maxaccel[motor] );
}

void produce_can_msg( void )
{
	unsigned char i;
	
	CAN_msg msg;
	
	while(getCanMsg(&msg)>-1)
	{	
		if ( msg.id > 0x7FF )
		{
			msg.id = 0x7FF;
		}
		if ( msg.len > 8 )
		{
			msg.len = 8;
		}

		if( tlv_producer_check_overrun( 13 ) )
		{
			return;
		}

		tlv_producer_write_payload( TAG_CAN_MSG );
		tlv_producer_write_payload( 11 );
		tlv_producer_write_payload_uint16( msg.id );
		tlv_producer_write_payload( msg.len );
		for( i = 0; i < msg.len; ++i )
		{
			tlv_producer_write_payload( msg.data[i] );
		}
		for( ; i < 8; ++i )
		{
			tlv_producer_write_payload( 0 );
		}
	}
}

void produce_get_nrst( void )
{
	if( tlv_producer_check_overrun( 3 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_NRST );
	tlv_producer_write_payload( 1 );
	tlv_producer_write_payload( io_nrst() );
}

void produce_get_boot( void )
{
	if( tlv_producer_check_overrun( 3 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_BOOT );
	tlv_producer_write_payload( 1 );
	tlv_producer_write_payload( io_boot() );
}

void produce_charger_info( unsigned char chargerID )
{
	unsigned int i;
	const char* p = pb_charger_state( chargerID );
	unsigned int length = strlen( p );

	if( chargerID>=MAX_NUM_CHARGERS ||  tlv_producer_check_overrun( length+22 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_CHARGER_INFO );
	tlv_producer_write_payload( length+22 );
	tlv_producer_write_payload( chargerID );
	tlv_producer_write_payload_uint32( pb_charger_info_array[chargerID].time );
	tlv_producer_write_payload_float( pb_charger_info_array[chargerID].batteryVoltage );
	tlv_producer_write_payload_float( pb_charger_info_array[chargerID].current );
	tlv_producer_write_payload_float( pb_charger_info_array[chargerID].battery1temp );
	tlv_producer_write_payload_float( pb_charger_info_array[chargerID].battery2temp );
	tlv_producer_write_payload( pb_charger_info_array[chargerID].state );
	for( i=0; i<length; ++i )
	{
		tlv_producer_write_payload( (unsigned char)*(p+i) );
	}
}

void produce_charger_event( unsigned char chargerID )
{
}

void produce_charger_version( unsigned char chargerID )
{
	if( chargerID>= MAX_NUM_CHARGERS || tlv_producer_check_overrun( 6 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_CHARGER_VERSION );
	tlv_producer_write_payload( 4 );
	tlv_producer_write_payload( chargerID );
	tlv_producer_write_payload( charger_version_array[chargerID].major );
	tlv_producer_write_payload( charger_version_array[chargerID].minor );
	tlv_producer_write_payload( charger_version_array[chargerID].patch );
}

void produce_charger_error( unsigned char chargerID )
{
	const char* str = charger_error_message( chargerID );
	int length = strlen( str );
	int i;

	if( chargerID>=MAX_NUM_CHARGERS || tlv_producer_check_overrun( 7+length ) )
	{
		return;
	}

	//printf( str );
	//printf("\n\r");

	tlv_producer_write_payload( TAG_CHARGER_ERROR );
	tlv_producer_write_payload( 5+length );
	tlv_producer_write_payload( chargerID );
	tlv_producer_write_payload_uint32( pb_charger_error_array[chargerID].time );
	for( i=0; i<length; ++i )
	{
		tlv_producer_write_payload( (unsigned char)*(str+i) );
	}
}

void produce_battery_min( void )
{
	if( tlv_producer_check_overrun( 6 ) )
	{
		return;
	}
	
	tlv_producer_write_payload( TAG_BATTERY_MIN );
	tlv_producer_write_payload( 4 );
	tlv_producer_write_payload_float( charger_min_battery_voltage() );
}

void produce_gpain( unsigned char channel )
{
	float value;

	if( tlv_producer_check_overrun( 7 ) )
	{
		return;
	}

	adc_gpain( channel, &value );
	
	tlv_producer_write_payload( TAG_GPAIN );
	tlv_producer_write_payload( 5 );
	tlv_producer_write_payload( channel );
	tlv_producer_write_payload_float( value );
}

void produce_versionbits( void )
{
	if( tlv_producer_check_overrun( 4 ) )
	{
		return;
	}
	
	tlv_producer_write_payload( TAG_VERSIONBITS );
	tlv_producer_write_payload( 2 );
	tlv_producer_write_payload( io_version0() );
	tlv_producer_write_payload( io_version1() );
}

void produce_error( void )
{
	int i;
	char* p = com_get_string_buffer();
	unsigned char length = strlen( p );

	if( length > COM_STR_BUFFER_SIZE )
	{
		length = COM_STR_BUFFER_SIZE;
		p[COM_STR_BUFFER_SIZE-1] = 0;
	}

	printf( p );
	printf( "\n\r" );

	if( tlv_producer_check_overrun( length+2 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_ERROR );
	tlv_producer_write_payload( length );
	for( i=0; i<length; ++i )
	{
		tlv_producer_write_payload( (unsigned char)*(p+i) );
	}
}

void produce_info( void )
{
	int i;
	char* p = com_get_string_buffer();
	unsigned char length = strlen( p );

	if( length > COM_STR_BUFFER_SIZE )
	{
		length = COM_STR_BUFFER_SIZE;
		p[COM_STR_BUFFER_SIZE-1] = 0;
	}

	printf( p );
	printf( "\n\r" );

	if( tlv_producer_check_overrun( length+2 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_INFO );
	tlv_producer_write_payload( length );
	for( i=0; i<length; ++i )
	{
		tlv_producer_write_payload( (unsigned char)*(p+i) );
	}
}

void produce_warning( void )
{
	int i;
	char* p = com_get_string_buffer();
	unsigned char length = strlen( p );

	if( length > COM_STR_BUFFER_SIZE )
	{
		length = COM_STR_BUFFER_SIZE;
		p[COM_STR_BUFFER_SIZE-1] = 0;
	}

	printf( p );
	printf( "\n\r" );

	if( tlv_producer_check_overrun( length+2 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_WARNING );
	tlv_producer_write_payload( length );
	for( i=0; i<length; ++i )
	{
		tlv_producer_write_payload( (unsigned char)*(p+i) );
	}
}

void produce_config_reset( void )
{
	if( tlv_producer_check_overrun( 3 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_CONFIG_RESET );
	tlv_producer_write_payload( 1 );
	tlv_producer_write_payload( 1 );
}
