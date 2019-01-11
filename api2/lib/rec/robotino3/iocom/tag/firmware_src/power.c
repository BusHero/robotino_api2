#include <LPC23XX.H>
#include "util.h"

#define COVER_PWR_BTN_BIT 25
#define COVER_PWR_BTN_PIN IOPIN0
#define COVER_PWR_BTN_DIR IODIR0
#define COVER_PWR_BTN_SET IOSET0
#define COVER_PWR_BTN_CLR IOCLR0

#define PWR_OK_BIT 12
#define PWR_OK_PIN IOPIN0
#define PWR_OK_DIR IODIR0
#define PWR_OK_SET IOSET0
#define PWR_OK_CLR IOCLR0

#define PWR_OK_CLK_BIT 16
#define PWR_OK_CLK_PIN IOPIN0
#define PWR_OK_CLK_DIR IODIR0
#define PWR_OK_CLK_SET IOSET0
#define PWR_OK_CLK_CLR IOCLR0

#define PWRBTN_BIT 23
#define PWRBTN_PIN IOPIN1
#define PWRBTN_DIR IODIR1
#define PWRBTN_SET IOSET1
#define PWRBTN_CLR IOCLR1

#define COVER_PWR_BTN_LED_BIT 26
#define COVER_PWR_BTN_LED_PIN IOPIN0
#define COVER_PWR_BTN_LED_DIR IODIR0
#define COVER_PWR_BTN_LED_SET IOSET0
#define COVER_PWR_BTN_LED_CLR IOCLR0

#define SYSRESET_BIT 25
#define SYSRESET_PIN IOPIN1
#define SYSRESET_DIR IODIR1
#define SYSRESET_SET IOSET1
#define SYSRESET_CLR IOCLR1

#define BATTX_ON_BIT 6
#define BATTX_ON_PIN IOPIN0
#define BATTX_ON_DIR IODIR0
#define BATTX_ON_SET IOSET0
#define BATTX_ON_CLR IOCLR0

#define CONFIG_RESET_BTN_BIT 24
#define CONFIG_RESET_BTN_PIN IOPIN0
#define CONFIG_RESET_BTN_DIR IODIR0
#define CONFIG_RESET_BTN_SET IOSET0
#define CONFIG_RESET_BTN_CLR IOCLR0

extern void sendline( const char* str );
extern int sendchar (int ch);

void io_power_on( void );

void power_init( void )
{
	setBit( BATTX_ON_DIR, BATTX_ON_BIT );

	setBit( PWR_OK_DIR, PWR_OK_BIT );

	setBit( PWR_OK_CLK_SET, PWR_OK_CLK_BIT );
	setBit( PWR_OK_CLK_DIR, PWR_OK_CLK_BIT );

	setBit( SYSRESET_SET, SYSRESET_BIT );
	setBit( SYSRESET_DIR, SYSRESET_BIT );

	setBit( PWRBTN_SET, PWRBTN_BIT );
	setBit( PWRBTN_DIR, PWRBTN_BIT );

	sendline( "Power Button:" );
	sendchar( 0x30 + bitIsSet( PWRBTN_PIN, PWRBTN_BIT ) );
	sendchar( '\n' );
	sendchar( '\r' );

	sendline( "SysReset:" );
	sendchar( 0x30 + bitIsSet( SYSRESET_PIN, SYSRESET_BIT ) );
	sendchar( '\n' );
	sendchar( '\r' );

	setBit( COVER_PWR_BTN_LED_DIR, COVER_PWR_BTN_LED_BIT );

	io_power_on();
}

void power_button_led_on( void )
{
	setBit( COVER_PWR_BTN_LED_SET, COVER_PWR_BTN_LED_BIT );
}

void power_button_led_off( void )
{
	setBit( COVER_PWR_BTN_LED_CLR, COVER_PWR_BTN_LED_BIT );
}

void power_button_led_toggle( void )
{
	if( bitIsSet( COVER_PWR_BTN_LED_PIN, COVER_PWR_BTN_LED_BIT ) )
	{
		power_button_led_off();
	}
	else
	{
		power_button_led_on();
	}
}

unsigned char cover_pwr_btn_is_pressed( void )
{
	return bitIsSet( COVER_PWR_BTN_PIN, COVER_PWR_BTN_BIT );
}

void set_power_ok_high( void )
{
	setBit( PWR_OK_SET, PWR_OK_BIT );
	setBit( PWR_OK_CLK_CLR, PWR_OK_CLK_BIT );
	setBit( PWR_OK_CLK_SET, PWR_OK_CLK_BIT );
}

void set_power_ok_low( void )
{
	setBit( PWR_OK_CLR, PWR_OK_BIT );
	setBit( PWR_OK_CLK_CLR, PWR_OK_CLK_BIT );
	setBit( PWR_OK_CLK_SET, PWR_OK_CLK_BIT );
}

unsigned char power_ok_state( void )
{
	return bitIsSet( PWR_OK_PIN, PWR_OK_BIT );
}

void io_set_sysreset_low( void )
{
	setBit( SYSRESET_CLR, SYSRESET_BIT );
}

void io_set_sysreset_high( void )
{
	setBit( SYSRESET_SET, SYSRESET_BIT );
}

void io_set_pwrbtn_low( void )
{
	setBit( PWRBTN_CLR, PWRBTN_BIT );
}

void io_set_pwrbtn_high( void )
{
	setBit( PWRBTN_SET, PWRBTN_BIT );
}

void io_power_off( void )
{
	setBit( BATTX_ON_CLR, BATTX_ON_BIT );
}

void io_power_on( void )
{
	setBit( BATTX_ON_SET, BATTX_ON_BIT );
}

unsigned char io_config_reset_button_pressed( void )
{
	return bitIsClear( CONFIG_RESET_BTN_PIN, CONFIG_RESET_BTN_BIT );
}
