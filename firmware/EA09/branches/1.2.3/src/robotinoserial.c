#include "robotinoserial.h"
#include "motor.h"
#include "adc.h"
#include "io.h"
#include "util.h"
#include "qdsa_protocol.h"
#include "com.h"
#include "fpga.h"
#include "version.h"
#include <string.h>

#include <LPC23XX.H>

#define USE_UART0

#ifdef USE_UART0
#  define UxRBR U0RBR
#  define UxLSR U0LSR
#  define UxIIR U0IIR
#  define UxTHR U0THR
#  define UxLCR U0LCR
#  define UxIER U0IER
#  define UxDLL U0DLL
#  define UxDLM U0DLM
#  define UxFDR U0FDR
#else
#  define UxRBR U1RBR
#  define UxLSR U1LSR
#  define UxIIR U1IIR
#  define UxTHR U1THR
#  define UxLCR U1LCR
#  define UxIER U1IER
#  define UxDLL U1DLL
#  define UxDLM U1DLM
#  define UxFDR U1FDR
#endif

unsigned char rbuf[ 256 ];//NB_P2Q_FULL ];
signed short rindex = 0;
volatile unsigned char rchar;

unsigned char tbuf[ 256 ];
unsigned char tindex;
unsigned char tMessageLength;

extern const unsigned char vel_old2new[256];
extern signed short vel_new2old( signed short new );

extern unsigned char robotino_shutdown;
unsigned char robotino_ignore_pid_parameters = 0;

extern unsigned int globalTime;

void robotinoserial_irq (void) __irq {
	volatile char dummy;
	volatile char IIR;

	/*------------------------------------------------
	Repeat while there is at least one interrupt source.
	------------------------------------------------*/
	while (((IIR = UxIIR) & 0x01) == 0) {
		switch (IIR & 0x0E) {
			 case 0x06: /* Receive Line Status */
				 dummy = UxLSR;  /* Just clear the interrupt source */
				 break;

			 case 0x04: /* Receive Data Available */
			 case 0x0C: /* Character Time-Out */
				 {
					 rchar = UxRBR;

					 if( rindex >= rbuf[1] ) //rbuf[1] stores the message length
					 {
						 //this can happen if we receive a new message before processing the last message
						 rindex = 0;
					 }

					 if( 0 == rindex && rchar < 1 )
					 {
						 //message id must be >0
						 rindex = -1;
					 }
					 else
					 {
						 switch( rindex )
						 {
						 case 0:
							 //the message id
							 break;

						 case 1:
							 if( rchar < 3 ) //the message length must be at least 3
							 {
								 rindex = -1;
							 }
							 else
							 {
								 if( START0 == rbuf[0] )
								 {
									 if( START1 != rchar )
									 {
										 rindex = -1;
									 }
									 else
									 {
										 rchar = NB_P2Q_FULL; //write the correct message length to rbuf[1]
									 }
								 }
							 }
							 break;

						 case 2:
							 if( START0 == rbuf[0] && START2 != rchar )
							 {
								 rindex = -1;
							 }
							 break;

						 case P2M_STOP0_POS:
							 if( START0 == rbuf[0] && STOP0 != rchar )
							 {
								 rindex = -1;
							 }
							 break;

						 case P2M_STOP1_POS:
							 if( START0 == rbuf[0] && STOP1 != rchar )
							 {
								 rindex = -1;
							 }
							 break;

						 case P2M_STOP2_POS:
							 if( START0 == rbuf[0] && STOP2 != rchar )
							 {
								 rindex = -1;
							 }
							 break;

						 default:
							 break;
						 }
					 }

					 if( rindex >= 0 )
					 {
						 rbuf[ rindex ] = rchar;
						 ++rindex;
					 }
					 else
					 {
						 rindex = 0;
					 }
				 }
				 break;

			 case 0x02: /* THRE Interrupt */
				 if( tindex < tMessageLength )
				 {
					 UxTHR = tbuf[ tindex ];
					 ++tindex;
				 }
				 break;

			 case 0x00: /* Modem Interrupt */
				 dummy = U1MSR;  /* Just clear the interrupt source */
				 break;

			 default:
				 break;
		}
	}

	VICVectAddr = 0; /* Acknowledge Interrupt */
}

void robotinoserial_init( void )
{
	volatile char dummy;

	rindex = 0;
	tindex = 0;

	memset( rbuf, 255, 256 );

	UxLCR = 0x03;            /* 8 bits, no Parity, 1 Stop bit */
	UxIER = 0;               /* Disable UART Interrupts */

#ifdef USE_UART0
	setBit( PINSEL0, 4 ); //set P0.2 as TXD0 
	setBit( PINSEL0, 6 ); //set P0.2 as RXD0

	VICVectAddr6 = (unsigned long)robotinoserial_irq;
	VICVectCntl6 = 14;       /* use it for UART0 Interrupt VICVectCntl is VICVectPriority in LPC2378*/
	VICIntEnable = 1 << 6;   /* Enable UART0 Interrupt */
#else
    setBit( PINSEL4, 1 ); //set P2.0 as TXD1
    setBit( PINSEL4, 3 ); //set P2.1 as RXD1

	VICVectAddr7 = (unsigned long)robotinoserial_irq;
	VICVectCntl7 = 14;       /* use it for UART1 Interrupt VICVectCntl is VICVectPriority in LPC2378*/
	VICIntEnable = 1 << 7;   /* Enable UART1 Interrupt */
#endif

	UxLCR |= 0x80;           /* Set DLAB */

  /******************* setup for 115200 baud (with PLL = 12MHz) ****************************/
  UxDLL = 4;
  UxDLM = 0;
  UxFDR = 5; /*DIVADDVAL = 5*/
  UxFDR |= 8 << 4; /*MULVAL = 8*/
  /******************* setup for 115200 baud (with PLL = 12MHz) ****************************/

	/******************* setup for 115200 baud (with PLL = 12.5MHz) ****************************/
	//UxDLL = 4;
	//UxDLM = 0;
	//UxFDR = 9; /*DIVADDVAL = 5*/
	//UxFDR |= 13 << 4; /*MULVAL = 8*/
	/******************* setup for 115200 baud (with PLL = 12.5MHz) ****************************/

	UxLCR &= ~0x80;          /* Clear DLAB */
	/*end setup baudrate*/

	dummy = UxIIR;   /* Read IrqID - Required to Get Interrupts Started */
	UxIER = 3;       /* Enable UART1 RX and THRE Interrupts */
}

void sendMessage( void )
{
	tindex = 1;
	UxTHR = tbuf[0];
}

void decodeMasterMessage( unsigned char* data )
{
	if( bitIsSet( *data, 1 ) )
	{
		robotino_shutdown = 1;
	}

	if( bitIsClear( *data, 3 ) )
	{
		robotino_ignore_pid_parameters = 1;
	}
	else
	{
		robotino_ignore_pid_parameters = 0;
	}
}

void decodeMotor( unsigned char* data, unsigned char motor )
{
	//set velocity
		
	unsigned char oldVel = *( data + S_DV );
	short newVel = vel_old2new[ oldVel ];

	if( bitIsClear( *(data + S_DO), 0 ) )
	{
		motor_speed_setpoint[ motor ] = 0;
	}
	else
	{
		if( bitIsSet( *( data + S_MISCOUT ), DV_DIR ) )
		{
		    motor_speed_setpoint[ motor ] = newVel;
		}
		else
		{
			motor_speed_setpoint[ motor ] = -newVel;
		}
	}

	//motor_speed_setpoint[ 0 ] =    *( data + S_DV );

	if( bitIsSet( *( data + S_MISCOUT ), RESET_POS ) )
	{
		fpga_motor_position[motor] = 0;
	}

	if( robotino_ignore_pid_parameters || ( 255 == *(data+7) && 255 == *(data+8) && 255 == *(data+9) ) )
	{
		motor_kp[motor] = motor_kp_default[motor];
		motor_ki[motor] = motor_ki_default[motor];
		motor_kd[motor] = motor_kd_default[motor];
	}
	else
	{
		motor_kp[motor] = *(data+7);
		motor_ki[motor] = *(data+8);
		motor_kd[motor] = *(data+9);
	}
}

void decodeSlave0Message( unsigned char* data )
{
	decodeMotor( data, 0 );

	io_set_dout( 0, bitIsSet( *(data + S_DO), 1 ) );
	io_set_dout( 1, bitIsSet( *(data + S_DO), 2 ) );
	io_set_dout( 2, bitIsSet( *(data + S_DO), 3 ) );
	io_set_dout( 3, bitIsSet( *(data + S_DO), 4 ) );

	fpga_set_io_rel( 0, bitIsSet( *(data + S_DO), 5 ) );
}

void decodeSlave1Message( unsigned char* data )
{
	decodeMotor( data, 1 );

	io_set_dout( 4, bitIsSet( *(data + S_DO), 1 ) );
	io_set_dout( 5, bitIsSet( *(data + S_DO), 2 ) );
	io_set_dout( 6, bitIsSet( *(data + S_DO), 3 ) );
	io_set_dout( 7, bitIsSet( *(data + S_DO), 4 ) );

	fpga_set_io_rel( 1, bitIsSet( *(data + S_DO), 5 ) );
}

void decodeSlave2Message( unsigned char* data )
{
	decodeMotor( data, 2 );
}

void decodeSlave3Message( unsigned char* data )
{
	decodeMotor( data, 3 );
}

void makeMasterMessage( unsigned char* data )
{
	unsigned char uint8Buffer;
	unsigned short uint16Buffer;

	//current measurment of the whole system (these are the 8 highest bits)
	uint16Buffer = adc_current();
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10

	uint8Buffer = ( uint16Buffer & 0x3 ); //bits 0-1

	*(data++) = 0;

	//battery voltage (these are the 8 highest bits)
	uint16Buffer = adc_battery();
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10

	uint8Buffer |= ( ( uint16Buffer & 0x3 ) << 2 ); //bits 0-1 of battery voltage at bits 2-3 of buffer

	//not used
	*(data++) = ( 0 );

	//not used
	*(data++) = MAJOR;

	//not used
	*(data++) = MINOR;

	//not used
	*(data++) = PATCH;

	//not used
	*(data++) = ( 0 );

	//bit 0-1 low bits of current measurment of the whole system
	//bit 2-3 low bits of battery voltage
	//bit 4-7 not used
	*(data++) = ( uint8Buffer );

	//not used
	*(data++) = ( 0 );

	//not used
	*(data++) = ( 0 );
}

unsigned char* encodeMotor( unsigned char* data, unsigned char motor )
{
	unsigned char velAbs;
	unsigned char tmp;
	signed int int32Buffer;
	unsigned char* p;

	//bit 0 is the direction of rotation measured by the encoder of motor 0
	//bit 1 not used
	//bit 2 not used
	//bit 3 not used
	//bit 4 firmware version bit 3
	//bit 5 firmware version bit 2
	//bit 6 firmware version bit 1
	//bit 7 firmware version bit 0

	if( fpga_motor_velocity[ motor ] >= 0 )
	{
		velAbs = fpga_motor_velocity[ motor ];
		tmp = 1;
	}
	else
	{
		velAbs = -fpga_motor_velocity[ motor ];
		tmp = 0;
	}

	velAbs = vel_new2old( velAbs );

	//set FIRMWAREVERSION1 bit
	tmp |= 1 << 6;
	*(data++) = tmp;

	//absolute value velocity
	*(data++) = velAbs;

	if( MOTOR_IS_POSITIONCONTROLLER_ON( motor ) )
	{
		int32Buffer = POSCONTROL[motor].posreached;
		int32Buffer = -int32Buffer;
	}
	else
	{
		int32Buffer = fpga_motor_position[ motor ];
	}
	p = (unsigned char*) &int32Buffer;
	//position motor 0 bits 0-7
	*(data++) = ( *p++ );
	//position motor 0 bits 8-15
	*(data++) = ( *p++ );
	//position motor 0 bits 16-23
	*(data++) = ( *p++ );
	//position motor 0 bits 24-31
	*(data++) = ( *p++ );

	return data;
}

void makeSlave0Message( unsigned char* data )
{
	unsigned char uint8Buffer;
	unsigned char uint8Buffer2;

	unsigned short uint16Buffer;

	unsigned char* p;

	//current measurement of motor 0
	uint16Buffer = motor_current[ 0 ];
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10
	uint8Buffer = ( uint16Buffer & 0x3 ); //bits 0-1 at bits 0-1 of buffer

	uint16Buffer = adc_distance( 5 );
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10
	uint8Buffer |= ( ( uint16Buffer & 0x3 ) << 2 ); //bits 0-1 of distance5 at bits 2-3 of buffer

	uint16Buffer = adc_distance( 6 );
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10
	uint8Buffer |= ( ( uint16Buffer & 0x3 ) << 4 ); //bits 0-1 of distance6 at bits 4-5 of buffer

	uint16Buffer = io_adc( 0 );
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10
	uint8Buffer |= ( ( uint16Buffer & 0x3 ) << 6 ); //bits 0-1 of ain0 at bits 6-7 of buffer

	uint16Buffer = io_adc( 1 );
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10
	uint8Buffer2 = ( uint16Buffer & 0x3 ); //bits 0-1 at bits 0-1 of buffer2

	uint16Buffer = io_adc( 2 );
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10
	uint8Buffer2 |= ( ( uint16Buffer & 0x3 ) << 2 ); //bits 0-1 of ain2 at bits 2-3 of buffer

	uint16Buffer = io_adc( 3 );
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10
	uint8Buffer2 |= ( ( uint16Buffer & 0x3 ) << 4 ); //bits 0-1 of ain3 at bits 4-5 of buffer

	uint16Buffer = adc_distance( 7 );
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10
	uint8Buffer2 |= ( ( uint16Buffer & 0x3 ) << 6 ); //bits 0-1 of distance7 at bits 6-7 of buffer

	*(data++) = ( uint8Buffer );
	*(data++) = ( uint8Buffer2 );

	data = encodeMotor( data, 0 );

	uint8Buffer = io_din_buf & 0xF;

	if( io_bumper() )
	{
		uint8Buffer |= 1 << 4;
	}
	*(data++) = ( uint8Buffer );

	p = (unsigned char*)&globalTime;

	//not used
	*(data++) = *(p++);

	//not used
	*(data++) = *(p++);

	//not used
	*(data++) = *(p++);

	//not used
	*(data++) = *(p++);
}

void makeSlave1Message( unsigned char* data )
{
    unsigned char uint8Buffer;
	unsigned char uint8Buffer2;

	unsigned short uint16Buffer;

	//current measurement of motor 1
	uint16Buffer = motor_current[ 1 ];
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10
	uint8Buffer = ( uint16Buffer & 0x3 ); //bits 0-1 at bits 0-1 of buffer

	uint16Buffer = adc_distance( 4 );
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10
	uint8Buffer |= ( ( uint16Buffer & 0x3 ) << 2 ); //bits 0-1 of distance5 at bits 2-3 of buffer

	uint16Buffer = adc_distance( 3 );
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10
	uint8Buffer |= ( ( uint16Buffer & 0x3 ) << 4 ); //bits 0-1 of distance6 at bits 4-5 of buffer

	uint16Buffer = io_adc( 4 );
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10
	uint8Buffer |= ( ( uint16Buffer & 0x3 ) << 6 ); //bits 2-3 of ain0 at bits 6-7 of buffer

	uint16Buffer = io_adc( 5 );
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10
	uint8Buffer2 = ( uint16Buffer & 0x3 ); //bits 0-1 at bits 0-1 of buffer

	uint16Buffer = io_adc( 6 );
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10
	uint8Buffer2 |= ( ( uint16Buffer & 0x3 ) << 2 ); //bits 0-1 of ain2 at bits 2-3 of buffer

	uint16Buffer = io_adc( 7 );
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10
	uint8Buffer2 |= ( ( uint16Buffer & 0x3 ) << 4 ); //bits 0-1 of ain3 at bits 4-5 of buffer

	uint16Buffer = adc_distance( 2 );
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10
	uint8Buffer2 |= ( ( uint16Buffer & 0x3 ) << 6 ); //bits 0-1 of distance7 at bits 6-7 of buffer

	*(data++) = ( uint8Buffer );
	*(data++) = ( uint8Buffer2 );

	data = encodeMotor( data, 1 );

	uint8Buffer = io_din_buf >> 4;

	*(data++) = ( uint8Buffer );

	//not used
	*(data++) = ( 0 );

	//not used
	*(data++) = ( 0 );

	//not used
	*(data++) = ( 0 );

	//not used
	*(data++) = ( 0 );
}

void makeSlave2Message( unsigned char* data )
{
    unsigned char uint8Buffer;
	unsigned char uint8Buffer2 = 0;

	unsigned short uint16Buffer;

	//current measurement of motor 0
	uint16Buffer = motor_current[ 2 ];
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10
	uint8Buffer = ( uint16Buffer & 0x3 ); //bits 0-1 at bits 0-1 of buffer

	uint16Buffer = adc_distance( 1 );
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10
	uint8Buffer |= ( ( uint16Buffer & 0x3 ) << 2 ); //bits 0-1 of distance1 at bits 2-3 of buffer

	uint16Buffer = adc_distance( 0 );
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10
	uint8Buffer |= ( ( uint16Buffer & 0x3 ) << 4 ); //bits 0-1 of distance0 at bits 4-5 of buffer

	*(data++) = 0;
	*(data++) = 0;
	*(data++) = 0;
	*(data++) = 0;

	uint16Buffer = adc_distance( 8 );
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10
	uint8Buffer2 |= ( ( uint16Buffer & 0x3 ) << 6 ); //bits 0-1 of distance8 at bits 6-7 of buffer

	*(data++) = ( uint8Buffer );
	*(data++) = ( uint8Buffer2 );

	data = encodeMotor( data, 2 );

	*(data++) = 0;

	//not used
	*(data++) = ( 0 );

	//not used
	*(data++) = ( 0 );

	//not used
	*(data++) = ( 0 );

	//not used
	*(data++) = ( 0 );
}

void makeSlave3Message( unsigned char* data )
{
    unsigned char uint8Buffer;
	unsigned char uint8Buffer2 = 0;

	unsigned short uint16Buffer;

	//current measurement of motor 0
	uint16Buffer = motor_current[ 3 ];
	*(data++) = ( uint16Buffer >> 2 ); //bits 2-10
	uint8Buffer = ( uint16Buffer & 0x3 ); //bits 0-1 at bits 0-1 of buffer


    *(data++) = 0;
	*(data++) = 0;
	*(data++) = 0;
	*(data++) = 0;
	*(data++) = 0;
	*(data++) = 0;
	*(data++) = 0;


	*(data++) = ( uint8Buffer );
	*(data++) = ( uint8Buffer2 );

	data = encodeMotor( data, 3 );

	*(data++) = 0;

	//not used
	*(data++) = ( 0 );

	//not used
	*(data++) = ( 0 );

	//not used
	*(data++) = ( 0 );

	//not used
	*(data++) = ( 0 );
}

void makeMessage( void )
{
	unsigned char* p = tbuf;

	tbuf[ 0 ] = START0;
	tbuf[ 1 ] = START1;
	tbuf[ 2 ] = START2;

	tbuf[ Q2P_STOP0_POS ] = STOP0;
	tbuf[ Q2P_STOP1_POS ] = STOP1;
	tbuf[ Q2P_STOP2_POS ] = STOP2;

	p += NB_START;
	makeMasterMessage( p );

	p += NUM_BYTES_M2P;
	makeSlave0Message( p );

	p += NUM_BYTES_S2P;
	makeSlave1Message( p );

	p += NUM_BYTES_S2P;
	makeSlave2Message( p );

	p += NUM_BYTES_S2P;
	makeSlave3Message( p );
}

unsigned char robotinoserial_process_p2q_message( void )
{
	unsigned char* p = rbuf;
	
	if( COM_IS_SERIALLINE_CONTROL_ENABLED )
	{
		p += NB_START;
		decodeMasterMessage( p );

		p += NUM_BYTES_P2M;
		decodeSlave0Message( p );

		p += NUM_BYTES_P2S;
		decodeSlave1Message( p );

		p += NUM_BYTES_P2S;
		decodeSlave2Message( p );

		p += NUM_BYTES_P2S;
		decodeSlave3Message( p );
	}

	makeMessage();

	if( COM_IS_SERIALLINE_CONTROL_ENABLED )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

unsigned char robotinoserial_process( void )
{
	unsigned char ret = 0;

	if( rbuf[1] == rindex ) //we received a full message
	{
		rindex = 0;

		switch( rbuf[0] )
		{
		case START0:
			ret = robotinoserial_process_p2q_message();
			tMessageLength = NB_Q2P_FULL;
			break;

		default:
			com_process_message( rbuf, tbuf );
			tMessageLength = tbuf[1];
			break;
		}	

		sendMessage();
	}

	return ret;
}
