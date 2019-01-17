#ifndef _GYRO_H_
#define _GYRO_H_

void gyro_init( void );
void gyro_update( void );

float gyro_z_angle( void );
float gyro_z_velocity( void );
float gyro_bias( void );
void gyro_set_bias( float bias );

float gyro_scale( void );
void gyro_set_scale( float scale );

#define PI			3.14159265358979323846f
#define PI2			6.28318530717958647692f
#define PI_2		1.57079632679489661923f

float deg2rad( float deg );
float rad2deg( float rad );

#define GYRO_DATA_MAX_SIZE 16

#ifndef _GYRO_NO_EXTERN
extern unsigned int gyro_data_stamps[GYRO_DATA_MAX_SIZE];
extern float gyro_data_omegas[GYRO_DATA_MAX_SIZE];
extern unsigned char gyro_data_size;
extern const unsigned int gyro_stamp;
extern const float gyro_deltat;
#endif

#endif
