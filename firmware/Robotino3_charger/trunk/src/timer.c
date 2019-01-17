#include "timer.h"

#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"

#define TICKS_PER_10MILLISEC (SystemCoreClock/100)
#define THRESHOLD ( SYSTICK_MAXCOUNT - TICKS_PER_10MILLISEC )
#define SYSTICK_MAXCOUNT 0xFFFFFF

#define MAINTIMER	0

extern uint32_t SystemCoreClock;

uint32_t milliTimer_ref[1] = { 0 };
uint32_t microTimer_ref[1] = { 0 };

static uint32_t mscounter = 0;
static uint32_t uscounter = 0;

#define MSTIMER_START( num )\
milliTimer_ref[num] = mscounter + ( TIM_GetCounter( TIM2 ) / 1000 )

#define MICROTIMER_START( num )\
microTimer_ref[num] = uscounter + TIM_GetCounter( TIM2 )

#define MSTIMER_ELASPED( num )\
uint32_t currentTime = mscounter + ( TIM_GetCounter( TIM2 ) / 1000 );\
if( currentTime >= milliTimer_ref[num] )\
{\
	return currentTime - milliTimer_ref[num];\
}\
else\
{\
	return 0xFFFFFFFF - ( milliTimer_ref[num] - currentTime ) + 1;\
}

#define MICROTIMER_ELASPED( num )\
uint32_t currentTime = uscounter + TIM_GetCounter( TIM2 );\
if( currentTime >= microTimer_ref[num] )\
{\
	return currentTime - microTimer_ref[num];\
}\
else\
{\
	return 0xFFFFFFFF - ( microTimer_ref[num] - currentTime ) + 1;\
}

void TIM2_Istr()
{
	if ( TIM_GetITStatus( TIM2, TIM_IT_Update ) == SET )
	{
		TIM_ClearITPendingBit( TIM2, TIM_IT_Update );
		mscounter += 10;
		uscounter += 10000;
	}
}

void timer_init()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );

	TIM_TimeBaseStructInit( &TIM_TimeBaseStructure );

	TIM_TimeBaseStructure.TIM_Period = 9999;
	TIM_TimeBaseStructure.TIM_Prescaler = 71;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit( TIM2, &TIM_TimeBaseStructure );
	TIM_PrescalerConfig( TIM2, 71, TIM_PSCReloadMode_Immediate );

	TIM_ITConfig( TIM2, TIM_IT_Update, ENABLE );

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init( &NVIC_InitStructure );

	mscounter = 0;
	uscounter = 0;

	TIM_Cmd( TIM2, ENABLE );
}

void mainTimer_start( void )
{
	MSTIMER_START( MAINTIMER );
}

uint32_t mainTimer_elapsed( void )
{
	MSTIMER_ELASPED( MAINTIMER );
}

static __inline void sleep_impl( uint32_t delay, uint32_t factor )
{
	//SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	SysTick->LOAD	=	SYSTICK_MAXCOUNT;	// Value to fire interrupt
											// (meaningless because we do not use the interrupt, but maybe necessary)
	SysTick->VAL	=	SYSTICK_MAXCOUNT;	// SysTick timer counts down
	SysTick->CTRL	=	SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;	// Enable SysTick timer but not its interrupt.
	while( delay >= factor )	// Count down in 10ms steps until delay < 10ms
	{
		if ( SysTick->VAL <= THRESHOLD )
		{
			SysTick->VAL = SYSTICK_MAXCOUNT;
			delay -= factor;
		}
	}
	if ( delay > 0 )	// If there is some delay left...
	{
		SysTick->VAL = SYSTICK_MAXCOUNT;
		delay = SYSTICK_MAXCOUNT - delay * TICKS_PER_10MILLISEC / factor;
		while( SysTick->VAL >= delay )
		{
		}
	}
	SysTick->CTRL	=	0;	// Disable SysTick timer.
}

void usleep( uint32_t usecs )
{
	sleep_impl( usecs, 10000 );
}

void msleep( uint32_t msecs )
{
	sleep_impl( msecs, 10 );
}
