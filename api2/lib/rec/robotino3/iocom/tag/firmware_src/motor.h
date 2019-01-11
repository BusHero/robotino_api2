#ifndef _MOTOR_H_
#define _MOTOR_H_

#define numMotors 4
#define motor_time_delta 10 //in 0.1ms

void motor_startMonitor(unsigned char motor);
unsigned char motor_currentMonitor( void );

void motor_ss_copy( unsigned char motor, float* value );
void motor_control_point_copy( unsigned char motor, signed short* value );
void motor_set_control_point( unsigned char motor, signed short value );
void motor_set_kp( unsigned char motor, float value );
void motor_set_ki( unsigned char motor, float value );
void motor_set_accel_limits_enabled( unsigned char enable );

void motor_set_speed_setpoint( unsigned char motor, signed short value );
void motor_set_minaccel( unsigned char motor, float value );
void motor_set_maxaccel( unsigned char motor, float value );

void motor_set_saftey_velocity( float value );

typedef struct
{
	unsigned int time; //ms
	signed short speed_setpoint; //rpm
	signed short speed; //rpm
	unsigned short current; //mA
	signed short control_point;
} motor_debug_data;

int motor_get_debug_data (motor_debug_data* dbg);

#endif //_MOTOR_H_
