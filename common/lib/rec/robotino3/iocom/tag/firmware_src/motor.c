#define _MOTOR_NO_EXTERN_
#include "motor.h"
#include "io.h"
#include "util.h"
#include "robotino3io.h"

#include <string.h>
#include <LPC23xx.H>

extern unsigned int globalTime( void );
extern unsigned int elapsed(unsigned int since);

#define MOTOR_DEBUG_BUF_SIZE   128      /*** Must be a power of 2 (2,4,8,16,32,64,128,256,512,...) ***/

#if MOTOR_DEBUG_BUF_SIZE < 2
#error MOTOR_DEBUG_BUF_SIZE is too small.  It must be larger than 1.
#elif ((MOTOR_DEBUG_BUF_SIZE & (MOTOR_DEBUG_BUF_SIZE-1)) != 0)
#error MOTOR_DEBUG_BUF_SIZE must be a power of 2.
#endif

struct motor_debug_st {
  unsigned int in;          /* Next In Index */
  unsigned int out;         /* Next Out Index */
  motor_debug_data buf[MOTOR_DEBUG_BUF_SIZE];     /* Buffer */
};

static struct motor_debug_st motorDebugBuf = { 0, 0, };
#define MOTOR_DEBUG_BUFLEN ((unsigned short)(motorDebugBuf.in - motorDebugBuf.out))

float motor_saftey_max_speed = 400.0; //rpm

extern unsigned char motor_needs_update;

extern signed int _fpga_motor_position[4];
extern signed short _fpga_motor_velocity[4];
extern unsigned char _fpga_io_din;


extern void pwm_init( void );
extern void pwm_set( unsigned char channel, unsigned char ratio );
extern unsigned char pwm_get( unsigned char channel );

extern float adc_motor_current( unsigned char motor );

#define motor_monitor_timeout 1000
unsigned char motor_monitor = 0;
unsigned int motor_monitor_startTime = 0;

short motor_speed_setpoint[ numMotors ] = { 0, 0, 0, 0 };
short _motor_speed_setpoint_buffer[ numMotors ] = { 0, 0, 0, 0 };
unsigned char _motor_speed_setpoint_buffer_flag = 0;
unsigned char _motor_speed_setpoint_buffer_lock = 0;

#define _sizeof_motor_ss numMotors*sizeof(float)
float motor_ss[ numMotors ] = { 0.0f, 0.0f, 0.0f, 0.0f };
float _motor_ss_copy[ numMotors ] = { 0.0f, 0.0f, 0.0f, 0.0f };
unsigned char _motor_ss_lock = 0;

unsigned short motor_current[ numMotors ] = { 0, 0, 0, 0 };

float motor_kp[ numMotors ] = { 0.1, 0.1, 0.1, 0.1 };
float motor_kp_default[ numMotors ] = { 0.1, 0.1, 0.1, 0.1 };
float _motor_kp_buffer[ numMotors ] = { 0, 0, 0, 0 };
unsigned char _motor_kp_buffer_flag = 0; 
unsigned char _motor_kp_buffer_lock = 0; 

float motor_ki[ numMotors ] = { 0.005, 0.005, 0.005, 0.005 };
float motor_ki_default[ numMotors ] = { 0.005, 0.005, 0.005, 0.005 };
float _motor_ki_buffer[ numMotors ] = { 0, 0, 0, 0 };
unsigned char _motor_ki_buffer_flag = 0; 
unsigned char _motor_ki_buffer_lock = 0; 

float motor_errant[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float motor_uant[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

unsigned char _motor_accel_limits_enabled = 1;

float motor_maxaccel[4] = { 12.0f, 12.0f, 12.0f, 12.0f }; //rpm/ms
float _motor_maxaccel_buffer[4] = { 0, 0, 0, 0 }; //rpm/ms
unsigned char _motor_maxaccel_buffer_flag = 0; 
unsigned char _motor_maxaccel_buffer_lock = 0; 

float motor_minaccel[4] = { -12.0f, -12.0f, -12.0f, -12.0f }; //rpm/ms
float _motor_minaccel_buffer[4] = { 0, 0, 0, 0 }; //rpm/ms
unsigned char _motor_minaccel_buffer_flag = 0; 
unsigned char _motor_minaccel_buffer_lock = 0; 

#define _sizeof_motor_control_point numMotors*sizeof(signed short)
signed short motor_control_point[ 4 ] = { 0, 0, 0, 0 };
signed short _motor_control_point_copy[ 4 ] = { 0, 0, 0, 0 };
unsigned char _motor_control_point_lock = 0;

signed short _motor_control_point_buffer[ 4 ] = { 0, 0, 0, 0 };
unsigned char _motor_control_point_buffer_lock = 0;

unsigned char motor_flags[ numMotors ] = { 0, 0, 0, 0 };

unsigned char _motor_stop_all = 0;

void motor_dir_output_init( void )
{
#ifndef MCB2300
	//P0.6 is DIR1
	//P0.7	is DIR2
	//P0.8	is DIR3
	//P0.9	is DIR4
	setBit( M1_DIR_DIR, M1_DIR_BIT );
	setBit( M2_DIR_DIR, M2_DIR_BIT );
	setBit( M3_DIR_DIR, M3_DIR_BIT );
	setBit( M4_DIR_DIR, M4_DIR_BIT );

	setBit( M1_ON_DIR, M1_ON_BIT );
	setBit( M2_ON_DIR, M2_ON_BIT );
	setBit( M3_ON_DIR, M3_ON_BIT );
	setBit( M4_ON_DIR, M4_ON_BIT );
#endif
}

unsigned char motor_dir( unsigned char motor )
{
	return bitIsSet( M1_DIR_PIN, (M1_DIR_BIT+motor) );
}

void motor_setDir( unsigned char motor, unsigned char dir )
{
	if( dir )
	{
		setBit( M1_DIR_SET, (M1_DIR_BIT+motor) );
	}
	else
	{
		setBit( M1_DIR_CLR, (M1_DIR_BIT+motor) );
	}
}

void motor_set_on( unsigned char motor )
{
	setBit( M1_ON_SET, (M1_ON_BIT+motor) );
}

void motor_set_off( unsigned char motor )
{
	setBit( M1_ON_CLR, (M1_ON_BIT+motor) );
}

void motor_all_off( void )
{
	unsigned char i;
	for( i=0; i<4; ++i )
	{
		motor_set_off( i );
	}
}

void motor_all_on( void )
{
	unsigned char i;
	for( i=0; i<4; ++i )
	{
		motor_set_on( i );
	}
}


void motor_init( void )
{
#ifndef MCB2300
	motor_dir_output_init();

	motor_all_off();

	pwm_init();

	pwm_set( 1, 0 );
	pwm_set( 2, 0 );
	pwm_set( 3, 0 );
	pwm_set( 4, 0 );
	pwm_set( 5, 0 );
	pwm_set( 6, 0 );

	motor_all_on();
#endif
}

void motor_stopAll( void )
{
	_motor_stop_all = 1;
}

void motor_setControlPoint( unsigned char motor, signed short control_point )
{
	unsigned char newDir = ( control_point > 0 ? 1 : 0 );

	motor_control_point[motor] = control_point;

	motor_setDir( motor, newDir );

	if( newDir )
	{
		pwm_set( motor+1, control_point );
	}
	else
	{
		pwm_set( motor+1, - control_point );
	}
}

void motor_pi_control( unsigned char motor )
{
	float K = motor_kp[motor];
	float Ki = motor_ki[motor];

	float ssd;
	float y;
	float err;
	float u;
	float ss;

	y = _fpga_motor_velocity[ motor ];

	ss = motor_speed_setpoint[motor];
	ssd = ss - motor_ss[motor];

	if( _motor_accel_limits_enabled )
	{
		if( ssd < motor_minaccel[motor] )
		{
			ssd = motor_minaccel[motor];
		}
		else if( ssd > motor_maxaccel[motor] )
		{
			ssd = motor_maxaccel[motor];
		}

		motor_ss[motor] = motor_ss[motor] + ssd;

		if( motor_ss[motor] > -1.0f && motor_ss[motor] < 1.0f )
		{
			motor_ss[motor] = 0;
		}
	}
	else
	{
		motor_ss[motor] = ss;
	}

	err = motor_ss[motor] - y;

	if( K < 0 )
	{
		K = motor_kp_default[motor];
	}

	if( Ki < 0 )
	{
		Ki = motor_ki_default[motor];
	}

	if( 0 == Ki )
	{
		u = K * err; // There is no integral term
	}
	else
	{
		u = motor_uant[motor] + K * (err - motor_errant[motor]) + Ki * err;
	}

	if( 0 == motor_ss[motor] && 0 == y )
	{
		u = 0;
	}


	if( u > 255.0f )
	{
		u = 255.0f;
	}
	else if( u < -255.0f )
	{
		u = -255.0f;
	}

	motor_uant[motor] = u;
	motor_errant[motor] = err;

	motor_setControlPoint( motor, (signed short)u );
}

void motor_update( void )
{
	unsigned char i;
	unsigned char sleep[numMotors] = { 0, 0, 0, 0 };
	static unsigned int sleepTimer[numMotors] = { 0, 0, 0, 0 };
	struct motor_debug_st *p = &motorDebugBuf;
	motor_debug_data *pd;

	if( _motor_stop_all )
	{
		_motor_stop_all = 0;

		for( i=0; i<numMotors; ++i )
		{
			motor_speed_setpoint[i] = 0;
			motor_ss[i] = 0.0f;
			motor_control_point[i] = 0;
			pwm_set( i+1, 0 );
		}
	}

	if( 0 != _motor_kp_buffer_flag && 0 == _motor_kp_buffer_lock )
	{
		for( i=0; i<numMotors; ++i )
		{
			if( bitIsSet( _motor_kp_buffer_flag, i ) )
			{
				clearBit( _motor_kp_buffer_flag, i );
				motor_kp[i] = _motor_kp_buffer[i];
			}
		}
	}

	if( 0 != _motor_ki_buffer_flag && 0 == _motor_ki_buffer_lock )
	{
		for( i=0; i<numMotors; ++i )
		{
			if( bitIsSet( _motor_ki_buffer_flag, i ) )
			{
				clearBit( _motor_ki_buffer_flag, i );
				motor_ki[i] = _motor_ki_buffer[i];
			}
		}
	}

	if( 0 != _motor_speed_setpoint_buffer_flag && 0 == _motor_speed_setpoint_buffer_lock )
	{
		for( i=0; i<numMotors; ++i )
		{
			if( bitIsSet( _motor_speed_setpoint_buffer_flag, i ) )
			{
				clearBit( _motor_speed_setpoint_buffer_flag, i );
				motor_speed_setpoint[i] = _motor_speed_setpoint_buffer[i];
			}
		}
	}

	if( bitIsClear( SAFTEY_PIN, SAFTEY_BIT ) )
	{
		if( 0 == ( _fpga_io_din & 0x1 ) )
		{
			for( i=0; i<3; ++i )
			{
				motor_speed_setpoint[i] = 0;
			}
		}
		else if( 0 == ( _fpga_io_din & 0x2 ) )
		{
			signed short maxSpeed = 0;
			for( i=0; i<3; ++i )
			{
				if( motor_speed_setpoint[i] > maxSpeed )
				{
					maxSpeed = motor_speed_setpoint[i];
				}
				else if( motor_speed_setpoint[i] < -maxSpeed )
				{
					maxSpeed = -motor_speed_setpoint[i];
				}
			}

			if( maxSpeed > motor_saftey_max_speed )
			{
				float f = motor_saftey_max_speed / maxSpeed;
				for( i=0; i<3; ++i )
				{
					motor_speed_setpoint[i] *= f;
				}
			}
		}
	}

	if( 0 != _motor_minaccel_buffer_flag && 0 == _motor_minaccel_buffer_lock )
	{
		for( i=0; i<numMotors; ++i )
		{
			if( bitIsSet( _motor_minaccel_buffer_flag, i ) )
			{
				clearBit( _motor_minaccel_buffer_flag, i );
				motor_minaccel[i] = _motor_minaccel_buffer[i];
			}
		}
	}

	if( 0 != _motor_maxaccel_buffer_flag && 0 == _motor_maxaccel_buffer_lock )
	{
		for( i=0; i<numMotors; ++i )
		{
			if( bitIsSet( _motor_maxaccel_buffer_flag, i ) )
			{
				clearBit( _motor_maxaccel_buffer_flag, i );
				motor_maxaccel[i] = _motor_maxaccel_buffer[i];
			}
		}
	}

	for( i=0; i<numMotors; ++i )
	{
		if( 0 == motor_speed_setpoint[i] && 0 == motor_ss[i] )
		{
			++sleepTimer[i];
			if( sleepTimer[i] > 1000 )
			{
				sleep[i] = 1;
				motor_uant[i] = 0;
				motor_errant[i] = 0;
				motor_setControlPoint( i, 0 );
			}
		}
		else
		{
			sleepTimer[i] = 0;
		}
	}

	for( i=0; i<numMotors; ++i )
	{
		if( 1 == sleep[i] )
		{
			continue;
		}

		if(motor_kp[i]>0)
		{
			motor_pi_control( i );
		}
		else
		{
			motor_ss[i] = motor_speed_setpoint[i];
			motor_setControlPoint( i, motor_speed_setpoint[i] );
		}
	}

	if( 0 == _motor_ss_lock )
	{
		memcpy( _motor_ss_copy, motor_ss, _sizeof_motor_ss );
	}

	if( 0 == _motor_control_point_lock )
	{
		memcpy( _motor_control_point_copy, motor_control_point, _sizeof_motor_control_point );
	}
	
	if(motor_monitor_startTime!=0)
	{
		if(elapsed(motor_monitor_startTime) < motor_monitor_timeout)
		{
			if (((p->in - p->out) & ~(MOTOR_DEBUG_BUF_SIZE-1)) == 0)
			{
				pd = &p->buf[p->in & (MOTOR_DEBUG_BUF_SIZE-1)];
				
				pd->time = globalTime();
				pd->control_point = motor_control_point[motor_monitor];
				pd->current = 1000.0f*adc_motor_current(motor_monitor);
				pd->speed_setpoint = motor_speed_setpoint[motor_monitor];
				pd->speed = _fpga_motor_velocity[motor_monitor];
				
				p->in++;
			}
		}
		else
		{
			motor_monitor_startTime = 0;
		}
	}
}

void motor_ss_copy( unsigned char motor, float* value )
{
	_motor_ss_lock = 1;
	*value = _motor_ss_copy[motor];
	_motor_ss_lock = 0;
}

void motor_control_point_copy( unsigned char motor, signed short* value )
{
	_motor_control_point_lock = 1;
	*value = _motor_control_point_copy[motor];
	_motor_control_point_lock = 0;
}

void motor_set_control_point( unsigned char motor, signed short value )
{
	_motor_control_point_buffer_lock = 1;
	_motor_control_point_buffer[motor] = value;
	_motor_control_point_buffer_lock = 0;
}

void motor_set_kp( unsigned char motor, float value )
{
	_motor_kp_buffer_lock = 1;
	setBit( _motor_kp_buffer_flag, motor );
	if(value >= 255)
	{
		_motor_kp_buffer[motor] = motor_kp_default[motor];
	}
	else
	{
		_motor_kp_buffer[motor] = value;
	}
	_motor_kp_buffer_lock = 0;
}

void motor_set_ki( unsigned char motor, float value )
{
	_motor_ki_buffer_lock = 1;
	setBit( _motor_ki_buffer_flag, motor );
	if(value >= 255)
	{
		_motor_ki_buffer[motor] = motor_ki_default[motor];
	}
	else
	{
		_motor_ki_buffer[motor] = value;
	}
	_motor_ki_buffer_lock = 0;
}

void motor_set_accel_limits_enabled( unsigned char enable )
{
	_motor_accel_limits_enabled = enable;
}

void motor_set_speed_setpoint( unsigned char motor, signed short value )
{
	_motor_speed_setpoint_buffer_lock = 1;
	setBit( _motor_speed_setpoint_buffer_flag, motor );
	_motor_speed_setpoint_buffer[motor] = value;
	_motor_speed_setpoint_buffer_lock = 0;
}

void motor_set_minaccel( unsigned char motor, float value )
{
	_motor_minaccel_buffer_lock = 1;
	setBit( _motor_minaccel_buffer_flag, motor );
	_motor_minaccel_buffer[motor] = value;
	_motor_minaccel_buffer_lock = 0;
}

void motor_set_maxaccel( unsigned char motor, float value )
{
	_motor_maxaccel_buffer_lock = 1;
	setBit( _motor_maxaccel_buffer_flag, motor );
	_motor_maxaccel_buffer[motor] = value;
	_motor_maxaccel_buffer_lock = 0;
}

void motor_set_saftey_velocity( float value )
{
	motor_saftey_max_speed = value;
}

void motor_startMonitor(unsigned char motor)
{
	if(motor<numMotors)
	{
		motor_monitor_startTime = globalTime();
		motor_monitor = motor;
	}
}

unsigned char motor_currentMonitor( void )
{
	return motor_monitor;
}

int __swi(13) motor_get_debug (motor_debug_data* dbg);
int __SWI_13              (motor_debug_data* dbg)
{
	motor_debug_data *pd;
  struct motor_debug_st *p = &motorDebugBuf;

  if (MOTOR_DEBUG_BUFLEN == 0)
	{
    return (-1);
	}

  pd = &(p->buf [(p->out++) & (MOTOR_DEBUG_BUF_SIZE - 1)]);
	
	dbg->time = pd->time;
	dbg->speed_setpoint = pd->speed_setpoint;
	dbg->speed = pd->speed;
	dbg->current = pd->current;
	dbg->control_point = pd->control_point;
	
	return MOTOR_DEBUG_BUFLEN;
}

int motor_get_debug_data (motor_debug_data* dbg)
{
	return motor_get_debug(dbg);
}
