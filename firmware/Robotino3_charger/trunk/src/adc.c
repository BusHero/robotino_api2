#include "adc.h"

#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#define ADC_INT_IVINMON	0
#define ADC_INT_ISMON	1
#define ADC_INT_U_AKKU	2
#define ADC_INT_NTC1	3
#define ADC_INT_NTC2	8
#define ADC_INT_VAREF	9

static const unsigned int channels[6] = { 0, 1, 2, 3, 8, 9 };

static unsigned short adcval_raw[6] = { 0, 0, 0, 0, 0, 0 };
static float adcval[6] = { 0, 0, 0, 0, 0, 0 };

static const float varef = 2.5f / 3.f;

static unsigned short adc_read_raw( unsigned int channel )
{
	if ( channel > 9 || ( channel > 3 && channel < 8 ) )
		return 0;

	ADC_RegularChannelConfig( ADC1, (uint8_t)channel, 1, ADC_SampleTime_28Cycles5 );
	ADC_SoftwareStartConvCmd( ADC1, ENABLE );
	while( ADC_GetFlagStatus( ADC1, ADC_FLAG_EOC ) != SET );
	return ADC_GetConversionValue( ADC1 );
}

void adc_init( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE );
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Init( GPIOA, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init( ADC1, &ADC_InitStructure );

	ADC_Cmd(ADC1, ENABLE);  

	/* Enable ADC reset calibaration register */   
	ADC_ResetCalibration( ADC1 );
	/* Check the end of ADC reset calibration register */
	while( ADC_GetResetCalibrationStatus( ADC1 ) );

	/* Start ADC calibaration */
	ADC_StartCalibration( ADC1 );
	/* Check the end of ADC calibration */
	while( ADC_GetCalibrationStatus( ADC1 ) );
}

void adc_update( void )
{
	int i;
	float corr_factor;
	for( i = 0; i < 6; ++i )
	{
		adcval_raw[i] = adc_read_raw( channels[i] );
	}
	corr_factor = varef / ( (float)adcval_raw[ADC_CH_VAREF] /*/ 4095.f * 3.3f */);
	for( i = 0; i < 6; ++i )
	{
		adcval[i] = (float)adcval_raw[i] * corr_factor /*/ 4095.f * 3.3f */;
	}
}

float adc_get_value( unsigned int channel )
{
	if ( channel > 5 )
		return 0;
	return adcval[channel];
}

unsigned short adc_get_raw_value( unsigned int channel )
{
	if ( channel > 5 )
		return 0;
	return adcval_raw[channel];
}

