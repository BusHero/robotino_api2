#include <LPC23XX.H>                        /* LPC214x definitions */
#include "type.h"
#include "sbl_iap.h"
#include "sbl_config.h"
#include "util.h"

extern void init_serial(void);
extern void timer_init(void);
extern void wait( unsigned int ms );
extern void sendline( const char* str );

extern void enter_usb_isp(void);

extern void power_init( void );
extern unsigned char cover_pwr_btn_is_pressed( void );
extern void set_power_ok_high( void );
extern unsigned char io_config_reset_button_pressed( void );

const char* enterBootloaderStr = "Entering bootloader";
const char* startUserAppStr = "Starting user app";

/* Main Program */

int main (void) {
	int coverbtncounter = 0;

	PINSEL10 = 0;                                      /* Disable ETM interface */
	
	timer_init();
	init_serial();

	sendline("Startup bootloader");
	if ( bitIsSet( RSIR, POR_RESET ) ) //RSIR & (unsigned long)1 )
	{
		sendline("POR reset!");
		// Power-on reset. Initialize flags in battery backup RAM.
		BATTERY_RAM_FLAGS = 0;

		while( coverbtncounter < 50 )
		{
			if( cover_pwr_btn_is_pressed() )
			{
				++coverbtncounter;
			}
			else
			{
				coverbtncounter = 0;
			}

			wait( 10 );

			sendline( "Check button" );
		}
	}
	RSIR = 0xff;

	sendline( "Power On" );
	power_init();

	sendline( "set pwr ok" );
	set_power_ok_high();

	/*if ( bitIsSet2( FIO2PIN, 11, 12 ) )
	{
		sendline(enterBootloaderStr);
		// Reset the flag to get the user code executed on next reset.
		clearBit( BATTERY_RAM_FLAGS, ENTER_USB_BOOTLOADER );
		enter_usb_isp();
	}*/

	if( io_config_reset_button_pressed() )
	{
	  sendline(enterBootloaderStr);
	  // Reset the flag to get the user code executed on next reset.
	  clearBit( BATTERY_RAM_FLAGS, ENTER_USB_BOOTLOADER );
	  enter_usb_isp();
	}
	else
	{
		/* If CRP3 is enabled and user flash start sector is not blank, execute the user code */
		if( crp == CRP3 && user_code_present() )
		{
			sendline(startUserAppStr);
			execute_user_code();
		}

		/* User code not present or isp entry requested */
		if ( !bitIsSet( BATTERY_RAM_FLAGS, ENTER_USB_BOOTLOADER ) && user_code_present() )
		{
			sendline(startUserAppStr);
			setBit( BATTERY_RAM_FLAGS, ENTER_USB_BOOTLOADER );
			execute_user_code();
		}
		else
		{
			sendline(enterBootloaderStr);
			// Reset the flag to get the user code executed on next reset.
			clearBit( BATTERY_RAM_FLAGS, ENTER_USB_BOOTLOADER );
			enter_usb_isp();
		}
	}
}
