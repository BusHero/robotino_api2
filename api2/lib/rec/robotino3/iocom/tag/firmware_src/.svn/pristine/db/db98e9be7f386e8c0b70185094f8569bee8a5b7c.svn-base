#include "parser.h"
#include "com.h"
#include "tagenum.h"
#include "util.h"
#include "can.h"
#include "fpga.h"
#include "motor.h"
#include "CAN.h"
#include "charger.h"

#include <stdio.h>
#include <string.h>

extern char* com_get_string_buffer( void );

extern void shutdown( void );

extern unsigned char decode_uint16( unsigned char* buffer, unsigned short* value );
extern unsigned char decode_int16( unsigned char* buffer, signed short* value );
extern unsigned char decode_uint32( unsigned char* buffer, unsigned int* value );
extern unsigned char decode_int32( unsigned char* buffer, signed int* value );
extern unsigned char decode_float( unsigned char* buffer, float* value );

extern void set_power_ok_low( void );
extern void set_power_ok_high( void );

extern void	io_set_sysreset_low( void );
extern void	io_set_sysreset_high( void );
extern void io_set_pwrbtn_low( void );
extern void io_set_pwrbtn_high( void );
extern void io_set_dout( unsigned char pin, unsigned char value );
extern void io_power_off( void );
void io_nrst_high( void );
void io_nrst_low( void );
void io_set_boot( unsigned char num );

extern void pwm_set( unsigned char channel, unsigned char value );

extern void motor_set_on( unsigned char motor );
extern void motor_set_off( unsigned char motor );

extern float odometry_x;
extern float odometry_y;
extern float odometry_phi;

extern void produce_hw_version( void );
extern void produce_sw_version( void );
extern void produce_distance_sensor_readings( void );
extern void produce_adc_parameters( void );
extern void produce_all_motor_speeds( void ); 
extern void produce_all_motor_positions( void ); 
extern void produce_all_pid_parameters( void );
extern void produce_odometry( void );
extern void produce_all_motor_current_readings( void );
extern void produce_all_analog_inputs( void );
extern void produce_all_digital_inputs( void );
extern void produce_bumper( void );
extern void produce_power_button( void );
extern void produce_fpga_power( void );
extern void produce_error( void );
extern void produce_pwr_ok_state( void );
extern void produce_com_express_states( void );
extern void produce_all_motor_readings( void );
extern void produce_ip_address( void );
extern void produce_motor_accel_limits( unsigned char motor );
extern void produce_can_msg( void );
extern void produce_get_nrst( void );
extern void produce_get_boot( void );
extern void produce_charger_version( unsigned char chargerID );
extern void produce_get_saftey_velocity( void );
extern void produce_battery_min( void );
extern void produce_gpain( unsigned char channel );
extern void produce_versionbits( void );

extern void resetDevice( unsigned char enterUSBBootloader );

extern unsigned char emergency_bumper;

void parse_set_motor_speed( unsigned char* data )
{
	unsigned char motor;
	signed short speed;

	if( 3 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_MOTOR_SPEED invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	motor = data[1];
	if( motor > 3 )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_MOTOR_SPEED invalid motor %d", (int)motor );
		produce_error();
		return;
	}

	//speed in RPM
	decode_int16( &data[2], &speed );

#ifdef PRINTF_DEBUG
	printf( "TAG_SET_MOTOR_SPEED %d %d\n\r", (int)motor, speed );
#endif

	//motor_speed_setpoint[ motor_mapping[motor] ] = speed;
	motor_set_speed_setpoint( motor, speed );
}

void parse_set_motor_position( unsigned char* data )
{
	int position;
	unsigned char motor;

	if( 5 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_MOTOR_POSITION invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	motor = data[1];
	if( motor > 3 )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_MOTOR_POSITION invalid motor %d", (int)motor );
		produce_error();
		return;
	}

	decode_int32( &data[2], &position );

#ifdef PRINTF_DEBUG
	printf( "TAG_SET_MOTOR_POSITION %d %d\n\r", (int)data[1], position );
#endif

	//fpga_motor_set_position( motor_mapping[motor], position );
	fpga_motor_set_position( motor, position );
}

void parse_set_motor_pid_parameters( unsigned char* data )
{
	unsigned char motor;
	float kp,ki,kd;

	if( 13 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_MOTOR_PID_PARAMETERS invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	motor = data[1];
	if( motor > 3 )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_MOTOR_PID_PARAMETERS invalid motor %d", (int)motor );
		produce_error();
		return;
	}

	decode_float( data+2, &kp );
	decode_float( data+6, &ki );
	decode_float( data+10, &kd );
	
#ifdef PRINTF_DEBUG
	printf( "TAG_SET_MOTOR_PID_PARAMETERS %d %f %f %f\n\r", (int)motor, kp, ki, kd );
#endif

	//motor = motor_mapping[motor];
	motor_set_kp( motor, kp );
	motor_set_ki( motor, ki );
}

void parse_set_all_digital_outputs( unsigned char* data )
{
	unsigned char state = 0;

	if( 1 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_ALL_DIGITAL_OUTPUTS invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	state = data[1];

#ifdef PRINTF_DEBUG
	printf( "TAG_SET_ALL_DIGITAL_OUTPUTS 0x%2X\n\r", (int)state );
#endif

	io_set_dout( 0, state & 0x1 );
	io_set_dout( 1, state & 0x2 );
	io_set_dout( 2, state & 0x4 );
	io_set_dout( 3, state & 0x8 );
	io_set_dout( 4, state & 0x10 );
	io_set_dout( 5, state & 0x20 );
	io_set_dout( 6, state & 0x40 );
	io_set_dout( 7, state & 0x80 );
}

void parse_set_all_relays( unsigned char* data )
{
	unsigned char state = 0;

	if( 1 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_ALL_RELAYS invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	state = data[1];

#ifdef PRINTF_DEBUG
	printf( "TAG_SET_ALL_RELAYS 0x%2X\n\r", (int)state );
#endif

	fpga_set_io_rel( 0, state & 0x1 );
	fpga_set_io_rel( 1, state & 0x2 );
}

void parse_set_odometry( unsigned char* data )
{
	float x,y,phi;

	if( 12 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_ODOMETRY invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	decode_float( &data[1], &x );
	decode_float( &data[5], &y );
	decode_float( &data[9], &phi );

#ifdef PRINTF_DEBUG
	printf( "TAG_SET_ODOMETRY %.2f %.2f %.2f\n\r", x, y, RAD2DEG( phi ) );
#endif

	odometry_x = x;
	odometry_y = y;
	odometry_phi = phi;
}

void parse_set_odometry_rotation( unsigned char* data )
{
	float phi;

	if( 4 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_ODOMETRY invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	decode_float( &data[1], &phi );

#ifdef PRINTF_DEBUG
	printf( "TAG_SET_ODOMETRY %.2f\n\r", RAD2DEG( phi ) );
#endif

	odometry_phi = phi;
}

void parse_set_fpga_power( unsigned char* data )
{
	if( 1 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_FPGA_POWER invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	if( 0 == data[1] )
	{
#ifdef PRINTF_DEBUG
		printf( "TAG_SET_FPGA_POWER off\n\r" );
#endif
		fpga_power_off();
	}
	else
	{
#ifdef PRINTF_DEBUG
		printf( "TAG_SET_FPGA_POWER on\n\r" );
#endif
		fpga_power_on();
	}
}

void parse_set_pwr_ok_state( unsigned char* data )
{
	if( 1 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_PWR_OK_STATE invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	if( 0 == data[1] )
	{
#ifdef PRINTF_DEBUG
		printf( "TAG_SET_PWR_OK_STATE low\n\r" );
#endif
		set_power_ok_low();
	}
	else
	{
#ifdef PRINTF_DEBUG
		printf( "TAG_SET_PWR_OK_STATE high\n\r" );
#endif
		set_power_ok_high();
	}
}

void parse_set_pwm( unsigned char* data )
{
	unsigned char channel;
	unsigned char ratio;

	if( 2 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_PWM invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	channel = data[1];
	if( channel < 1 || channel > 6 )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_PWM invalid channel %d", (int)channel );
		produce_error();
		return;
	}

	ratio = data[2];

	pwm_set( channel, ratio );
#ifdef PRINTF_DEBUG
	printf( "TAG_SET_PWM ch:%d ratio:%d\n\r", (int)channel, (int)ratio );
#endif
}

void parse_set_motor_on( unsigned char* data )
{
	unsigned char motor;
	unsigned char on;

	if( 2 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_MOTOR_ON invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	motor = data[1];
	if( motor > 3 )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_MOTOR_ON invalid motor %d", (int)motor );
		produce_error();
		return;
	}

	on = ( data[2] != 0 ? 1 : 0 );

	if( on )
	{
		motor_set_on( motor );
	}
	else
	{
		motor_set_off( motor );
	}

#ifdef PRINTF_DEBUG
	printf( "TAG_SET_MOTOR_ON motor:%d ", (int)motor );
	if( on )
	{
		printf( "On\n\r" );
	}
	else
	{
		printf( "Off\n\r" );
	}
#endif
}

void parse_set_pwrbtn( unsigned char* data )
{
	unsigned char on;

	if( 1 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_PWRBTN invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	on = ( data[1] != 0 ? 1 : 0 );

	if( on )
	{
		io_set_pwrbtn_high();
#ifdef PRINTF_DEBUG
		printf( "TAG_SET_PWRBTN High\n\r" );
#endif
	}
	else
	{
		io_set_pwrbtn_low();
#ifdef PRINTF_DEBUG
		printf( "TAG_SET_PWRBTN Low\n\r" );
#endif
	}
}

void parse_set_sys_reset( unsigned char* data )
{
	unsigned char on;

	if( 1 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_SYS_RESET invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	on = ( data[1] != 0 ? 1 : 0 );

	if( on )
	{
		io_set_sysreset_high();
#ifdef PRINTF_DEBUG
		printf( "TAG_SET_SYS_RESET High\n\r" );
#endif
	}
	else
	{
		io_set_sysreset_low();
#ifdef PRINTF_DEBUG
		printf( "TAG_SET_SYS_RESET Low\n\r" );
#endif
	}
}

void parse_set_ip_address( unsigned char* data )
{
	unsigned int addr;
	unsigned int mask;

	if( 8 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_IP_ADDRESS invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	decode_uint32( data+1, &addr );
	decode_uint32( data+5, &mask );
#ifdef PRINTF_DEBUG
	printf( "TAG_SET_IP_ADDRESS %08X %08X\n\r", addr, mask );
#endif
}

void parse_set_emergency_bumper( unsigned char* data )
{
	if( 1 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_EMERGENCY_BUMPER invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	emergency_bumper = ( data[1] != 0 );
#ifdef PRINTF_DEBUG
	printf( "TAG_SET_EMERGENCY_BUMPER \n\r" );
	if( emergency_bumper )
	{
		printf( "on\n\r" );
	}
	else
	{
		printf( "off\n\r" );
	}
#endif
}

void parse_set_motor_mode( unsigned char* data )
{
	unsigned char motor;

	if( 2 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_MOTOR_MODE invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	motor = data[1];
	if( motor > 3 )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_MOTOR_MODE invalid motor %d", (int)motor );
		produce_error();
		return;
	}

#ifdef PRINTF_DEBUG
	printf( "TAG_SET_MOTOR_MODE %d %02X", (int)motor, (int)data[2] );
#endif

	//motor = motor_mapping[motor];

	snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_MOTOR_MODE not implemented" );
	produce_error();
}

void parse_reset_lpc( unsigned char* data )
{
	unsigned char enterUSBBootloader;

	if( 1 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_RESET_LPC invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	enterUSBBootloader = ( data[1] != 0 ? 1 : 0 );

#ifdef PRINTF_DEBUG
	if( enterUSBBootloader )
	{
		printf( "TAG_RESET_LPC enter USB bootloader\n\r" );
	}
	else
	{
		printf( "TAG_RESET_LPC simple reset\n\r" );
	}
#endif

	resetDevice( enterUSBBootloader );
}

void parse_set_saftey_velocity( unsigned char* data )
{
	float vel;

	if( 4 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_SAFTEY_VELCOITY invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	decode_float( data+1, &vel );

	motor_set_saftey_velocity( vel );

#ifdef PRINTF_DEBUG
	printf( "TAG_SET_SAFTEY_VELCOITY %.2frpm\n\r", vel );
#endif
}

void parse_set_motor_accel_limits( unsigned char* data )
{
	unsigned char motor;
	float min, max;

	if( 9 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_MOTOR_ACCEL_LIMITS invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	motor = data[1];
	if( motor > 3 )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_MOTOR_ACCEL_LIMITS invalid motor %d", (int)motor );
		produce_error();
		return;
	}

	decode_float( data+2, &min );
	decode_float( data+6, &max );

	if( min > 0.0f || max < min )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_MOTOR_ACCEL_LIMITS invalid min %f max %f values", min, max );
		produce_error();
		return;
	}

	motor_set_minaccel( motor, min / 1000.0f );
	motor_set_maxaccel( motor, max / 1000.0f );

#ifdef PRINTF_DEBUG
	printf( "TAG_SET_MOTOR_ACCEL_LIMITS %d %.2frpm/ms %.2frpm/ms\n\r", (int)motor, min / 1000.0f, max / 1000.0f );
#endif
}

void parse_set_battery_min( unsigned char* data )
{
	float pb_min;
	float nimh_min;
	
	if( 8 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_BATTERY_MIN invalid length %d", (int)data[0] );
		produce_error();
		return;
	}
	
	decode_float( data+1, &pb_min );
	decode_float( data+5, &nimh_min );
	
	charger_set_battery_min( pb_min, nimh_min );
	
#ifdef PRINTF_DEBUG
	printf( "TAG_SET_BATTERY_MIN PB:%.2fV NIMH:%.2fV\n\r", pb_min, nimh_min );
#endif
}

void parse_get_motor_accel_limits( unsigned char* data )
{
	unsigned char motor;

	if( 1 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_MOTOR_ACCEL_LIMITS invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	motor = data[1];
	if( motor > 3 )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_MOTOR_ACCEL_LIMITS invalid motor %d", (int)motor );
		produce_error();
		return;
	}

#ifdef PRINTF_DEBUG
	printf( "TAG_GET_MOTOR_ACCEL_LIMITS %d\n\r", (int)motor );
#endif

	produce_motor_accel_limits( motor );
}

void parse_can_msg( unsigned char* data )
{
	unsigned short id;
	unsigned char dlc, canData[8];

	if( 11 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_CAN_MSG invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	decode_uint16( data + 1, &id );
	dlc = data[3];
	memcpy( canData, data + 4, 8 );

#ifdef PRINTF_DEBUG
	printf( "TAG_CAN_MSG\n\r" );
#endif

	if ( id > 0x7FF )
		id = 0x7FF;
	if ( dlc > 8 )
		dlc = 8;

	if( CAN_TxRdy == 0 )
	{
		printf( "CAN busy\n\r" );
	}
	else
	{
		CAN_TxRdy = 0;
		CAN_TxMsg.id = id;
		CAN_TxMsg.len = dlc;
		memcpy( CAN_TxMsg.data, canData, 8 );
		CAN_wrMsg( &CAN_TxMsg );
	}
}

void parser_set_nrst( unsigned char* data )
{
	unsigned char on;

	if( 1 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_NRST invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	on = ( data[1] != 0 ? 1 : 0 );

	if( on )
	{
		io_nrst_high();
#ifdef PRINTF_DEBUG
		printf( "TAG_SET_NRST High\n\r" );
#endif
	}
	else
	{
		io_nrst_low();
#ifdef PRINTF_DEBUG
		printf( "TAG_SET_NRST Low\n\r" );
#endif
	}
}

void parser_set_boot( unsigned char* data )
{
	if( 1 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_BOOT invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	io_set_boot( data[1] );

#ifdef PRINTF_DEBUG
		printf( "TAG_SET_BOOT %d\n\r", (int)data[1] );
#endif
}

void parser_charger_get_version( unsigned char* data )
{
	if( 1 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_CHARGER_GET_VERSION invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	if( data[1] > 2 )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_CHARGER_GET_VERSION invalid charger ID %d", (int)data[1] );
		produce_error();
		return;
	}

	charger_getVersion();

#ifdef PRINTF_DEBUG
		printf( "TAG_CHARGER_GET_VERSION %d\n\r", (int)data[1] );
#endif
}

void parser_charger_clear_error( unsigned char* data )
{
	if( 1 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_CHARGER_CLEAR_ERROR invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	if( data[1] > 2 )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_CHARGER_CLEAR_ERROR invalid charger ID %d", (int)data[1] );
		produce_error();
		return;
	}

	charger_clearError( data[1] );

#ifdef PRINTF_DEBUG
		printf( "TAG_CHARGER_CLEAR_ERROR %d\n\r", (int)data[1] );
#endif
}

void parser_get_gpain( unsigned char* data )
{
	if( 1 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_GET_GPAIN invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	if( data[1] < 1 || data[1] > 10 )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_GET_GPAIN invalid GPAIN %d", (int)data[1] );
		produce_error();
		return;
	}

	produce_gpain( data[1] );

#ifdef PRINTF_DEBUG
		printf( "TAG_GET_GPAIN %d\n\r", (int)data[1] );
#endif
}

void parse_set_motor_debug( unsigned char* data )
{
	if( 1 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_MOTOR_DEBUG invalid length %d", (int)data[0] );
		produce_error();
		return;
	}
	
	if( data[1] > 4 )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_MOTOR_DEBUG invalid motor %d", (int)data[1] );
		produce_error();
		return;
	}
	
	motor_startMonitor(data[1]);
}

void parser( unsigned char* data, int length )
{
	unsigned char tag_length;

	while( length >= 2 )
	{
		tag_length = data[1];

		if( tag_length > length-2 )
		{
			snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "tag_length=%d greater than length=%d", (int)tag_length, length );
			produce_error();
			return;
		}

		//printf( "%x %x\n\r", (int)data[0], (int)data[1] );

		switch( data[0] )
		{
		case TAG_GET_HW_VERSION:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_HW_VERSION\n\r" );
#endif
			produce_hw_version();
			break;

		case TAG_GET_SW_VERSION:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_SW_VERSION\n\r" );
#endif
			produce_sw_version();
			break;

		case TAG_GET_DISTANCE_SENSOR_READINGS:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_DISTANCE_SENSOR_READINGS\n\r" );
#endif
			produce_distance_sensor_readings();
			break;

		case TAG_GET_ADC_PARAMETERS:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_ADC_PARAMETERS\n\r" );
#endif
			produce_adc_parameters();
			break;

		case TAG_SET_MOTOR_SPEED:
			parse_set_motor_speed( &data[1] );
			break;

		case TAG_GET_ALL_MOTOR_SPEEDS:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_ALL_MOTOR_SPEEDS\n\r" );
#endif
			produce_all_motor_speeds();
			break;

		case TAG_SET_MOTOR_POSITION:
			parse_set_motor_position( &data[1] );
			break;

		case TAG_GET_ALL_MOTOR_POSITIONS:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_ALL_MOTOR_POSITIONS\n\r" );
#endif
			produce_all_motor_positions();
			break;

		case TAG_SET_MOTOR_PID_PARAMETERS:
			parse_set_motor_pid_parameters( &data[1] );
			break;

		case TAG_GET_ALL_MOTOR_PID_PARAMETERS:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_ALL_MOTOR_PID_PARAMETERS\n\r" );
#endif
			produce_all_pid_parameters();
			break;

		case TAG_SET_ALL_DIGITAL_OUTPUTS:
			parse_set_all_digital_outputs( &data[1] );
			break;

		case TAG_SET_ALL_RELAYS:
			parse_set_all_relays( &data[1] );
			break;

		case TAG_SET_ODOMETRY:
			parse_set_odometry( &data[1] );
			break;

		case TAG_SET_ODOMETRY_ROTATION:
			parse_set_odometry_rotation( &data[1] );
			break;

		case TAG_GET_ODOMETRY:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_ODOMETRY\n\r" );
#endif
			produce_odometry();
			break;

		case TAG_GET_ALL_MOTOR_CURRENT_READINGS:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_ALL_MOTOR_CURRENT_READINGS\n\r" );
#endif
			produce_all_motor_current_readings();
			break;

		case TAG_GET_ALL_ANALOG_INPUTS:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_ALL_ANALOG_INPUTS\n\r" );
#endif
			produce_all_analog_inputs();
			break;

		case TAG_GET_ALL_DIGITAL_INPUTS:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_ALL_DIGITAL_INPUTS\n\r" );
#endif
			produce_all_digital_inputs();
			break;

		case TAG_GET_BUMPER:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_BUMPER\n\r" );
#endif
			produce_bumper();
			break;

		case TAG_GET_POWER_BUTTON:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_POWER_BUTTON\n\r" );
#endif
			produce_power_button();
			break;

		case TAG_SET_FPGA_POWER:
			parse_set_fpga_power( &data[1] );
			break;

		case TAG_GET_FPGA_POWER:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_FPGA_POWER\n\r" );
#endif
			produce_fpga_power();
			break;

		case TAG_GET_PWR_OK_STATE:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_PWR_OK_STATE\n\r" );
#endif
			produce_pwr_ok_state();
			break;

		case TAG_SET_PWR_OK_STATE:
			parse_set_pwr_ok_state( &data[1] );
			break;

		case TAG_SET_PWM:
			parse_set_pwm( &data[1] );
			break;

		case TAG_SET_MOTOR_ON:
			parse_set_motor_on( &data[1] );
			break;

		case TAG_SET_PWRBTN:
			parse_set_pwrbtn( &data[1] );
			break;

		case TAG_SET_SYS_RESET:
			parse_set_sys_reset( &data[1] );
			break;

		case TAG_GET_COM_EXPRESS_STATES:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_COM_EXPRESS_STATES\n\r" );
#endif
			produce_com_express_states();
			break;

		case TAG_GET_ALL_MOTOR_READINGS:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_ALL_MOTOR_READINGS\n\r" );
#endif
			produce_all_motor_readings();
			break;

		case TAG_GET_IP_ADDRESS:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_IP_ADDRESS\n\r" );
#endif
			produce_ip_address();
			break;

		case TAG_SET_IP_ADDRESS:
			parse_set_ip_address( &data[1] );
			break;

		case TAG_SET_EMERGENCY_BUMPER:
			parse_set_emergency_bumper( &data[1] );
			break;

		case TAG_SET_MOTOR_MODE:
			parse_set_motor_mode( &data[1] );
			break;

		case TAG_RESET_LPC:
			parse_reset_lpc( &data[1] );
			break;

		case TAG_POWER_OFF:
			shutdown();
			break;

		case TAG_SET_SAFTEY_VELOCITY:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_SAFTEY_VELOCITY\n\r" );
#endif
			parse_set_saftey_velocity( &data[1] );
			break;

		case TAG_GET_SAFTEY_VELOCITY:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_SAFTEY_VELOCITY\n\r" );
#endif
			produce_get_saftey_velocity();
			break;

		case TAG_SET_MOTOR_ACCEL_LIMITS:
			parse_set_motor_accel_limits( &data[1] );
			break;

		case TAG_GET_MOTOR_ACCEL_LIMITS:
			parse_get_motor_accel_limits( &data[1] );
			break;

		case TAG_GET_GYRO_Z_ANGLE:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_GYRO_Z_ANGLE\n\r" );
#endif
			break;

		case TAG_CAN_MSG:
#ifdef PRINTF_DEBUG
			printf( "TAG_CAN_MSG\n\r" );
#endif
			parse_can_msg( &data[1] );
			break;

		case TAG_GET_CAN_MSG:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_CAN_MSG\n\r" );
#endif
			produce_can_msg();
			break;

		case TAG_SET_NRST:
			parser_set_nrst( &data[1] );
			break;

		case TAG_GET_NRST:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_NRST\n\r" );
#endif
			produce_get_nrst();
			break;

		case TAG_SET_BOOT:
			parser_set_boot( &data[1] );
			break;

		case TAG_GET_BOOT:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_BOOT\n\r" );
#endif
			produce_get_boot();
			break;

		case TAG_CHARGER_GET_VERSION:
			parser_charger_get_version( &data[1] );
			break;

		case TAG_CHARGER_CLEAR_ERROR:
			parser_charger_clear_error( &data[1] );
			break;
		
		case TAG_SET_BATTERY_MIN:
#ifdef PRINTF_DEBUG
			printf( "TAG_SET_BATTERY_MIN\n\r" );
#endif
			parse_set_battery_min( &data[1] );
		break;
		
		case TAG_GET_BATTERY_MIN:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_BATTERY_MIN\n\r" );
#endif
			produce_battery_min();
			break;
		
		case TAG_GET_GYRODATA:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_GYRODATA\n\r" );
#endif
			break;
		
		case TAG_GET_GPAIN:
			parser_get_gpain( &data[1] );
			break;
		
		case TAG_GET_VERSIONBITS:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_VERSIONBITS\n\r" );
#endif
			produce_versionbits();
			break;
		
		case TAG_GYRO_SET_PARAM:
				break;
			
		case TAG_GYRO_GET_PARAM:
#ifdef PRINTF_DEBUG
			printf( "TAG_GYRO_GET_PARAM\n\r" );
#endif
			break;
		
		case TAG_FESTOOL_CHARGER_HOTSWAP_START:
			charger_startHotSwap();
			break;
		
		case TAG_SET_MOTOR_DEBUG:
			parse_set_motor_debug( &data[1] );
			break;

		default:
			snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "Tag %d not implemented", (int)data[0] );
			produce_error();
			break;
		}

		data += tag_length+2;
		length -= tag_length+2;
	}
}
