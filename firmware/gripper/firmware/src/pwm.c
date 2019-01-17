#include "clk.h"
#include "util.h"

#include <string.h>
#include <stdint.h>

#define PWMCLK  1000000                  /* Timer Clock rate 10^6/s     */
#define PWMCNT (PCLK/PWMCLK)              /* Timer Counts                */

void pwm_init( void )
{
	setBit( PINSEL0, 17 ); //P0.8 -> PWM4
	setBit( PINSEL0, 15 ); //P0.7 -> PWM2

	PWMPR = PWMCNT - 1;						// Prescale so that PWM is increase every micro second
	PWMMR0 = 20000;							// 20ms
	PWMMR2 = 1500;							// 1.5ms
	PWMMR4 = 1500;							// 1.5ms

	PWMPCR = 1<<10 | 1<<12;					//The PWM2 and PWM4 output enabled
	PWMMCR = 1<<1;							//Reset on PWMMR0: the PWMTC will be reset if PWMMR0 matches it.

	PWMTCR = 1<<1; //PWM reset
	PWMTCR = 1 | 1<<3; //Counter and PWM enabled
}

void pwm_set_ratio1( unsigned short ratio )
{
	PWMMR4 = 1000 + ratio;
	setBit( PWMLER, 4 );
}

void pwm_set_ratio2( unsigned short ratio )
{
	PWMMR2 = 1000 + ratio;
	setBit( PWMLER, 2 );
}
