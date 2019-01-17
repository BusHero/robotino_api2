#include <LPC23xx.H>                             /* LPC23xx definitions */
#include "CAN.h"
#include "led.h"
#include <stdio.h>
#include <string.h>

const float NIMH_LOW_VOLTAGE=19.8f;
const float PB_LOW_VOLTAGE=19.0f;

#define BATTERY_LOW_COUNTDOWN 60000 //1min

extern const unsigned char charger_version[];

extern void produce_can_msg( void );

extern unsigned char io_charger_exists( unsigned char number );
extern void io_clear_boot( void );
extern unsigned char io_num_charger( void );
extern unsigned char io_is_ext_power( void );
extern void io_nrst_high( void );
extern void io_nrst_low( void );

extern void wait_micro( unsigned int microsec );

extern void charger_isp_init( void );
extern int charger_isp_program ( int number );

extern void wait_ms( unsigned int msecs );
extern void watchdog_feed( void );

extern void chargerTimer_start( void );
extern unsigned int chargerTimer_elapsed( void );

float _charger_max_batteryVoltage = 0.0f;
float _charger_min_batteryVoltage = PB_LOW_VOLTAGE;

unsigned char _charger_battery_low_counter_started = 0;

void charger_program( void );
void charger_print_state( int state );

#define charger_state_string_count 13
const char* charger_state_string[charger_state_string_count] = {
			"None"
			,"Idle"
			,"Monitor"
			,"CheckBattery"
			,"No battery"
			,"NTC Error"
			,"Start"
			,"Stage1"
			,"Stage2"
			,"Stage3"
			,"Stage4"
			,"Error"
			,"Unknown"
};

//Error types
enum
{
	SYSTEM_ERROR = 0,
	CHARGER_ERROR
};

#define charger_error_type_string_count 2
const char* charger_error_type_string[charger_error_type_string_count] = {
	"system",
	"charger"
};

//Charger error types
enum
{
	TIMEOUT_ERROR = 0,
	MISSING_C10_ERROR,
	OVERVOLTAGE_ERROR,
	NTC_MISMATCH_ERROR,
	BAT1_OVERTEMP_ERROR,
	BAT2_OVERTEMP_ERROR,
	UNDERVOLTAGE_ERROR,
	STAGE2_NTC_ERROR,
	STAGE2_CURRENTHIGH_ERROR,
	STAGE3_NTC_ERROR,
	STAGE3_UNDERVOLTAGE_ERROR,
	STAGE3_OVERVOLTAGE_ERROR
};

#define charger_charger_error_type_string_count 12
const char* charger_charger_error_type_string[charger_charger_error_type_string_count] = {
	"timeout",
	"missing c10",
	"overvoltage",
	"NTC mismatch",
	"bat1 overtemp",
	"bat2 overtemp",
	"undervoltage",
	"stage2 NTC",
	"stage2 current high",
	"stage3 NTC",
	"stage3 undervoltage",
	"stage3 overvoltage"
};

//System error types
enum
{
	UNKNOWN_ERROR = 0,
	CAN_TX_ERROR,
	CAN_RX_ERROR,
	UNKNOWN_CHARGER_STATE_ERROR
};

#define charger_system_error_type_string_count 4
const char* charger_system_error_type_string[charger_system_error_type_string_count] = {
	"Unknown",
	"CAN TX",
	"CAN RX",
	"Unknown state"
};

#define _charger_error_message_buffer_count 128
char _charger_error_message_buffer[_charger_error_message_buffer_count];

const char* charger_unkown_state_string = "Unknown";

const char* charger_error_message( unsigned int chargerID )
{
	unsigned char error_type;
	
	if( chargerID >= 3 )
	{
		return 0;
	}

	error_type = CAN_charger_error[chargerID].data[2];

	switch( error_type )
	{
	case SYSTEM_ERROR:
		{
			unsigned char charger_system_error_type = CAN_charger_error[chargerID].data[3];
			unsigned char charger_state = CAN_charger_error[chargerID].data[4];

			if( charger_system_error_type >= charger_system_error_type_string_count )
			{
				charger_system_error_type = 0;
			}

			if( charger_state >= charger_state_string_count )
			{
				charger_state = charger_state_string_count-1;
			}

			snprintf( _charger_error_message_buffer
				, _charger_error_message_buffer_count
				, "%s %s in state %s"
				, charger_error_type_string[error_type]
				, charger_system_error_type_string[charger_system_error_type]
				, charger_state_string[charger_state]
			);
		}
		break;

	case CHARGER_ERROR:
		{
			unsigned char charger_error_type = CAN_charger_error[chargerID].data[3];
			switch( charger_error_type )
			{
			case TIMEOUT_ERROR:
				{
					unsigned char charger_state = CAN_charger_error[chargerID].data[4];
					if( charger_state >= charger_state_string_count )
					{
						charger_state = charger_state_string_count-1;
					}

					snprintf( _charger_error_message_buffer
						, _charger_error_message_buffer_count
						, "%s %s in state %s"
						, charger_error_type_string[error_type]
						, charger_charger_error_type_string[charger_error_type]
						, charger_state_string[charger_state]
					);
				}
				break;

			case OVERVOLTAGE_ERROR:
			case UNDERVOLTAGE_ERROR:
			case MISSING_C10_ERROR:
			case STAGE3_UNDERVOLTAGE_ERROR:
			case STAGE3_OVERVOLTAGE_ERROR:
				{
					float batteryVoltage = CAN_charger_error[chargerID].data[4];
					batteryVoltage += (CAN_charger_error[chargerID].data[5] << 8);
					batteryVoltage /= 1000;

					snprintf( _charger_error_message_buffer
						, _charger_error_message_buffer_count
						, "%s %s %.2fV"
						, charger_error_type_string[error_type]
						, charger_charger_error_type_string[charger_error_type]
						, batteryVoltage
					);
				}
				break;

			case NTC_MISMATCH_ERROR:
			case STAGE2_NTC_ERROR:
			case STAGE3_NTC_ERROR:
				{
					unsigned short ntc1, ntc2;
					
					ntc1 = CAN_charger_error[chargerID].data[4];
					ntc1 |= (CAN_charger_error[chargerID].data[5] << 8);

					ntc2 = CAN_charger_error[chargerID].data[6];
					ntc2 |= (CAN_charger_error[chargerID].data[7] << 8);

					snprintf( _charger_error_message_buffer
						, _charger_error_message_buffer_count
						, "%s %s %u %u"
						, charger_error_type_string[error_type]
						, charger_charger_error_type_string[charger_error_type]
						, ntc1
						, ntc2
					);
				}
				break;

			case BAT1_OVERTEMP_ERROR:
			case BAT2_OVERTEMP_ERROR:
				{
					float bat1, bat2;
					
					bat1 = CAN_charger_error[chargerID].data[4];
					bat1 += (CAN_charger_error[chargerID].data[5] << 8);
					bat1 /= 1000;

					bat2 = CAN_charger_error[chargerID].data[6];
					bat2 += (CAN_charger_error[chargerID].data[7] << 8);
					bat2 /= 1000;

					snprintf( _charger_error_message_buffer
						, _charger_error_message_buffer_count
						, "%s %s %.2fdeg %.2fdeg"
						, charger_error_type_string[error_type]
						, charger_charger_error_type_string[charger_error_type]
						, bat1
						, bat2
					);
				}
				break;
				
				case STAGE2_CURRENTHIGH_ERROR:
				{
					float current;
					
					current = CAN_charger_error[chargerID].data[4];
					current += (CAN_charger_error[chargerID].data[5] << 8);
					current /= 1000;
					
					snprintf( _charger_error_message_buffer
						, _charger_error_message_buffer_count
						, "%s %s %.2fA"
						, charger_error_type_string[error_type]
						, charger_charger_error_type_string[charger_error_type]
						, current
					);
				}
					break;

			default:
				break;
			}
		}
		break;

	default:
		return charger_unkown_state_string;
	}

	return _charger_error_message_buffer;
}

void charger_set_battery_min( float pb_min, float nimh_min )
{
	if( io_num_charger() > 1 )
	{
		_charger_min_batteryVoltage = nimh_min;
	}
	else
	{
		_charger_min_batteryVoltage = pb_min;
	}
	printf("Set battery low voltage %.2fV\n\r", _charger_min_batteryVoltage );
}

float charger_min_battery_voltage( void )
{
	return _charger_min_batteryVoltage;
}

void charger_init( void )
{
	printf( "CAN setup\n\r" );
	CAN_setup(1, 125000);

	printf( "CAN start\n\r" );
	CAN_start(1);

	printf( "CAN wait ready\n\r" );
	CAN_waitReady(1);

	if( 0 == io_is_ext_power() )
	{
		io_nrst_low();
		wait_micro( 1 );
		io_nrst_high();
	}

	charger_program();
}

void charger_program( void )
{
	int i,j;
	unsigned char doRequestVersionAfterUpdate=0;

	charger_isp_init();

	for( i=0; i<3; ++i )
	{
		unsigned char* major = &CAN_charger_version[i].data[0];
		unsigned char* minor = &CAN_charger_version[i].data[1];
		unsigned char* patch = &CAN_charger_version[i].data[2];

		unsigned char doUpdate = 0;

		if( io_charger_exists( i+1 ) )
		{
			printf("Found charger %d with version ", i+1);

			CAN_writeGetVersion( i );
			for( j=0; j<50; ++j )
			{
				if( CAN_charger_version_count[i] > 0 )
				{
					break;
				}
				else
				{
					printf(".");
					watchdog_feed();
					wait_ms( 100 );
				}
			}

			printf(" %d.%d.%d\n\r", (int)*major, (int)*minor, (int)*patch );

			if( charger_version[0] > *major )
			{
				doUpdate = 1;
			}
			else if( charger_version[0] == *major )
			{
				if( charger_version[1] > *minor )
				{
					doUpdate = 1;
				}
				else if( charger_version[1] == *minor )
				{
					if( charger_version[2] > *patch )
					{
						doUpdate = 1;
					}
				}
			}
		}
		else
		{
			printf("Charger %d not found\n\r", i+1);
			continue;
		}

		if( doUpdate )
		{
			led_set_pattern( LED_FAST_RECT );
			printf("Updating charger from version %d.%d.%d to %d.%d.%d\n\r", (int)*major, (int)*minor, (int)*patch, (int)charger_version[0], (int)charger_version[1], (int)charger_version[2] );
			charger_isp_program( i+1 );
			doRequestVersionAfterUpdate = 1;
		}
		else
		{
			printf("Charger %d up to date\n\r", i+1);
		}
	}

	io_clear_boot();

	if( doRequestVersionAfterUpdate )
	{
		printf("Waiting for charger to boot ");
		for( i=0; i<10; ++i )
		{
			wait_ms( 200 );
			watchdog_feed();
			printf( "." );
		}
		printf(" Done\n\r");

		for( i=0; i<3; ++i )
		{
			if( io_charger_exists( i+1 ) )
			{
				CAN_writeGetVersion( i );
			}
		}
	}

	charger_set_battery_min( PB_LOW_VOLTAGE , NIMH_LOW_VOLTAGE );

	led_set_pattern( LED_UP );
}

unsigned int charger_time( unsigned char chargerID )
{
	unsigned int time;

	if( chargerID >= 3 )
	{
		return 0;
	}

	time = CAN_charger_info[chargerID].data[0];
	time |= CAN_charger_info[chargerID].data[1] << 8;

	return time;
}

unsigned char charger_state_number( unsigned char chargerID )
{
	if( chargerID >= 3 )
	{
		return 0;
	}

	return CAN_charger_info[chargerID].data[7];
}

const char* charger_state( unsigned char chargerID )
{
	unsigned char state = 0;
	if( chargerID >= 3 )
	{
		return charger_unkown_state_string;
	}

	state = charger_state_number( chargerID );

	if( state >= charger_state_string_count )
	{
		state = charger_state_string_count-1;
	}
	return charger_state_string[state];
}

float charger_batteryVoltage( unsigned char chargerID )
{
	float voltage;
	if( chargerID >= 3 )
	{
		return 0.0f;
	}

	voltage = CAN_charger_info[chargerID].data[2];
	voltage += (CAN_charger_info[chargerID].data[3] << 8);
	voltage /= 1000;

	return voltage;
}

float charger_current( unsigned char chargerID )
{
	float current;
	if( chargerID >= 3 )
	{
		return 0.0f;
	}

	current = CAN_charger_info[chargerID].data[4];
	current /= 100;

	return current;
}

float charger_bat1temp( unsigned char chargerID )
{
	float temp;
	if( chargerID >= 3 )
	{
		return 0.0f;
	}

	temp = CAN_charger_info[chargerID].data[5];

	return temp;
}

float charger_bat2temp( unsigned char chargerID )
{
	float temp;
	if( chargerID >= 3 )
	{
		return 0.0f;
	}

	temp = CAN_charger_info[chargerID].data[6];

	return temp;
}

void charger_get_version( unsigned char chargerID, unsigned char* major, unsigned char* minor, unsigned char* patch )
{
	if( chargerID >= 3 )
	{
		return;
	}

	*major = CAN_charger_version[chargerID].data[0];
	*minor = CAN_charger_version[chargerID].data[1];
	*patch = CAN_charger_version[chargerID].data[2];
}

unsigned int charger_error_time( unsigned char chargerID )
{
	unsigned int time;

	if( chargerID >= 3 )
	{
		return 0;
	}

	time = CAN_charger_error[chargerID].data[0];
	time |= CAN_charger_error[chargerID].data[1] << 8;

	return time;
}

void charger_update( void )
{
	static unsigned int info_count[3] = {0,0,0};
	static unsigned int error_count[3] = {0,0,0};
	static unsigned int version_count[3] = {0,0,0};

	int i;
	for( i=0; i<3; ++i )
	{
		if( CAN_charger_info_count[i] > info_count[i] )
		{
			info_count[i] = CAN_charger_info_count[i];
			//CAN_printMsg( &CAN_charger_info[i] );
			//printf("CAN received Charger %d info\n\r", i+1);
			//printf( "  Time: %u\n\r", charger_time(i) );
			//printf( "  Voltage: %.2f\n\r", charger_batteryVoltage(i) );
			//printf( "  Current: %.1f\n\r", charger_current(i) );
			//printf( "  Temp1: %.1f\n\r", charger_bat1temp(i) );
			//printf( "  Temp2: %.1f\n\r", charger_bat2temp(i) );
			//printf( "  State: " );
			//printf( charger_state( i ) );
			//printf( "\n\r" );

			_charger_max_batteryVoltage = 0.0f;
		}

		if( CAN_charger_error_count[i] > error_count[i] )
		{
			error_count[i] = CAN_charger_error_count[i];
			printf( charger_error_message(i) );
			printf("\n\r");
		}

		if( CAN_charger_version_count[i] > version_count[i] )
		{
			version_count[i] = CAN_charger_version_count[i];

			printf("CAN received Charger %d version: %d.%d.%d\n\r", i+1, (int)CAN_charger_version[i].data[0], (int)CAN_charger_version[i].data[1], (int)CAN_charger_version[i].data[2] );
			//CAN_printMsg( &CAN_charger_version[i] );
		}
	}

	if( 0 == _charger_max_batteryVoltage )
	{
		for( i=0; i<3; ++i )
		{
			float v = charger_batteryVoltage( i );
			if( v > _charger_max_batteryVoltage )
			{
				_charger_max_batteryVoltage = v;
			}
		}
	}
}

unsigned char charger_is_battery_low( void )
{
	if( io_is_ext_power() )
	{
		return 0;
	}

	return ( _charger_max_batteryVoltage > 5.0f && (_charger_max_batteryVoltage < _charger_min_batteryVoltage) );
}

unsigned int charger_battery_low_count( void )
{
	unsigned int ms;
	if( charger_is_battery_low() )
	{
		if( 0 == _charger_battery_low_counter_started )
		{
			printf("Start battery low counter\n\r");
			_charger_battery_low_counter_started = 1;
			chargerTimer_start();
		}

		ms = chargerTimer_elapsed();

		if( ms < BATTERY_LOW_COUNTDOWN )
		{
			return BATTERY_LOW_COUNTDOWN - ms;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		_charger_battery_low_counter_started = 0;
		return BATTERY_LOW_COUNTDOWN;
	}
}

float charger_max_batteryVoltage( void )
{
	return _charger_max_batteryVoltage;
}
