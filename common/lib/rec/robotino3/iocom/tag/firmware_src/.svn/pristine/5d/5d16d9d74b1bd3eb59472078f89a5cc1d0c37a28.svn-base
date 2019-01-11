#ifndef COMSERIAL

#include <LPC23XX.H>
#include <rtl.H>
#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "cdc.h"
#include "cdcuser.h"

static const int com_numBytesToRead = 1;

void com_init( void )
{
	CDC_Init ();

	USB_Init();                               /* USB Initialization */
	USB_Connect(__TRUE);                      /* USB Connect */
}

void com_num_bytes_available( int* numBytesAvailable )
{
	CDC_OutBufAvailChar( numBytesAvailable );
}

void com_read( unsigned char* ch )
{
	CDC_RdOutBuf ( (char*)ch, &com_numBytesToRead );
}

void com_write( unsigned char* buffer, unsigned short length )
{
	USB_WriteEP (CDC_DEP_IN, buffer, length );
}

void com_read_update( void )
{
}

#endif// COMSERIAL
