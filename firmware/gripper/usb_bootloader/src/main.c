#include <LPC214X.H>                        /* LPC214x definitions */
#include "type.h"
#include "sbl_iap.h"
#include "sbl_config.h"
#include "util.h"
#include "io.h"

extern void serial_init( void );
extern void serial_sendline( const char* str );
extern int serial_sendChar( int ch );

extern void timer_init( void );

extern void enter_usb_isp(void);

const char* enterBootloaderStr = "Entering bootloader";
const char* startUserAppStr = "Starting user app";

/* Main Program */

int main (void) {
	timer_init();
	serial_init();
	
	setBit( AD04_DIR, AD04_BIT );
	setBit( AD04_CLR, AD04_BIT ); 

	serial_sendline("Startup bootloader");
	if ( bitIsSet( RSIR, POR_RESET ) ) //RSIR & (unsigned long)1 )
	{
		serial_sendline("POR reset!");
	}
	RSIR = 0xff;

	/* If CRP3 is enabled and user flash start sector is not blank, execute the user code */
	if( crp == CRP3 && bitIsSet( AD03_PIN, AD03_BIT ) && user_code_present() )
	{
		serial_sendline(startUserAppStr);
		execute_user_code();
	}

	serial_sendline(enterBootloaderStr);
	enter_usb_isp();
}
