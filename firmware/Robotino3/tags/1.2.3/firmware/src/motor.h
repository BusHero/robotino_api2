#ifndef _MOTOR_H_
#define _MOTOR_H_

#define numMotors 4

#define M0CTRL MCTRL[0]
#define M1CTRL MCTRL[1]
#define M2CTRL MCTRL[2]
#define M3CTRL MCTRL[3]

#define M0MONITOR MMONITOR[0]
#define M1MONITOR MMONITOR[1]
#define M2MONITOR MMONITOR[2]
#define M3MONITOR MMONITOR[3]

#define MOTOR_IS_CONTROLLER_OFF( motor ) ( 0x0 == MCTRL[motor] )
#define MOTOR_IS_INTERNAL_CONTROLLER_ON( motor ) ( 0x1 == MCTRL[motor] )
#define MOTOR_IS_EXTERNAL_CONTROLLER_ON( motor ) ( 0x2 == MCTRL[motor] )
#define MOTOR_IS_SHORTCIRCUIT_ON( motor ) ( 0x3 == MCTRL[motor] )
#define MOTOR_IS_POSITIONCONTROLLER_ON( motor ) ( 0x4 == MCTRL[motor] )

#define MOTOR_SET_INTERNAL_CONTROLLER_ON( motor ) ( motor_setMCTRL(motor,0x1) )
#define MOTOR_SET_SHORTCIRCUIT_ON( motor ) ( motor_setMCTRL(motor,0x3) )
#define MOTOR_SET_POSITIONCONTROLLER_ON( motor ) ( motor_setMCTRL(motor,0x4) )

#define MOTOR_SEND_TYPE_A_MESSAGE( motor ) ( bitIsSet( MMONITOR[motor], 0 ) )
#define MOTOR_SEND_TYPE_B_MESSAGE( motor ) ( bitIsSet( MMONITOR[motor], 1 ) )

struct motor_poscontrol_param
{
	unsigned char initialized;
	unsigned char speed;
	unsigned short imax;
	signed int minpos;
	signed int maxpos;
	signed int midpos;
	signed int posrange;
	unsigned char posreached;
	unsigned short imaxcnt;
	unsigned short overcurrent;
	signed int targetpos;
};

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

#endif //_MOTOR_H_
