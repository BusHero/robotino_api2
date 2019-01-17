#define _GYRO_NO_EXTERN
#include "gyro.h"
#include "timer.h"

extern unsigned char io_version0( void );

extern void MPU6000_init( void );
extern unsigned char MPU6000_update( void );
extern float MPU6000_z_angle( void );
extern float MPU6000_z_velocity( void );
extern float MPU6000_bias( void );
extern float MPU6000_scale( void );
extern void MPU6000_set_scale( float scale );

unsigned int gyro_data_stamps[GYRO_DATA_MAX_SIZE] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
float gyro_data_omegas[GYRO_DATA_MAX_SIZE] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
unsigned char gyro_data_size = 0;

unsigned int last_gyroUpdateTime = 0;
unsigned int gyro_stamp = 0;
float gyro_deltat = 0;

void gyro_init( void )
{
	MPU6000_init();
}

void gyro_update( void )
{
	gyro_stamp = globalTimeMicro;
	
	if( gyro_stamp >= last_gyroUpdateTime )
	{
		gyro_deltat = 0.000001f * (gyro_stamp - last_gyroUpdateTime);
	}
	else
	{
		gyro_deltat = 0.0000001f * (0xFFFFFFFF - ( last_gyroUpdateTime - gyro_stamp ) + 1);
	}
	
	if( MPU6000_update() )
	{
		last_gyroUpdateTime = gyro_stamp;
	}
}

float gyro_z_angle( void )
{
	return MPU6000_z_angle();
}

float gyro_z_velocity( void )
{
	return MPU6000_z_velocity();
}

float gyro_bias( void )
{
	return MPU6000_bias();
}

void gyro_set_bias( float bias )
{
}

float gyro_scale( void )
{
	return MPU6000_scale();
}

void gyro_set_scale( float scale )
{
	MPU6000_set_scale( scale );
}

float deg2rad( float deg )
{
	return PI * deg / 180.f;
}

float rad2deg( float rad )
{
	return 180.0f * rad / PI;
}

