#ifndef _CHARGER_H_
#define _CHARGER_H_

#define MAX_NUM_CHARGERS 3

typedef struct
{
	unsigned int time;
	float batteryVoltage;
	float current;
	float battery1temp;
	float battery2temp;
	unsigned char state;
} pb_charger_info_st;

typedef struct
{
	unsigned int time;
	unsigned char error;
	unsigned char type;
	unsigned char data[4];
} pb_charger_error_st;

typedef struct
{
	unsigned int time;
	unsigned char akku_sel[4];
	unsigned char akku_load[4];
	unsigned char ext_power;
	unsigned char charger_present;
	unsigned char mos_on_batt1;
	unsigned char akku_on[4];
	unsigned short batteryVoltage[4]; //in mV
	unsigned char batteryCapacity[4];
	unsigned char isBatteryLow;
} festool_charger_info_st;

typedef struct
{
	unsigned char seq;
	unsigned char major;
	unsigned char minor;
	unsigned char patch;
} charger_version_st;

extern pb_charger_info_st pb_charger_info_array[MAX_NUM_CHARGERS];
extern pb_charger_error_st pb_charger_error_array[MAX_NUM_CHARGERS];
extern festool_charger_info_st festool_charger_info;
extern charger_version_st charger_version_array[MAX_NUM_CHARGERS];

enum
{
	CHARGER_STATE_NORMAL,
	CHARGER_STATE_STARTUP,
	CHARGER_STATE_PROGRAM,
	CHARGER_STATE_COUNT
};

extern unsigned char charger_state;

void charger_getVersion(void);
void charger_startHotSwap(void);
void charger_program(unsigned char chargerID );
void charger_clearError( unsigned char chargerID );

const char* pb_charger_state( unsigned char chargerID );

unsigned char charger_is_battery_low( void );
unsigned int charger_battery_low_count( void );
float charger_max_batteryVoltage( void );
const char* charger_error_message( unsigned int chargerID );
void charger_set_battery_min( float pb_min, float nimh_min );
float charger_min_battery_voltage( void );

#endif //_CHARGER_H_
