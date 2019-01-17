#include "util.h"
#include <LPC23XX.H>

extern unsigned int emergency_bumper_counter;

void pwm_port_init( void );

//void PWM1_isr(void)  __irq
//{
//  PWM1IR       |= 0x00000001;               /* Clear match0 interrupt */
//  VICVectAddr  = 0x00000000;

//  clearBit( PWM1MCR, 0 );

//  pwm_port_init();
//}

void pwm_port_init( void )
{
#ifndef MCB2300
	//PWM1 -> P3.24
	setBit2( PINSEL7, 17, 16 );

	//PWM2 -> P3.25
	setBit2( PINSEL7, 19, 18 );

	//PWM3 -> P3.26
	setBit2( PINSEL7, 21, 20 );

	//PWM4 -> P2.3
	clearBit( PINSEL4, 7 );
	setBit( PINSEL4, 6 );

	//PWM5 -> P1.24

	//PWM6 -> P1.26
#endif
}

void pwm_init( void )
{
	//VICVectAddr8 = (unsigned)PWM1_isr;        /* Set the PWM ISR vector address */
	//VICVectCntl8 = 0x00000028;                /* Set channel */
	//VICIntEnable = 0x00000100;                /* Enable the interrupt */ 

	//setBit( PWM1MCR, 0 ); //Interrupt on PWMMR0: an interrupt is generated when PWMMR0 matches the value in the PWMTC.

	pwm_port_init();

	PWM1MCR = 0x00000003;                      /* On match with timer reset the counter */

	//count up to 255
	PWM1MR0 = 255;
	PWM1MR1 = 0;
	PWM1MR2 = 0;
	PWM1MR3 = 0;
	PWM1MR4 = 0;
	PWM1MR5 = 0;
	PWM1MR6 = 0;

	PWM1TCR = 0x00000002;                      /* Reset counter and prescaler           */ 
	PWM1TCR = 0x00000009;                      /* enable counter and PWM, release counter from reset */

	//enable PWM mode
	setBit( PWM1TCR, 3 );

	//enable PWM
	setBit( PWM1TCR, 0 );

	//enable PWM1 output
	setBit( PWM1PCR, 9 );

	//enable PWM2 output
	setBit( PWM1PCR, 10 );

	//enable PWM3 output
	setBit( PWM1PCR, 11 );

	//enable PWM4 output
	setBit( PWM1PCR, 12 );

	//enable PWM5 output
	setBit( PWM1PCR, 13 );

	//enable PWM6 output
	setBit( PWM1PCR, 14 );
}

unsigned char pwm_get( unsigned char channel )
{
	switch( channel )
	{
	case 1:
		return PWM1MR1;

	case 2:
		return PWM1MR2;

	case 3:
		return PWM1MR3;

	case 4:
		return PWM1MR4;

	case 5:
		return PWM1MR5;

	case 6:
		return PWM1MR6;

	default:
		return 0;
	}
}

/**
@param channel valid values are 1,2,3,4,5,6
@param ratio The on/off ratio
0->constant low
255->constant high
*/
void pwm_set( unsigned char channel, unsigned char value )
{
	if( emergency_bumper_counter > 0 )
	{
		value = 0;
	}

	switch( channel )
	{
	case 1:
		PWM1MR1 = value;
		break;

	case 2:
		PWM1MR2 = value;
		break;

	case 3:
		PWM1MR3 = value;
		break;

	case 4:
		PWM1MR4 = value;
		break;

	case 5:
		PWM1MR5 = value;
		break;

	case 6:
		PWM1MR6 = value;
		break;

	default:
		return;
	}

	setBit( PWM1LER, channel );
}
