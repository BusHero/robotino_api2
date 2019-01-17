#define _MOTOR_NO_EXTERN_
#include "motor.h"
#include "pwm.h"
#include "adc.h"
#include "io.h"
#include "fpga.h"
#include "udp.h"
#include "util.h"

#include <LPC23xx.H>

extern const signed short newVelMax;

const unsigned char numMotors = 4;

unsigned int MCTRL[ 4 ] = { 0x1, 0x1, 0x1, 0x3 };
unsigned int MMONITOR[ 4 ] = { 0, 0, 0, 0 };

short motor_speed_setpoint[ numMotors ] = { 0, 0, 0, 0 };
unsigned short motor_current[ numMotors ] = { 0, 0, 0, 0 };

unsigned char motor_kp[ numMotors ] = { 25, 25, 25, 25 };
unsigned char motor_kp_default[ numMotors ] = { 25, 25, 25, 25 };
unsigned char motor_ki[ numMotors ] = { 25, 25, 25, 25 };
unsigned char motor_ki_default[ numMotors ] = { 25, 25, 25, 25 };
unsigned char motor_kd[ numMotors ] = { 25, 25, 25, 25 };
unsigned char motor_kd_default[ numMotors ] = { 25, 25, 25, 25 };
short motor_setpoint_deviation[ 4 ] = { 0, 0, 0, 0 };
int motor_proportional_term[ 4 ] = { 0, 0, 0, 0 };
int motor_integral_term[ 4 ] = { 0, 0, 0, 0 };
int motor_differential_term[ 4 ] = { 0, 0, 0, 0 };
int motor_lastError[ 4 ] = { 0, 0, 0, 0 };
short motor_control_point[ 4 ] = { 0, 0, 0, 0 };

unsigned char motor_flags[ numMotors ] = { 0, 0, 0, 0 };

//The length of the error history buffer must be a power of 2
#define EH_BUF_SIZE 64

#if EH_BUF_SIZE < 2
#error EH_BUF_SIZE is too small.  It must be larger than 1.
#elif ((EH_BUF_SIZE & (EH_BUF_SIZE-1)) != 0)
#error EH_BUF_SIZE must be a power of 2.
#endif

struct motor_poscontrol_param POSCONTROL[ 4 ] = {
	{0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0}
};

#define SET_DIR_M0( x ) if( x > 0 ) { setBit( IOSET0, 29 ); } else { setBit( IOCLR0, 29 ); }
#define DIR_M0 ( bitIsSet( IOPIN0, 29 ) ? 1 : 0 )
#define SET_DIR_M1( x ) if( x > 0 ) { setBit( IOSET0, 30 ); } else { setBit( IOCLR0, 30 ); }
#define DIR_M1 ( bitIsSet( IOPIN0, 30 ) ? 1 : 0 )
#define SET_DIR_M2( x ) if( x > 0 ) { setBit( FIO3SET, 23 ); } else { setBit( FIO3CLR, 23 ); }
#define DIR_M2 ( bitIsSet( FIO3PIN, 23 ) ? 1 : 0 )
#define SET_DIR_M3( x ) if( x > 0 ) { setBit( IOSET1, 18 ); } else { setBit( IOCLR1, 18 ); }
#define DIR_M3 ( bitIsSet( IOPIN1, 18 ) ? 1 : 0 )

struct eh_st
{
	unsigned short index;
	int sum;
	short buffer[EH_BUF_SIZE];
};

static struct eh_st eh[ numMotors ] = { {0,0,}, {0,0,}, {0,0,}, {0,0,} };


void motor_dir_output_init( void )
{
	//P0.29 is DIR1
	//P0.30	is DIR2
	setBit2( IODIR0, 29, 30 );

	//P3.23	is DIR3
	setBit( FIO3DIR, 23 );

	//P1.18	is DIR4
	setBit( IODIR1, 18 );
}

unsigned char motor_dir( unsigned char motor )
{
	switch( motor )
	{
	case 0:
		return DIR_M0;

	case 1:
		return DIR_M1;

	case 2:
		return DIR_M2;

	case 3:
		return DIR_M3;

	default:
		return 0;
	}
}

void motor_setDir( unsigned char motor, unsigned char dir )
{
	switch( motor )
	{
	case 0:
		SET_DIR_M0( dir );
		break;

	case 1:
		SET_DIR_M1( dir );
		break;

	case 2:
		SET_DIR_M2( dir );
		break;

	case 3:
		SET_DIR_M3( dir );
		break;

	default:
		break;
	}
}

void motor_init( void )
{
	unsigned char motor;
	unsigned int index;
	struct eh_st *ehp;

	for( motor=0; motor<numMotors; ++motor )
	{
		ehp = &eh[motor];
		for( index=0; index<EH_BUF_SIZE; ++index )
		{
			*( ehp->buffer + index ) = 0;
		}
	}

	motor_dir_output_init();

	pwm_init();

	pwm_set( 1, 0 );
	pwm_set( 2, 0 );
	pwm_set( 3, 0 );
	pwm_set( 4, 0 );
	pwm_set( 5, 0 );
	pwm_set( 6, 0 );
}

void motor_stopAll( void )
{
	int i;
	for( i=0; i<numMotors; ++i )
	{
		motor_speed_setpoint[i] = 0;
		motor_control_point[i] = 0;
		pwm_set( i+1, 0 );
	}
}

void eh_add( struct eh_st *ehp, int err )
{
	ehp->sum -= ehp->buffer[ (ehp->index) & (EH_BUF_SIZE-1) ];
	ehp->sum += err;
	ehp->buffer[ (ehp->index++) & (EH_BUF_SIZE-1) ] = err;
}

void motor_updateSensors( unsigned char motor )
{
	motor_current[motor] = adc_motor_current( motor );
	//flags[motor] = din_motor_tempFlag( motor );
}

void motor_setMCTRL( unsigned char motor, unsigned int value )
{
	MCTRL[motor] = value;
	udp_send_SetMCTRL( motor );
}

void motor_setMMONITOR( unsigned char motor, unsigned int value )
{
	MMONITOR[motor] = value;
	udp_send_SetMMONITOR( motor );
}

void motor_setControlPoint( unsigned char motor )
{
	short* control_point = motor_control_point + motor;
	unsigned char newDir = ( *control_point > 0 ? 1 : 0 );

	if( newDir != motor_dir( motor )	)
	{
		if( pwm_get( motor+1 ) > 0 )
		{
			pwm_set( motor+1, 0 );
		}
		else
		{
			motor_setDir( motor, newDir );
		}

		return;
	}

	if( *control_point > 0 )
	{
		if( *control_point > 255 )
		{
			pwm_set( motor+1, 255 );
		}
		else
		{
			pwm_set( motor+1, *control_point );
		}
	}
	else
	{
		if( *control_point < -255 )
		{
			pwm_set( motor+1, 255 );
		}
		else
		{
			pwm_set( motor+1, - *control_point );
		}
	}
}

void motor_setPosControlParameters( unsigned char motor, unsigned char speed, unsigned short imax )
{
	POSCONTROL[motor].initialized = 0;
	POSCONTROL[motor].speed = speed;
	POSCONTROL[motor].imax = imax;
	POSCONTROL[motor].imaxcnt = 0;
	POSCONTROL[motor].posreached = 0;
	POSCONTROL[motor].targetpos = 0;
}

void motor_pi_control( unsigned char motor )
{
	short* err;
	int* proportional;
	int* integral;
	int* differential;
	int *lastError;
	short* control_point;
	int sum;
	struct eh_st *ehp = &eh[motor];

	err = motor_setpoint_deviation + motor;
	proportional = motor_proportional_term + motor;
	integral = motor_integral_term + motor;
	differential = motor_differential_term + motor;
	lastError = motor_lastError + motor;
	control_point = motor_control_point + motor;

	*err = motor_speed_setpoint[motor] - fpga_motor_velocity[ motor ];

	*proportional = *err * motor_kp[motor];

	eh_add( ehp, *err );
	*integral = ehp->sum * motor_ki[motor];
	if( *integral < (1<<23) )
	{
		*integral *= motor_kp[motor];
		*integral = ( *integral >> 9 );
	}
	else
	{
		*integral = ( *integral >> 9 );
		*integral *= motor_kp[motor];
	}

	*differential = ( *err - *lastError ) * motor_kd[motor];
	
	*lastError = *err;

	sum = *proportional + *integral + *differential;

	sum = ( sum >> 1 );

	if( sum > 255 )
	{
		*control_point = 255;
	}
	else if( sum < -255 )
	{
		*control_point = -255;
	}
	else
	{
		*control_point = sum;
	}

	//setValue = gain + motor_speed_setpoint[motor];

	//solange noch kein Motor angeschlossen ist
	//setValue = setVelocity[motor];

	motor_setControlPoint( motor );

	if( MOTOR_SEND_TYPE_B_MESSAGE( motor ) )
	{
		udp_send_monitor_message_type_b( motor );
	}

	//	if( MOTOR_IS_PIREPORT_ON( motor ) )
	//	{
	//		udp_send_pi_control_values( motor,
	//			motor_kp[motor],
	//			motor_ki[motor],
	//			motor_speed_setpoint[motor],
	//			fpga_motor_velocity[motor],
	//			proportional,
	//			ehp->sum,
	//			integral,
	//			gain,
	//			setValue,
	//			dir );
	//	}
}

void motor_pos_control( unsigned char motor )
{
	short* control_point;
	unsigned int deltapos;
	signed int currentpos = -fpga_motor_position[motor];

	control_point = motor_control_point + motor;

	*control_point = 0;

	if( motor_current[ motor ] > POSCONTROL[motor].imax )
	{
		if( POSCONTROL[motor].imaxcnt < 255 )
		{
			POSCONTROL[motor].imaxcnt++;
		}
	}
	else if( POSCONTROL[motor].imaxcnt > 0 )
	{
		POSCONTROL[motor].imaxcnt--;
	}

	if( POSCONTROL[motor].overcurrent > 0 )
	{
		if( 0 == POSCONTROL[motor].imaxcnt )
		{
			POSCONTROL[motor].overcurrent = 0;
		}

		*control_point = 0;
		motor_setControlPoint( motor );
		return;
	}
	else if( POSCONTROL[motor].imaxcnt > 100 )
	{
		POSCONTROL[motor].imaxcnt = 255;
		POSCONTROL[motor].overcurrent = 1;
	}

	/*POSCONTROL[motor].speed = speed;
	POSCONTROL[motor].imax = imax;*/
	switch( POSCONTROL[motor].initialized )
	{
	case 0:
		*control_point = -POSCONTROL[motor].speed;
		if( io_din( 0 ) == 0 )
		{
			*control_point = 0;
			POSCONTROL[motor].initialized = 1;
			POSCONTROL[motor].minpos = currentpos;
		}
		break;

	case 1:
		*control_point = POSCONTROL[motor].speed;
		if( io_din( 1 ) == 0 )
		{
			*control_point = 0;

			POSCONTROL[motor].maxpos = currentpos;
			POSCONTROL[motor].posrange = ( POSCONTROL[motor].maxpos - POSCONTROL[motor].minpos );
			POSCONTROL[motor].midpos = POSCONTROL[motor].minpos + POSCONTROL[motor].posrange / 2;
			POSCONTROL[motor].initialized = 2;

			POSCONTROL[motor].targetpos = POSCONTROL[motor].midpos;
		}
		break;

	case 2:
		if( 0 != motor_speed_setpoint[motor] )
		{
			if( motor_speed_setpoint[motor] >= newVelMax-1 )
			{
				POSCONTROL[motor].targetpos = POSCONTROL[motor].maxpos;
			}
			else if( motor_speed_setpoint[motor] <= -newVelMax+1 )
			{
				POSCONTROL[motor].targetpos = POSCONTROL[motor].minpos;
			}
			else
			{ 
				deltapos = ( POSCONTROL[motor].posrange * ( motor_speed_setpoint[motor] + newVelMax ) ) / ( newVelMax << 1 );
				POSCONTROL[motor].targetpos = POSCONTROL[motor].minpos + deltapos;
			}

			if( POSCONTROL[motor].targetpos > POSCONTROL[motor].maxpos || POSCONTROL[motor].targetpos < POSCONTROL[motor].minpos )
			{
				POSCONTROL[motor].targetpos = POSCONTROL[motor].maxpos;
			}
			else if( POSCONTROL[motor].targetpos < POSCONTROL[motor].minpos )
			{
				POSCONTROL[motor].targetpos = POSCONTROL[motor].minpos;
			}
		}

		if( currentpos < POSCONTROL[motor].targetpos - 100 )
		{
			*control_point = POSCONTROL[motor].speed;
			POSCONTROL[motor].posreached = 0;
		}
		else if( currentpos > POSCONTROL[motor].targetpos + 100 )
		{
			*control_point = -POSCONTROL[motor].speed;
			POSCONTROL[motor].posreached = 0;
		}
		else
		{
			POSCONTROL[motor].posreached = 1;
			*control_point = 0;
		}
		break;

	default:
		break;
	}

	motor_setControlPoint( motor );
}

void motor_update( void )
{
	unsigned char i;
	unsigned char sleep[numMotors] = { 0, 0, 0, 0 };
	static unsigned int sleepTimer[numMotors] = { 0, 0, 0, 0 };

	for( i=0; i<numMotors; ++i )
	{
		if( 0 == motor_speed_setpoint[i] && MOTOR_IS_INTERNAL_CONTROLLER_ON( i ) )
		{
			++sleepTimer[i];
			if( sleepTimer[i] > 1000 )
			{
				sleep[i] = 1;
				motor_control_point[i] = 0;
				motor_setControlPoint( i );
			}
		}
		else
		{
			sleepTimer[i] = 0;
		}

		motor_updateSensors( i );
	}

	for( i=0; i<numMotors; ++i )
	{
		if( 1 == sleep[i] )
		{
			continue;
		}

		if( MOTOR_IS_INTERNAL_CONTROLLER_ON( i ) )
		{
			motor_pi_control( i );
		}
		else if( MOTOR_IS_EXTERNAL_CONTROLLER_ON( i ) )
		{
			//set from udp_callback
			//motor_setControlPoint( i );
		}
		else if( MOTOR_IS_SHORTCIRCUIT_ON( i ) )
		{
			motor_control_point[i] = motor_speed_setpoint[i];
			motor_setControlPoint( i );
		}
		else if( MOTOR_IS_POSITIONCONTROLLER_ON( i ) )
		{
			motor_pos_control( i );
		}
		else
		{
			motor_setControlPoint( 0 );
		}
	}

	for( i=0; i<numMotors; ++i )
	{
		if( MOTOR_SEND_TYPE_A_MESSAGE( i ) )
		{
			udp_send_monitor_message_type_a( i );
		}
	}
}

