#ifndef _GRIPPERCOM_TAGENUM_H_
#define _GRIPPERCOM_TAGENUM_H_

typedef enum {
	TAG_GET_HW_VERSION						=	1,
	TAG_HW_VERSION							=	2,
	TAG_GET_SW_VERSION						=	3,
	TAG_SW_VERSION							=	4,
	TAG_RESET_LPC							=	5,
	TAG_GET_SENSORREADINGS					=	6,
	TAG_SENSORREADINGS						=	7,
	TAG_SET_PWM								=	8,
	TAG_SET_LED								=	9,
	TAG_ERROR								=	255
} Tag_Value_t;

#endif // _GRIPPERCOM_TAGENUM_H_
