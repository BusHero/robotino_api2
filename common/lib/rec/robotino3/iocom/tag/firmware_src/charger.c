#include <LPC23xx.H> /* LPC23xx definitions */
#include "charger.h"
#include "CAN.h"
#include "led.h"
#include "util.h"
#include <stdio.h>
#include <string.h>

unsigned char charger_state = CHARGER_STATE_STARTUP;

const char* charger_internal_state_string[CHARGER_STATE_COUNT] = {
	"Normal",
	"Startup",
	"Program"
};

const float NIMH_LOW_VOLTAGE=19.8f;
const float PB_LOW_VOLTAGE=19.0f;

#define BATTERY_LOW_COUNTDOWN 60000 //1min

extern const unsigned char charger_version[];
extern const unsigned char festool_charger_version[];

unsigned char chargerNeedsUpdate[3] = {0,0,0};

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

void charger_print_state( int state );

extern unsigned int globalTime( void );

pb_charger_info_st pb_charger_info_array[3] = { {0,}, {0,}, {0,} };
pb_charger_error_st pb_charger_error_array[3] = { {0,}, {0,}, {0,} };
festool_charger_info_st festool_charger_info = {0,{0,0,0,0},{0,0,0,0},0,0,0,{0,0,0,0},{0,0,0,0},{0,0,0,0},0};
charger_version_st charger_version_array[3] = { {0,0,0,0}, {0,0,0,0}, {0,0,0,0} }; 

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
	
	if( chargerID >= MAX_NUM_CHARGERS )
	{
		return 0;
	}

	error_type = pb_charger_error_array[chargerID].error;

	switch( error_type )
	{
	case SYSTEM_ERROR:
		{
			unsigned char charger_system_error_type = pb_charger_error_array[chargerID].type;
			unsigned char charger_state = pb_charger_error_array[chargerID].data[0];

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
			unsigned char charger_error_type = pb_charger_error_array[chargerID].type;
			switch( charger_error_type )
			{
			case TIMEOUT_ERROR:
				{
					unsigned char charger_state = pb_charger_error_array[chargerID].data[0];
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
					float batteryVoltage = pb_charger_error_array[chargerID].data[0];
					batteryVoltage += (pb_charger_error_array[chargerID].data[1] << 8);
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
					
					ntc1 = pb_charger_error_array[chargerID].data[0];
					ntc1 |= (pb_charger_error_array[chargerID].data[1] << 8);

					ntc2 = pb_charger_error_array[chargerID].data[2];
					ntc2 |= (pb_charger_error_array[chargerID].data[3] << 8);

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
					
					bat1 = pb_charger_error_array[chargerID].data[0];
					bat1 += (pb_charger_error_array[chargerID].data[1] << 8);
					bat1 /= 1000;

					bat2 = pb_charger_error_array[chargerID].data[2];
					bat2 += (pb_charger_error_array[chargerID].data[3] << 8);
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
					
					current = pb_charger_error_array[chargerID].data[0];
					current += (pb_charger_error_array[chargerID].data[1] << 8);
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
	CAN_setup(125000);

	printf( "CAN start\n\r" );
	CAN_start();

	printf( "CAN wait ready\n\r" );
	CAN_waitReady();
	
	charger_isp_init();
}

void charger_reset(void)
{
	io_nrst_low();
	wait_micro( 1 );
	io_nrst_high();
}

void charger_program(unsigned char chargerID )
{
	const unsigned int ispWDTC = 10000000; //10000ms
	unsigned int oldWDTC = WDTC;
	
	printf("Set watchdog to %ums\r\n",ispWDTC/1000);
	WDTC = ispWDTC;
	
	led_set_pattern( LED_FAST_RECT );
	
	charger_isp_program( chargerID );

	led_set_pattern( LED_UP );

	io_clear_boot();
	
	printf("Set watchdog to %ums\r\n",oldWDTC/1000);
	WDTC = oldWDTC;
	watchdog_feed();
}

const char* pb_charger_state( unsigned char chargerID )
{
	unsigned char state = 0;
	if( chargerID >= 3 )
	{
		return charger_unkown_state_string;
	}

	state = pb_charger_info_array[chargerID].state;
	
	if( state >= charger_state_string_count )
	{
		state = charger_state_string_count-1;
	}
	return charger_state_string[state];
}

void charger_getVersion(void)
{
	unsigned char i;
	
	CAN_TxMsg.format = STANDARD_FORMAT;
	CAN_TxMsg.type = DATA_FRAME;

	CAN_TxMsg.len = 8;
	CAN_TxMsg.data[0] = 0;
	CAN_TxMsg.data[1] = 0;
	CAN_TxMsg.data[2] = 0;
	CAN_TxMsg.data[3] = 0;
	CAN_TxMsg.data[4] = 0;
	CAN_TxMsg.data[5] = 0;
	CAN_TxMsg.data[6] = 0;
	CAN_TxMsg.data[7] = 0;

	for(i=0;i<3;++i)
	{
		CAN_TxMsg.id = 0x511 + i*0x20;
		CAN_wrMsg( &CAN_TxMsg );
	}
}

void charger_clearError( unsigned char chargerID )
{
	CAN_TxMsg.id = 0x512 + chargerID*0x20;
	CAN_TxMsg.format = STANDARD_FORMAT;
	CAN_TxMsg.type = DATA_FRAME;

	CAN_TxMsg.len = 8;
	CAN_TxMsg.data[0] = 0xAA;
	CAN_TxMsg.data[1] = 0x55;
	CAN_TxMsg.data[2] = 0;
	CAN_TxMsg.data[3] = 0;
	CAN_TxMsg.data[4] = 0;
	CAN_TxMsg.data[5] = 0;
	CAN_TxMsg.data[6] = 0;
	CAN_TxMsg.data[7] = 0;

	CAN_wrMsg( &CAN_TxMsg );
}

void charger_startHotSwap(void)
{
	CAN_TxMsg.id = 0x611;
	CAN_TxMsg.format = STANDARD_FORMAT;
	CAN_TxMsg.type = DATA_FRAME;

	CAN_TxMsg.len = 8;
	CAN_TxMsg.data[0] = 0;
	CAN_TxMsg.data[1] = 0;
	CAN_TxMsg.data[2] = 0;
	CAN_TxMsg.data[3] = 0;
	CAN_TxMsg.data[4] = 0;
	CAN_TxMsg.data[5] = 0;
	CAN_TxMsg.data[6] = 0;
	CAN_TxMsg.data[7] = 0;
	
	CAN_wrMsg( &CAN_TxMsg );
}

void charger_transit(int newState)
{
	unsigned char i;
	static unsigned char startup_count = 0;
	
	switch(newState)
	{
		case CHARGER_STATE_STARTUP:
			if(startup_count > 2)
			{
				newState = CHARGER_STATE_NORMAL;
			}
			else
			{
				startup_count++;
				
				for(i=0;i<MAX_NUM_CHARGERS;++i)
				{
					charger_version_array[i].major = 0;
					chargerNeedsUpdate[i] = 0;
				}
			}
			break;
		
		default:
			break;
	}
	
	printf("\r\nCharger transit %s -> %s\r\n", charger_internal_state_string[charger_state],charger_internal_state_string[newState]);
	charger_state = newState;
}

void charger_update( void )
{
	static unsigned int prevTime = 0;
	static unsigned int step = 0;
	
	int nextState;
	int j;
	int i;
	int chargerid;
	pb_charger_info_st* p_pb_charger_info;
	pb_charger_error_st* p_pb_charger_error;
	charger_version_st* p_charger_version;
	
	CAN_msg dst;
	
	const unsigned int currentTime = globalTime();
	
	static const unsigned int stepMs = 100;
	
	if(currentTime > prevTime && currentTime - prevTime < stepMs)
	{
		return;
	}
	
	prevTime = currentTime;
	
	if(CAN_error())
	{
		CAN_clear_error();
		return;
	}
	
	while(getCanMsg(&dst)>-1)
	{
		switch( dst.id )
		{
		case 0x501: //PB-charger info message
		case 0x521:
		case 0x541:
			chargerid=(dst.id-0x501) >> 5;
		  if(chargerid<3)
			{
				p_pb_charger_info = &pb_charger_info_array[chargerid];
				
				p_pb_charger_info->time = dst.data[0];
				p_pb_charger_info->time |= dst.data[1] << 8;
				
				p_pb_charger_info->batteryVoltage = dst.data[2];
				p_pb_charger_info->batteryVoltage += (dst.data[3] << 8);
				p_pb_charger_info->batteryVoltage /= 1000;
				
				p_pb_charger_info->current = dst.data[4];
				p_pb_charger_info->current /= 100;
				
				p_pb_charger_info->battery1temp = dst.data[5];
				p_pb_charger_info->battery1temp = dst.data[6];
				
				p_pb_charger_info->state = dst.data[7];
			}
			break;

		case 0x502:
		case 0x522:
		case 0x542:
			chargerid=(dst.id-0x502) >> 5;
			break;

		case 0x503:
		case 0x523:
		case 0x543:
			chargerid=(dst.id-0x503) >> 5;
			if(chargerid<3)
			{
				p_charger_version = &charger_version_array[chargerid];
				
				p_charger_version->seq++;
				p_charger_version->major = dst.data[0];
				p_charger_version->minor = dst.data[1];
				p_charger_version->patch = dst.data[2];
			}
			break;

		case 0x504:
		case 0x524:
		case 0x544:
			//Pb charger error
			chargerid=(dst.id-0x504) >> 5;
			if(chargerid<3)
				{
					p_pb_charger_error = &pb_charger_error_array[chargerid];
					
					p_pb_charger_error->time = dst.data[0];
					p_pb_charger_error->time |= dst.data[1] << 8;
					
					p_pb_charger_error->error = dst.data[2];
					p_pb_charger_error->type = dst.data[3];
					
					memcpy(&p_pb_charger_error->data[0],&dst.data[4],4);
				}
			break;
		
		case 0x601:
			festool_charger_info.time = dst.data[0];
			festool_charger_info.time |= dst.data[1] << 8;
		
			for(i=0;i<4;i++)
			{
				festool_charger_info.akku_sel[i] = 0;
			}
			
			if(dst.data[2] & 0x1) //akku_sel_enabled
			{
				festool_charger_info.akku_sel[((dst.data[2] >> 1) & 0x3)] = 1;
			}
			
			for(i=0;i<4;i++)
			{
				festool_charger_info.akku_load[i] = ((dst.data[2] >> (3+i)) & 0x1);
			}
		
			festool_charger_info.ext_power = (dst.data[3] & 0x1);
			festool_charger_info.charger_present = ((dst.data[3] >> 1) & 0x1);
			festool_charger_info.mos_on_batt1 = ((dst.data[3] >> 2) & 0x1);
			
			for(i=0;i<4;i++)
			{
				festool_charger_info.akku_on[i] = ((dst.data[3] >> (3+i)) & 0x1);
			}
			
			festool_charger_info.isBatteryLow = ((dst.data[3] >> 7) & 0x1);
		
			for(i=0;i<4;i++)
			{
				festool_charger_info.batteryCapacity[i] = dst.data[4+i];
			}
			break;
		
		case 0x602:
			for(i=0;i<4;i++)
			{
				festool_charger_info.batteryVoltage[i] = dst.data[2*i];
				festool_charger_info.batteryVoltage[i] |= (dst.data[2*i+1] << 8);
			}
			break;

		default:                        
			break;
		}
	}
	
	switch(charger_state)
	{
		case CHARGER_STATE_NORMAL:			
			_charger_max_batteryVoltage = 0.0f;
	
			if( 2 == charger_version_array[0].major )
			{
				for(i=0;i<4;++i)
				{
					if(festool_charger_info.akku_load[i])
					{
						continue;
					}
					
					if( festool_charger_info.batteryVoltage[i] > _charger_max_batteryVoltage )
					{
						_charger_max_batteryVoltage = 0.001f*festool_charger_info.batteryVoltage[i];
					}
				}
			}
			else
			{
				for( i=0; i<MAX_NUM_CHARGERS; ++i )
				{
					float v = pb_charger_info_array[i].batteryVoltage;
					if( v > _charger_max_batteryVoltage )
					{
						_charger_max_batteryVoltage = v;
					}
				}
			}
			break;
		
		case CHARGER_STATE_STARTUP:
			printf(".");
			if(step>100)
			{
				step=0;
				charger_transit(CHARGER_STATE_PROGRAM);
			}
			else
			{		
				for(i=0;i<MAX_NUM_CHARGERS;++i)
				{
					chargerNeedsUpdate[i] = 0;
					
					if(io_charger_exists(i+1))
					{
						//printf("Charger %d version %d.%d.%d\r\n",i+1,(int)charger_version_array[i].major,(int)charger_version_array[i].minor,(int)charger_version_array[i].patch);
						
						if(0 == charger_version_array[i].major)
						{
							chargerNeedsUpdate[i] = 1;
						}
						else if(1 == charger_version_array[i].major)
						{
							if( charger_version[0] == charger_version_array[i].major )
							{
								if( charger_version[1] > charger_version_array[i].minor )
								{
									chargerNeedsUpdate[i] = 1;
								}
								else if( charger_version[1] == charger_version_array[i].minor )
								{
									if( charger_version[2] > charger_version_array[i].patch )
									{
										chargerNeedsUpdate[i] = 1;
									}
								}
							}
						}
						else if(2 == charger_version_array[i].major ) //Festool charger
						{
							if( festool_charger_version[0] == charger_version_array[i].major )
							{
								if( festool_charger_version[1] > charger_version_array[i].minor )
								{
									chargerNeedsUpdate[i] = 1;
								}
								else if( festool_charger_version[1] == charger_version_array[i].minor )
								{
									if( festool_charger_version[2] > charger_version_array[i].patch )
									{
										chargerNeedsUpdate[i] = 1;
									}
								}
							}
						}
						else //not supported
						{
						}
					}
				}
				
				j=0;
				for(i=0;i<MAX_NUM_CHARGERS;++i)
				{
					if(charger_version_array[i].major > 0)
					{
						++j;
					}
				}
				
				if(j==io_num_charger())
				{
					step=0;
					charger_transit(CHARGER_STATE_PROGRAM);
				}
				else
				{
					if(50 == step)
					{
						charger_reset();
					}
					charger_getVersion();
				}
			}
			break;
		
		case CHARGER_STATE_PROGRAM:
			nextState = CHARGER_STATE_NORMAL;
			for(i=0;i<MAX_NUM_CHARGERS;++i)
			{
				if(chargerNeedsUpdate[i])
				{
					printf("Charger %d needs update from %d.%d.%d -> ",i+1,(int)charger_version_array[i].major,(int)charger_version_array[i].minor,(int)charger_version_array[i].patch);
					if(0 == charger_version_array[i].major)
					{
						printf("%d.%d.%d or %d.%d.%d\r\n",(int)charger_version[0], (int)charger_version[1], (int)charger_version[2],(int)festool_charger_version[0], (int)festool_charger_version[1], (int)festool_charger_version[2]);
					}
					else if(1 == charger_version_array[i].major)
					{
						printf("%d.%d.%d\r\n",(int)charger_version[0], (int)charger_version[1], (int)charger_version[2]);
					}
					else if(2 == charger_version_array[i].major)
					{
						printf("%d.%d.%d\r\n",(int)festool_charger_version[0], (int)festool_charger_version[1], (int)festool_charger_version[2]);
					}
					else
					{
						printf("error\r\n");
					}
					
					charger_program(i+1);
					nextState = CHARGER_STATE_STARTUP;
				}
				else
				{
					if(io_charger_exists(i+1))
					{
						printf("Charger %d up to date version %d.%d.%d\r\n",i+1,(int)charger_version_array[i].major,(int)charger_version_array[i].minor,(int)charger_version_array[i].patch);
					}
					else
					{
						printf("Charger %d not connected\r\n",i+1);
					}
				}
			}
			
			step=0;
			charger_transit(nextState);
			break;
		
		default:
			break;
	}
	
	++step;
}

unsigned char charger_is_battery_low( void )
{
	if( io_is_ext_power() )
	{
		return 0;
	}
	
	if( 2 == charger_version_array[0].major )
	{
		return festool_charger_info.isBatteryLow;
	}
	else
	{
		return ( _charger_max_batteryVoltage > 5.0f && (_charger_max_batteryVoltage < _charger_min_batteryVoltage) );
	}
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
