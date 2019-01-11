#ifndef _CLK_H_
#define _CLK_H_

#define MAIN_CLK	12000000 // Main oscillator's frequency. In most cases 12MHz. If not, please change!

// Don't change the lines below!

#include <LPC23XX.H>

#define PLL_M		( ( PLLCFG & 0x7FFF ) + 1 )
#define PLL_N		( ( ( PLLCFG >> 16 ) & 0xFF ) + 1 )
#define PLL_CLK		( 2 * MAIN_CLK * PLL_M / PLL_N )
#define CPU_CLK		( PLL_CLK / ( ( CCLKCFG & 0xFF ) + 1 ) )

#define PCLKSEL_WDT		(   PCLKSEL0         & 0x03 )
#define PCLKSEL_TIMER0	( ( PCLKSEL0 >> 02 ) & 0x03 )
#define PCLKSEL_TIMER1	( ( PCLKSEL0 >> 04 ) & 0x03 )
#define PCLKSEL_UART0	( ( PCLKSEL0 >> 06 ) & 0x03 )
#define PCLKSEL_UART1	( ( PCLKSEL0 >> 08 ) & 0x03 )
#define PCLKSEL_PWM1	( ( PCLKSEL0 >> 12 ) & 0x03 )
#define PCLKSEL_I2C0	( ( PCLKSEL0 >> 14 ) & 0x03 )
#define PCLKSEL_SPI		( ( PCLKSEL0 >> 16 ) & 0x03 )
#define PCLKSEL_RTC		( ( PCLKSEL0 >> 18 ) & 0x03 )
#define PCLKSEL_SSP1	( ( PCLKSEL0 >> 20 ) & 0x03 )
#define PCLKSEL_DAC		( ( PCLKSEL0 >> 22 ) & 0x03 )
#define PCLKSEL_ADC		( ( PCLKSEL0 >> 24 ) & 0x03 )
#define PCLKSEL_CAN1	( ( PCLKSEL0 >> 26 ) & 0x03 )
#define PCLKSEL_CAN2	( ( PCLKSEL0 >> 28 ) & 0x03 )
#define PCLKSEL_AFC		( ( PCLKSEL0 >> 30 ) & 0x03 )
#define PCLKSEL_BAT_RAM	(   PCLKSEL1         & 0x03 )
#define PCLKSEL_GPIO	( ( PCLKSEL1 >> 02 ) & 0x03 )
#define PCLKSEL_PCB		( ( PCLKSEL1 >> 04 ) & 0x03 )
#define PCLKSEL_I2C1	( ( PCLKSEL1 >> 06 ) & 0x03 )
#define PCLKSEL_SSP0	( ( PCLKSEL1 >> 10 ) & 0x03 )
#define PCLKSEL_TIMER2	( ( PCLKSEL1 >> 12 ) & 0x03 )
#define PCLKSEL_TIMER3	( ( PCLKSEL1 >> 14 ) & 0x03 )
#define PCLKSEL_UART2	( ( PCLKSEL1 >> 16 ) & 0x03 )
#define PCLKSEL_UART3	( ( PCLKSEL1 >> 18 ) & 0x03 )
#define PCLKSEL_I2C2	( ( PCLKSEL1 >> 20 ) & 0x03 )
#define PCLKSEL_I2S 	( ( PCLKSEL1 >> 22 ) & 0x03 )
#define PCLKSEL_MCI		( ( PCLKSEL1 >> 24 ) & 0x03 )
#define PCLKSEL_SYSCON	( ( PCLKSEL1 >> 28 ) & 0x03 )

#define GPIO_CLK	( CPU_CLK / PCLK_DIVISOR[PCLKSEL_GPIO] )
#define TIMER0_CLK	( CPU_CLK / PCLK_DIVISOR[PCLKSEL_TIMER0] )
#define TIMER1_CLK	( CPU_CLK / PCLK_DIVISOR[PCLKSEL_TIMER1] )
#define TIMER2_CLK	( CPU_CLK / PCLK_DIVISOR[PCLKSEL_TIMER2] )
#define TIMER3_CLK	( CPU_CLK / PCLK_DIVISOR[PCLKSEL_TIMER3] )
#define UART0_CLK	( CPU_CLK / PCLK_DIVISOR[PCLKSEL_UART0] )
#define UART1_CLK	( CPU_CLK / PCLK_DIVISOR[PCLKSEL_UART1] )
#define UART2_CLK	( CPU_CLK / PCLK_DIVISOR[PCLKSEL_UART2] )
#define UART3_CLK	( CPU_CLK / PCLK_DIVISOR[PCLKSEL_UART3] )
#define CAN1_CLK	( CPU_CLK / PCLK_DIVISOR[PCLKSEL_CAN1] )
#define CAN2_CLK	( CPU_CLK / PCLK_DIVISOR[PCLKSEL_CAN2] )
#define SPI_CLK		( CPU_CLK / PCLK_DIVISOR[PCLKSEL_SPI] )
#define I2C0_CLK	( CPU_CLK / PCLK_DIVISOR[PCLKSEL_I2C0] )
#define I2C1_CLK	( CPU_CLK / PCLK_DIVISOR[PCLKSEL_I2C1] )
#define I2C2_CLK	( CPU_CLK / PCLK_DIVISOR[PCLKSEL_I2C2] )
#define I2S_CLK		( CPU_CLK / PCLK_DIVISOR[PCLKSEL_I2S] )
#define ADC_CLK		( CPU_CLK / PCLK_DIVISOR[PCLKSEL_ADC] )
#define DAC_CLK		( CPU_CLK / PCLK_DIVISOR[PCLKSEL_DAC] )
#define PWM1_CLK	( CPU_CLK / PCLK_DIVISOR[PCLKSEL_PWM1] )
#define RTC_CLK		( CPU_CLK / PCLK_DIVISOR[PCLKSEL_RTC] )
#define WDT_CLK		( CPU_CLK / PCLK_DIVISOR[PCLKSEL_WDT] )
#define SSP0_CLK	( CPU_CLK / PCLK_DIVISOR[PCLKSEL_SSP0] )
#define SSP1_CLK	( CPU_CLK / PCLK_DIVISOR[PCLKSEL_SSP1] )
#define AFC_CLK		( CPU_CLK / PCLK_DIVISOR[PCLKSEL_ARFC] )
#define PCB_CLK		( CPU_CLK / PCLK_DIVISOR[PCLKSEL_PCB] )
#define MCI_CLK		( CPU_CLK / PCLK_DIVISOR[PCLKSEL_MCI] )
#define SYSCON_CLK	( CPU_CLK / PCLK_DIVISOR[PCLKSEL_SYSCON] )
#define BAT_RAM_CLK	( CPU_CLK / PCLK_DIVISOR[PCLKSEL_BAT_RAM] )

extern const unsigned char PCLK_DIVISOR[4];

#endif