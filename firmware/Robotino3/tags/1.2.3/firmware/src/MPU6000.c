#include "gyro.h"

#include "i2c.h"
#include "ssp.h"
#include "timer.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"

#include "util.h"

#include "robotino3io.h"

#include <math.h>
#include <string.h>
#include <stdio.h>

// Configuration

#define FSR			250
#define SAMPLE_RATE	100
#define FIFO_RATE	100
#define INTEGRATE	1

#define USE_SSP1	1
#define SSP_CH		1

// Do not change the lines below

#define SQRT1_2		0.707106781186547524401f
#define EPSILON		0.0001f
#define QUAT_SENS	1073741824.f
#define GYRO_SENS	( 131.f * 250.f / (float)FSR )
#define SENSORCFG	INV_XYZ_GYRO

//#define PRINTF_DEBUG
#define PRINT_EULER		0
#define PRINT_QUAT		0
#define PRINT_GYRO		0

#ifdef PRINTF_DEBUG
#define EXEC( cmd, msg ) \
	if ( 0 == cmd ) { \
		printf( msg " success\n\r" ); \
	} else { \
		printf( msg " failed\n\r" ); \
	} \
	msleep( 1 );
#else
#define EXEC( cmd, msg ) \
	if ( 0 != cmd ) { \
		printf( msg " failed\n\r" ); \
	} \
	msleep( 1 );
#endif
	
void MPU6000_enable_calibration( void );
void MPU6000_disable_calibration( void );
	
extern void wait_ms( unsigned int msecs );
extern const unsigned int fpga_msecs_motorsStopped;

static float z_angle = 0;
static float z_velocity = 0;
#if INTEGRATE
static float z_angle2 = 0;
#endif
	
static float _MPU6000_bias = 0.0f;
static float _MPU6000_scale = 1.0f;

static const signed char gyro_orientation[9] = { 1, 0, 0,
												 0, 1, 0,
												 0, 0, 1 };

static unsigned char gyro_calibration_enabled = 0;

static unsigned short inv_row_2_scale(const signed char *row)
{
    unsigned short b;

    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7;      // error
    return b;
}

static unsigned short inv_orientation_matrix_to_scalar(
    const signed char *mtx)
{
    unsigned short scalar;

    /*
       XYZ  010_001_000 Identity Matrix
       XZY  001_010_000
       YXZ  010_000_001
       YZX  000_010_001
       ZXY  001_000_010
       ZYX  000_001_010
     */

    scalar = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;


    return scalar;
}

static void run_self_test(void)
{
    int result;
    long gyro[3], accel[3];

    result = mpu_run_self_test(gyro, accel);
    if (result & 0x1) {
        /* Test passed. We can trust the gyro data here, so let's push it down
         * to the DMP.
         */
        float sens;
        mpu_get_gyro_sens(&sens);
        gyro[0] = (long)(gyro[0] * sens);
        gyro[1] = (long)(gyro[1] * sens);
        gyro[2] = (long)(gyro[2] * sens);
        dmp_set_gyro_bias(gyro);
#ifdef PRINTF_DEBUG
		printf( "MPU self test success\n\r" );
#endif
    }
	else
		printf( "MPU self test failed\n\r" );
}

#if PRINT_EULER && defined PRINTF_DEBUG
#if 0
static void quaternionToEuler( const float* quat_wxyz, float* x, float* y, float* z )
{
	// good reference: http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
	float test;
	const struct quat { float w, x, y, z; } *q = ( const struct quat* )quat_wxyz;

	float sqx = q->x * q->x;
	float sqy = q->y * q->y;
	float sqz = q->z * q->z;

	test = q->x * q->y + q->z * q->w;

	if( test > 0.5f - EPSILON )
	{
		*x = 2.f * atan2( q->x, q->w );
		*y = PI_2;
		*z = 0;
	}
	else if( test < -0.5f + EPSILON )
	{
		*x = -2.f * atan2( q->x, q->w );
		*y = -PI_2;
		*z = 0;
	}
	else
	{
		*x = atan2( 2.f * ( q->y * q->w - q->x * q->z ), 1.f - 2.f * ( sqy - sqz ) );
		*y = asin( 2.f * test );
		*z = atan2( 2.f * ( q->x * q->w - q->y * q->z ), 1.f - 2.f * ( sqx - sqz ) );
	}
}
#else
static void quaternionToEuler( const float* quat_wxyz, float* x, float* y, float* z )
{
	float test;
	const struct quat { float w, x, y, z; } *q = ( const struct quat* )quat_wxyz;

	float sqy = q->y * q->y;
	float sqz = q->z * q->z;
	float sqw = q->w * q->w;

	test = q->x * q->z - q->w * q->y;

	if( test > 0.5f - EPSILON )
	{
		*x = 2.f * atan2( q->y, q->w );
		*y = PI_2;
		*z = 0;
	}
	else if( test < -0.5f + EPSILON )
	{
		*x = -2.f * atan2( q->y, q->w );
		*y = -PI_2;
		*z = 0;
	}
	else
	{
		*x = atan2( 2.f * ( q->x * q->w + q->y * q->z ), 1.f - 2.f * ( sqz + sqw ) );
		*y = asin( 2.f * test );
		*z = atan2( 2.f * ( q->x * q->y - q->z * q->w ), 1.f - 2.f * ( sqy + sqz ) );
	}
}
#endif
#endif

#if 0
static float quaternionToZAngle( const float* quat_wxyz )
{
	// good reference: http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
	return asin( 2.f * ( quat_wxyz[1] /* Qx */ * quat_wxyz[2] /* Qy */ + quat_wxyz[3] /* Qz */ * quat_wxyz[0] /* Qw */ ) );
}
#else
static float quaternionToZAngle( const float* quat_wxyz )
{
	const struct quat { float w, x, y, z; } *q = ( const struct quat* )quat_wxyz;

	if( fabs( q->x * q->z - q->w * q->y ) > 0.5f - EPSILON )
	{
		return 0;
	}
	else
	{
		return atan2( 2.f * ( q->x * q->y - q->z * q->w ), 1.f - 2.f * ( q->y * q->y + q->z * q->z ) );
	}
}
#endif

#ifdef PRINTF_DEBUG
static float rad2deg( float rad )
{
	return 180.f * rad / PI;
}
#endif

void MPU6000_init( void )
{
	struct int_param_s int_param;

	z_angle = 0;
	z_velocity = 0;
	
	setBit( MPU6000_CS_DIR, MPU6000_CS_BIT );
	setBit( MPU6000_CS_SET, MPU6000_CS_BIT );
	
#if USE_SSP1
	printf( "SSP%u init\n\r", SSP_CH );
	ssp_init( SSP_CH, SSP_DSS_8Bit, SSP_FRF_SPI, SSP_CPOL_LOW, SSP_CPHA_1ST, 1000000 );
#else
	printf( "I2C init\n\r" );
	i2c_init( 0, 400000 );
#endif
	int_param.spi = USE_SSP1;

	EXEC( mpu_init( &int_param ), "MPU init" );
	EXEC( mpu_set_sensors( SENSORCFG ), "MPU set XYZ gyro" );
	EXEC( mpu_set_sample_rate( SAMPLE_RATE ), "MPU set sample rate" );
	EXEC( mpu_set_gyro_fsr( FSR ), "MPU set gyroscope full scale range" );
	EXEC( dmp_load_motion_driver_firmware(), "MPU load DMP firmware" );
	EXEC( mpu_configure_fifo( SENSORCFG ), "MPU configure fifo" );
    EXEC( dmp_set_orientation( inv_orientation_matrix_to_scalar( gyro_orientation ) ), "MPU set orientation" );
	
	EXEC( dmp_enable_feature( DMP_FEATURE_LP_QUAT | DMP_FEATURE_SEND_CAL_GYRO/* | DMP_FEATURE_GYRO_CAL*/ ), "MPU set DMP features" );
	//gyro_enable_calibration();

	EXEC( mpu_set_sample_rate( SAMPLE_RATE ), "MPU set sample rate" );
    EXEC( dmp_set_fifo_rate( FIFO_RATE ), "MPU set DMP fifo rate" );
    EXEC( mpu_set_dmp_state( 1 ), "MPU enable DMP" );
	run_self_test();
}

unsigned char MPU6000_update( void )
{
	const int rateWindowSize = 10;
	static float rateWindow[rateWindowSize] = {0,0,0,0,0,0,0,0,0,0};
	static int rateWindowIndex = 0;
	int i;

	short gyro[3], accel[3], sensors;
	unsigned char more;
	long quat[4];
	unsigned long timestamp = 0;
	
	if ( gyroTimer_elapsed() < 10 )
	{
		return 0;
	}
	
	gyroTimer_start();
	
	if ( 0 == dmp_read_fifo( gyro, accel, quat, &timestamp, &sensors, &more ) )
	{
		if ( ( sensors & INV_XYZ_GYRO ) && ( sensors & INV_WXYZ_QUAT ) )
		{
#if INTEGRATE
#else
			float fQuat[4];
			int i;
			for( i = 0; i < 4; ++i )
				fQuat[i] = (float)quat[i] / QUAT_SENS;
			z_angle = quaternionToZAngle( fQuat );
#endif
			z_velocity = (float)gyro[2] / GYRO_SENS;
			
			if( gyro_data_size < GYRO_DATA_MAX_SIZE )
			{
				gyro_data_stamps[gyro_data_size] = gyro_stamp;
				gyro_data_omegas[gyro_data_size] = z_velocity;
				++gyro_data_size;
			}
			
			rateWindow[rateWindowIndex] = z_velocity;
			++rateWindowIndex;
			if( rateWindowIndex == rateWindowSize )
			{
				rateWindowIndex = 0;
			}
			
			z_velocity = 0;
			for( i=0; i<rateWindowSize; ++i )
			{
				z_velocity += rateWindow[i];
			}
			z_velocity /= (float)rateWindowSize;
			
			z_velocity = deg2rad( z_velocity );
#if INTEGRATE
			if( fpga_msecs_motorsStopped > 500 )
			{
				_MPU6000_bias = z_velocity;
			}
			z_velocity -= _MPU6000_bias;
			z_velocity *= _MPU6000_scale;
			
			z_angle2 -= z_velocity * gyro_deltat;
#endif
#ifdef PRINTF_DEBUG
#	if PRINT_GYRO
			printf( "GYRO x = %d\ty = %d\tz = %d\n\r", gyro[0], gyro[1], gyro[2]);
#	endif
#	if PRINT_QUAT
			printf( "QUAT w = %f\tx = %f\ty = %f\tz = %f\n\r", fQuat[0], fQuat[1], fQuat[2], fQuat[3]);
#	endif
#	if PRINT_EULER
			{
				float x, y, z;
				quaternionToEuler( fQuat, &x, &y, &z );
				printf( "x %f y %f z %f\n\r", rad2deg( x ), rad2deg( y ), rad2deg( z ) );
			}
#	endif
#if INTEGRATE
			printf( "angle 1 %f?\tangle 2 %f?\tvelocity %f?/s\n\r", rad2deg( z_angle ), rad2deg( z_angle2 ), rad2deg( z_velocity ) );
#else
			printf( "angle %f?\tvelocity %f?/s\n\r", rad2deg( z_angle ), rad2deg( z_velocity ) );
#endif
#endif
		}
#ifdef PRINTF_DEBUG
		else
		{
			printf( "MPU No new data\n\r" );
		}
#endif
	}
//#ifdef PRINTF_DEBUG
	else
	{
		printf( "MPU read FIFO failed\n\r" );
	}
//#endif
	
	return 1;
}

void MPU6000_enable_calibration( void )
{
	if( 0 == gyro_calibration_enabled )
	{
		if( 0 == dmp_enable_gyro_cal(1) )
		{
			printf("Gyro calibration enabled\n\r");
			gyro_calibration_enabled = 1;
		}
		else
		{
			printf("Gyro: Failed to enable calibration\n\r");
		}
	}
}

void MPU6000_disable_calibration( void )
{
	if( 1 == gyro_calibration_enabled )
	{
		if( 0 == dmp_enable_gyro_cal(0) )
		{
			printf("Gyro calibration disabled\n\r");
			gyro_calibration_enabled = 0;
		}
		else
		{
			printf("Gyro: Failed to disable calibration\n\r");
		}
	}
}

float MPU6000_z_angle( void )
{
#if INTEGRATE
	while( z_angle2 < -PI ) { z_angle2 += PI2; };
	while( z_angle2 > PI ) { z_angle2 -= PI2; };
	return z_angle2;
#else
	return z_angle;
#endif
}

float MPU6000_z_velocity( void )
{
	return z_velocity;
}

float MPU6000_bias( void )
{
	return _MPU6000_bias;
}

float MPU6000_scale( void )
{
	return _MPU6000_scale;
}

void MPU6000_set_scale( float scale )
{
	_MPU6000_scale = scale;
	printf("MPU6000 set scale %f\n\r", scale);
}

unsigned int MPU6000_readBuf( unsigned char slaveAddr, unsigned char reg, unsigned char* buffer, unsigned int length )
{
#if USE_SSP1
	ssp_select_gyro();
	ssp_readBuf( SSP_CH, reg, buffer, length );
	ssp_deselect_gyro();
	return 0;
#else
	return i2c_readBuf( 0, slaveAddr, reg, buffer, length );
#endif
}

unsigned int MPU6000_writeBuf( unsigned char slaveAddr, unsigned char reg, const unsigned char* buffer, unsigned int length )
{
#if USE_SSP1
	ssp_select_gyro();
	ssp_writeBuf( SSP_CH, reg, buffer, length );
	ssp_deselect_gyro();
	return 0;
#else
	return i2c_writeBuf( 0, slaveAddr, reg, buffer, length );
#endif
}
