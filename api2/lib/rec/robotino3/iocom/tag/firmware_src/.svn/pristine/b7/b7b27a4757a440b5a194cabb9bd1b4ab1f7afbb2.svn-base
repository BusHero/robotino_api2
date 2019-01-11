#include <math.h>

float odometry_x = 0;
float odometry_y = 0;
float odometry_phi = 0;

float _odometry_gear = 32.0f;
float _odometry_radius_wheel = 0.06f;
float _odometry_radius_base = 0.17f;
float _odometry_k = 1;
float _odometry_sqrt3k = 1;
float _odometry_23k = 1;

extern signed short fpga_motor_velocity( unsigned char motor );

#define M_PI 3.14159265f
/**
called from inside critical section
*/
void odometry_update( void )
{
	const float delta_m2m0 = fpga_motor_velocity(2) - fpga_motor_velocity(0);

	float vx = delta_m2m0 / _odometry_sqrt3k;
	float vy = _odometry_23k * ( fpga_motor_velocity(0) + 0.5f * delta_m2m0 - fpga_motor_velocity(1) );

	const float vw = vy + (float)fpga_motor_velocity(1) / _odometry_k;

	float omega = vw / _odometry_radius_base;

	vx *= 0.001f;
	vy *= 0.001f;
	omega *= 0.001f;

	//Project the local coordinates to global coordinates
	odometry_phi += omega;
	odometry_x += cosf( odometry_phi ) * vx - sinf( odometry_phi ) * vy;
	odometry_y += sinf( odometry_phi ) * vx + cosf( odometry_phi ) * vy;
}

void odometry_update_k( void )
{
	_odometry_k = 30.0f * 60.0f * _odometry_gear / ( 2.0f * M_PI * _odometry_radius_wheel );
	_odometry_23k = 2.0f / 3.0f / _odometry_k;
	_odometry_sqrt3k = 1.7320508f / _odometry_k;
}

void odometry_init( void )
{
	odometry_update_k();
}

void odometry_set_gear( float gear )
{
	_odometry_gear = gear;
	odometry_update_k();
}

void odometry_set_radius_wheel( float rw )
{
	_odometry_radius_wheel = rw;
	odometry_update_k();
}

void odometry_set_radius_base( float rb )
{
	_odometry_radius_base = rb;
	odometry_update_k();
}
