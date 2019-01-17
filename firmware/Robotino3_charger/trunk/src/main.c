/**
******************************************************************************
* @file    Project/Template/main.c
* @author  MCD Application Team
* @version V3.1.0
* @date    06/19/2009
* @brief   Main program body
******************************************************************************
* @copy
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
*/

/* Includes ------------------------------------------------------------------*/
#include "timer.h"
#include "serial.h"
#include "io.h"
#include "can.h"
#include "adc.h"
#include "dac.h"
#include "spi.h"

#include "includes.h"
#include <stdbool.h>

//static unsigned char cnt;
//static unsigned char len;
//static unsigned char txData[8];
static unsigned short dacvalue = 0;
static bool newdacval = false;

void new_dac_value( unsigned short dacval )
{
	dacvalue = dacval;
	newdacval = true;
}

/**
* @brief  Main program.
* @param  None
* @retval None
*/
int main(void)
{
	/* Setup STM32 system (clock, PLL and Flash configuration) */
	SystemInit();

	io_init();

	io_led_on( 1 );
	io_led_on( 2 );

	timer_init();

	serial_init();
	
	can_init();
	
	spi_init();
	
	adc_init();

	printf( "Hello!!!\n\r" );

	io_led_off( 1 );
	io_led_off( 2 );
	
	//cnt = 0;
	//len = 1;
	
	dac_load( 0x0000 );

	while (1)
	{
		if ( mainTimer_elapsed() > 500 )
		{
			mainTimer_start();
			//memset( txData, cnt, 8 );
			//can_send( len, txData );
			//if ( ++len == 9 )
			//	len = 1;
			//++cnt;
			io_led_cnt();
			
			if ( newdacval )
			{
				newdacval = false;
				dac_load( dacvalue );
				printf( "new DAC value %04x\n\r", dacvalue );
			}
			can_send( 2, (unsigned char*)&dacvalue );

			//printf( "dac %04x\n\r", dacval );
			//dac_load( dacval );
			//dacval += 0x04;
			//dacval %= 0x400;
			
			adc_update();
			//printf( "raw %04X %04X %04X %04X %04X %04X\n\r", adc_get_raw_value( 0 ), adc_get_raw_value( 1 ), adc_get_raw_value( 2 ), adc_get_raw_value( 3 ), adc_get_raw_value( 4 ), adc_get_raw_value( 5 ) );
			printf( "flt %f %f %f %f %f %f\n\r", adc_get_value( 0 ), adc_get_value( 1 ), adc_get_value( 2 ), adc_get_value( 3 ), adc_get_value( 4 ), adc_get_value( 5 ) );
		}
	}
}

#ifdef  USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*   where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif
