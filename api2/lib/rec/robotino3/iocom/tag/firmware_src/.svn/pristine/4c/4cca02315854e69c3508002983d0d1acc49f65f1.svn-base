#ifndef _REC_ROBOTINO3_IOCOM_TAG_TAGENUM_H_
#define _REC_ROBOTINO3_IOCOM_TAG_TAGENUM_H_

typedef enum {
	TAG_GET_HW_VERSION						=	1,
	TAG_HW_VERSION							=	2,
	TAG_GET_SW_VERSION						=	3,
	TAG_SW_VERSION							=	4,
	TAG_GET_DISTANCE_SENSOR_READINGS		=	5,
	TAG_DISTANCE_SENSOR_READINGS			=	6,
	TAG_GET_ADC_PARAMETERS					=	7,
	TAG_ADC_PARAMETERS						=	8,
	TAG_SET_MOTOR_SPEED						=	9,
	TAG_GET_ALL_MOTOR_SPEEDS				=	10,
	TAG_ALL_MOTOR_SPEEDS					=	11,
	TAG_SET_MOTOR_POSITION					=	12,
	TAG_GET_ALL_MOTOR_POSITIONS				=	13,
	TAG_ALL_MOTOR_POSITIONS					=	14,
	TAG_SET_MOTOR_PID_PARAMETERS			=	15,
	TAG_GET_ALL_MOTOR_PID_PARAMETERS		=	16,
	TAG_ALL_MOTOR_PID_PARAMETERS			=	17,
	TAG_SET_ALL_DIGITAL_OUTPUTS				=	18,
	TAG_SET_ALL_RELAYS						=	19,
	TAG_SET_ODOMETRY						=	20,
	TAG_SET_ODOMETRY_ROTATION				=	21,
	TAG_GET_ODOMETRY						=	22,
	TAG_ODOMETRY							=	23,
	TAG_GET_ALL_MOTOR_CURRENT_READINGS		=	26,
	TAG_ALL_MOTOR_CURRENT_READINGS			=	27,
	TAG_GET_ALL_ANALOG_INPUTS				=	32,
	TAG_ALL_ANALOG_INPUTS					=	33,
	TAG_GET_ALL_DIGITAL_INPUTS				=	34,
	TAG_ALL_DIGITAL_INPUTS					=	35,
	TAG_GET_BUMPER							=	36,
	TAG_BUMPER								=	37,
	TAG_GET_POWER_BUTTON					=	38,
	TAG_POWER_BUTTON						=	39,
	TAG_SET_FPGA_POWER						=	40,
	TAG_GET_FPGA_POWER						=	41,
	TAG_FPGA_POWER							=	42,
	TAG_GET_PWR_OK_STATE					=	43,
	TAG_PWR_OK_STATE						=	44,
	TAG_SET_PWR_OK_STATE					=	45,
	TAG_SET_PWM								=	46,
	TAG_SET_MOTOR_ON						=	47,
	TAG_SET_PWRBTN							=	48,
	TAG_SET_SYS_RESET						=	49,
	TAG_GET_COM_EXPRESS_STATES				=	50,
	TAG_COM_EXPRESS_STATES					=	51,
	TAG_GET_ALL_MOTOR_READINGS				=	52,
	TAG_ALL_MOTOR_READINGS					=	53,
	TAG_GET_IP_ADDRESS						=	54,
	TAG_IP_ADDRESS							=	55,
	TAG_SET_IP_ADDRESS						=	56,
	TAG_SET_EMERGENCY_BUMPER				=	57,
	TAG_SET_MOTOR_MODE						=	58,
	TAG_RESET_LPC							=	59,
	TAG_POWER_OFF							=	60,
	TAG_SET_SAFTEY_VELOCITY					=	61,	
	TAG_GET_SAFTEY_VELOCITY					=	62,	
	TAG_SAFTEY_VELOCITY						=	63,	
	TAG_POWER_SOURCE_READINGS				=	65,
	TAG_SET_MOTOR_ACCEL_LIMITS				=	66,
	TAG_MOTOR_ACCEL_LIMITS					=	67,
	TAG_GET_MOTOR_ACCEL_LIMITS				=	68,
	TAG_GET_GYRO_Z_ANGLE					=	69,
	TAG_GYRO_Z_ANGLE						=	70,
	TAG_GET_CAN_MSG							=	71,
	TAG_CAN_MSG								=	72,
	TAG_SET_NRST							=	73,
	TAG_GET_NRST							=	74,
	TAG_NRST								=	75,
	TAG_SET_BOOT							=	76,
	TAG_GET_BOOT							=	77,
	TAG_BOOT								=	78,
	TAG_CONFIG_RESET						=	79,
	TAG_CHARGER_INFO						=	80,
	TAG_CHARGER_EVENT						=	81,
	TAG_CHARGER_VERSION						=	82,
	TAG_CHARGER_GET_VERSION					=	83,
	TAG_CHARGER_CLEAR_ERROR					=	84,
	TAG_CHARGER_ERROR						=	85,
	TAG_SET_BATTERY_MIN						=	86,
	TAG_GET_BATTERY_MIN						=	87,
	TAG_BATTERY_MIN							=	88,
	TAG_GET_GYRODATA						=	89,
	TAG_GYRODATA							=	90,
	TAG_GET_GPAIN							= 91,
	TAG_GPAIN									= 92,
	TAG_GET_VERSIONBITS				= 93,
	TAG_VERSIONBITS						= 94,
	TAG_GYRO_SET_PARAM				= 95,
	TAG_GYRO_GET_PARAM				= 96,
	TAG_GYRO_PARAM						= 97,
	
	TAG_FESTOOL_CHARGER_INFO = 98,
	/*
	Byte  Bit   Description
	0-3   0-31  Time
	4           Accu load
	      0     High -> Accu 0 is loading
				1     High -> Accu 1 is loading
				2     High -> Accu 2 is loading
				3     High -> Accu 3 is loading
				4     High -> Accu 0 is connected to system
				5     High -> Accu 1 is connected to system
				6     High -> Accu 2 is connected to system
				7     High -> Accu 3 is connected to system
	5
				0			High -> External power active
				1			High -> Charger present
	6-7   0-15  Accu 0 voltage in mV
	8-9   0-15  Accu 1 voltage in mV
	10-11   0-15  Accu 2 voltage in mV
	12-13 0-15  Accu 3 voltage in mV
	14					Accu 0 capacity in %
	15					Accu 1 capacity in %
	16					Accu 2 capacity in %
	17					Accu 3 capacity in %
	...					Messages
	*/
	
	TAG_FESTOOL_CHARGER_HOTSWAP_START = 99,
	/*
	Empty Tag. Tells Festool charger to stop measuring single batteries
	so that all batteries are available for powering the system.
	*/
	
	TAG_MOTOR_DEBUG = 100,
	/*
	Byte  Bit   Description
	0		  		  Motor number
	1-4					Start time
	5						Time delta in 0.1ms
	6-7					Data0: Speed setpoint in rpm (signed short)
	8-9					Data0: Speed in rpm (signed short)
	10-11					Data0: Current in mA (signed short)
	11-12				Data0: control point (signed short)
	...
	*/
	
	TAG_SET_MOTOR_DEBUG = 101,
	/*
	Byte  Bit   Description
	0						Enable motor debugging
							0 - debugging off
							1 - debug motor 0
							2 - debug motor 1
							3 - debug motor 2
							4 - debug motor 3
	*/
	
	TAG_INFO								=	250,
	TAG_WARNING								=	251,
	TAG_ERROR								=	252,
	TAG_LINGO_A								=	253,
	TAG_LINGO_B								=	254,
	TAG_LINGO_C								=	255
} Tag_Value_t;

typedef enum {
	GYRO_PARAM_BIAS,
	GYRO_PARAM_SCALE
} Gyro_Param_t;

#endif //_REC_ROBOTINO3_IOCOM_TAG_TAGENUM_H_
