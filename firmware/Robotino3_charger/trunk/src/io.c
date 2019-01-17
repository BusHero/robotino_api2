#include "io.h"

#include "stm32f10x_gpio.h"
#include <stdbool.h>

static GPIO_TypeDef* LED_PORT = GPIOB;
static uint16_t LED_PINS[2] = { 14, 13 };

static uint8_t iter = 0;

static __inline void io_init_output( GPIO_TypeDef* gpio, uint16_t pin, GPIOMode_TypeDef mode )
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 
	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = (1 << pin );
	GPIO_InitStructure.GPIO_Mode = mode; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(gpio, &GPIO_InitStructure);
}

static __inline void io_init_input( GPIO_TypeDef* gpio, uint16_t pin, GPIOMode_TypeDef mode )
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 
	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = ( 1 << pin );
	GPIO_InitStructure.GPIO_Mode = mode; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(gpio, &GPIO_InitStructure);
}

static __inline void io_set_output( GPIO_TypeDef* gpio, uint16_t pin, uint8_t state )
{
	if ( state )
		gpio->BSRR = ( 1 << pin );
	else
		gpio->BRR = ( 1 << pin );
}

static __inline uint8_t io_get_input( GPIO_TypeDef* gpio, uint16_t pin )
{
	return GPIO_ReadInputDataBit(gpio, ( 1 << pin ));
}

void io_init(void)
{
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
							RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG, ENABLE );

	io_init_output( LED_PORT, LED_PINS[0], GPIO_Mode_Out_PP );
	io_init_output( LED_PORT, LED_PINS[1], GPIO_Mode_Out_PP );
}

void io_led_on( unsigned char lednum )
{
	io_set_output( LED_PORT, LED_PINS[lednum - 1], 0 );
}

void io_led_off( unsigned char lednum )
{
	io_set_output( LED_PORT, LED_PINS[lednum - 1], 1 );
}

void io_led_cnt( void )
{
	if ( iter & 1 )
	{
		io_led_on( 1 );
	}
	else
	{
		io_led_off( 1 );
	}
	if ( iter & 2 )
	{
		io_led_on( 2 );
	}
	else
	{
		io_led_off( 2 );
	}
	++iter;
	if ( iter > 3 )
	{
		iter = 0;
	}
}
