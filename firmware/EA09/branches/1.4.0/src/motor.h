#ifndef _MOTOR_H_
#define _MOTOR_H_

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

void motor_init( void );

void motor_stopAll( void );

/**
Run the internal PID control loop on all motors with bitIsClear( mode, 2 )
For motor with bitIsSet( mode, 1 ) generate udp message
*/
void motor_update( void );

void motor_setMCTRL( unsigned char motor, unsigned int value );

void motor_setMMONITOR( unsigned char motor, unsigned int value );

void motor_setControlPoint( unsigned char motor );

void motor_setPosControlParameters( unsigned char motor, unsigned char speed, unsigned short imax );

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

#ifndef _MOTOR_NO_EXTERN_
extern const unsigned int MCTRL[ 4 ];
extern const unsigned int MMONITOR[ 4 ];

extern short motor_speed_setpoint[ 4 ];
extern unsigned short motor_current[ 4 ];

extern unsigned char motor_kp[ 4 ];
extern unsigned char motor_kp_default[ 4 ];
extern unsigned char motor_ki[ 4 ];
extern unsigned char motor_ki_default[ 4 ];
extern unsigned char motor_kd[ 4 ];
extern unsigned char motor_kd_default[ 4 ];
extern short motor_setpoint_deviation[ 4 ];
extern int motor_proportional_term[ 4 ];
extern int motor_integral_term[ 4 ];
extern short motor_control_point[ 4 ];

extern unsigned char motor_flags[ 4 ];

extern const struct motor_poscontrol_param POSCONTROL[ 4 ];
#endif

#endif //_MOTOR_H_
