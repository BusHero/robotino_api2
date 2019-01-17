#ifndef _CHARGER_H_
#define _CHARGER_H_

unsigned int charger_time( unsigned char chargerID );
const char* charger_state( unsigned char chargerID );
unsigned char charger_state_number( unsigned char chargerID );
float charger_batteryVoltage( unsigned char chargerID );
float charger_current( unsigned char chargerID );
float charger_bat1temp( unsigned char chargerID );
float charger_bat2temp( unsigned char chargerID );
void charger_get_version( unsigned char chargerID, unsigned char* major, unsigned char* minor, unsigned char* patch );
unsigned int charger_error_time( unsigned char chargerID );
unsigned char charger_is_battery_low( void );
unsigned int charger_battery_low_count( void );
float charger_max_batteryVoltage( void );
const char* charger_error_message( unsigned int chargerID );
void charger_set_battery_min( float pb_min, float nimh_min );
float charger_min_battery_voltage( void );

#endif //_CHARGER_H_
