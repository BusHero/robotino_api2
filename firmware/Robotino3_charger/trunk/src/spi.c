#include "spi.h"

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"

#include "timer.h"

#define CS_GPIO			GPIOA
#define CS_DAC_PIN		GPIO_Pin_8
#define CS_EEPROM_PIN	GPIO_Pin_4

static uint32_t current_cs = SPI_CS_NONE;

void spi_init( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE );

	/* Configure SPI1 pins */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	/* Configure ChipSelect pins */
	GPIO_InitStructure.GPIO_Pin   = CS_DAC_PIN | CS_EEPROM_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init( CS_GPIO, &GPIO_InitStructure );

	/* Configure SPI1 */
	SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial     = 7;

	SPI_Init( SPI1, &SPI_InitStructure );
	
	/* Pull CS pins high */
	CS_GPIO->BSRR = CS_DAC_PIN | CS_EEPROM_PIN;
	current_cs = SPI_CS_NONE;

	/* Enable SPI1 */
	SPI_Cmd( SPI1, ENABLE );
}

void spi_chipSelect( unsigned int dev )
{
	if ( dev == current_cs )
		return;

	CS_GPIO->BSRR = CS_DAC_PIN | CS_EEPROM_PIN;
	usleep( 100 );

	switch( dev )
	{
	case SPI_CS_DAC:
		CS_GPIO->BRR = CS_DAC_PIN;
		break;
	case SPI_CS_EEPROM:
		CS_GPIO->BRR = CS_EEPROM_PIN;
		break;
	case SPI_CS_NONE:
	default:
		;
	}
	current_cs = dev;
	usleep( 100 );
}

void spi_write( unsigned int value )
{
	while ( SPI_I2S_GetFlagStatus( SPI1, SPI_I2S_FLAG_TXE ) == RESET );
	SPI_I2S_SendData( SPI1, value & 0xFFFF );
}

void spi_read( unsigned int* value )
{
	if ( 0 == value )
		return;

	while ( SPI_I2S_GetFlagStatus( SPI1, SPI_I2S_FLAG_RXNE ) == RESET );
	*value = SPI_I2S_ReceiveData( SPI1 );
}
