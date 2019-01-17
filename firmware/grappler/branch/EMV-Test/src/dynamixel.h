#ifndef _DYNAMIXEL_H_
#define _DYNAMIXEL_H_

#define NUMSERVOS 9
#define DYNAMIXEL_TIMEOUT 6
#define DYNAMIXEL_TX_TIMEOUT 3
#define NUMCHANNELS 3

unsigned char dynamixel_txchecksum( unsigned char* buffer );

void dynamixel_init( void );

void dynamixel_scanServos( void );

void dynamixel_update( void );

void dynamixel_getPositions( void );

void dynamixel_resetChannel( unsigned char channel );

struct dynamixel_servo_descriptor{
	unsigned char found;
	unsigned char current_position_set_point_low;
	unsigned char current_position_set_point_high;
	unsigned char current_speed_set_point_low;
	unsigned char current_speed_set_point_high;
	unsigned char current_position_low;
	unsigned char current_position_high;
	unsigned char current_speed_low;
	unsigned char current_speed_high;
	unsigned char cw_limit_low;
	unsigned char cw_limit_high;
	unsigned char ccw_limit_low;
	unsigned char ccw_limit_high;
	unsigned char error;
	unsigned char new_set_point;
};

#ifndef _DYNAMIXEL_NO_EXTERN_
extern unsigned char dynamixel_autoUpdateEnabled;
extern const unsigned char dynamixel_actionCommand[6];
extern struct dynamixel_servo_descriptor dynamixel_servos[NUMSERVOS];
#endif //_DYNAMIXEL_NO_EXTERN_

#endif //_DYNAMIXEL_H_
