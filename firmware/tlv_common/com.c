#define _COM_NOEXTERN_
#include "com.h"
#include <string.h>
#include <stdio.h>

extern void producer_read_buffer( unsigned char** p, unsigned short* length );
extern void producer_reset( void );
extern void produce_error( void );

extern void parser( unsigned char* data, int length );

extern void wait_micro( unsigned int microsec ); 

enum
{
	COM_STATE_WAITING_FOR_MESSAGE = 0,
	COM_STATE_READING_LENGTH = 1,
	COM_STATE_READING_PAYLOAD = 2,
	COM_STATE_READING_CHECKSUM = 3
};

enum
{
	COM_DATA_STATE_NORMAL = 0,
	COM_DATA_STATE_ESCAPED = 1
};

unsigned int com_rx_counter = 0;

static unsigned char com_state = 0;
static unsigned char com_data_state = 0;

static unsigned char com_length_buffer[2];
static int com_length_num_bytes_to_read = 0;
const int com_length_length = 2;

#define COM_PAYLOAD_MAX_LENGTH 512
unsigned char com_payload_buffer[COM_PAYLOAD_MAX_LENGTH];
int com_payload_num_bytes_to_read = 0;
int com_payload_length = 0;

unsigned char com_checksum_buffer[2];
int com_checksum_num_bytes_to_read = 0;
const int com_checksum_length = 2;

char com_str_buffer[COM_STR_BUFFER_SIZE];

extern void com_num_bytes_available( int* numBytesAvailable );
extern void com_read( unsigned char* ch );
extern void com_write( unsigned char* buffer, unsigned short length );
extern void com_read_update( void );

char* com_get_string_buffer( void )
{
	return com_str_buffer;
}

void com_transit( int state )
{
	com_data_state = COM_DATA_STATE_NORMAL;

#ifdef PRINTF_DEBUG
	switch( com_state )
	{
	case COM_STATE_WAITING_FOR_MESSAGE:
		printf( "COM_STATE_WAITING_FOR_MESSAGE -> " );
	break;

	case COM_STATE_READING_LENGTH:
		printf( "COM_STATE_READING_LENGTH -> " );
		break;

	case COM_STATE_READING_PAYLOAD:
		printf( "COM_STATE_READING_PAYLOAD -> " );
		break;

	case COM_STATE_READING_CHECKSUM:
		printf( "COM_STATE_READING_CHECKSUM -> " );
		break;

	default:
		printf( "?? -> " );
		break;
	}
#endif

	switch( state )
	{
	case COM_STATE_WAITING_FOR_MESSAGE:
#ifdef PRINTF_DEBUG
		printf( "COM_STATE_WAITING_FOR_MESSAGE\n\r" );
#endif
		com_state = COM_STATE_WAITING_FOR_MESSAGE;
	break;

	case COM_STATE_READING_LENGTH:
#ifdef PRINTF_DEBUG
		printf( "COM_STATE_READING_LENGTH\n\r" );
#endif
		com_state = COM_STATE_READING_LENGTH;
		com_length_num_bytes_to_read = 2;
		break;

	case COM_STATE_READING_PAYLOAD:
		com_state = COM_STATE_READING_PAYLOAD;
		com_payload_num_bytes_to_read = com_length_buffer[0];
		com_payload_num_bytes_to_read |= ( com_length_buffer[1] << 8 );
		com_payload_length = com_payload_num_bytes_to_read;

#ifdef PRINTF_DEBUG
		printf( "COM_STATE_READING_PAYLOAD %d\n\r", (int)com_payload_length );
#endif

		if( com_payload_length > COM_PAYLOAD_MAX_LENGTH )
		{
			snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "payload to long: %d > %d\n\r", (int)com_payload_length, (int)COM_PAYLOAD_MAX_LENGTH );
			produce_error();
			com_state = COM_STATE_WAITING_FOR_MESSAGE;
		}

		break;

	case COM_STATE_READING_CHECKSUM:
#ifdef PRINTF_DEBUG
		printf( "COM_STATE_READING_CHECKSUM\n\r" );
#endif
		com_state = COM_STATE_READING_CHECKSUM;
		com_checksum_num_bytes_to_read = 2;
		break;

	default:
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "Invalid transit state=%d", (int)state );
		produce_error();
		com_state = COM_STATE_WAITING_FOR_MESSAGE;
		break;
	}
}

void read_unmangled( unsigned char* buffer, int* num_bytes_to_read )
{
	int  numAvailByte;
	unsigned char ch;

	com_num_bytes_available(&numAvailByte);

	while( *num_bytes_to_read > 0 && numAvailByte > 0 )
	{
		com_read( &ch );
		--numAvailByte;

		switch( com_data_state )
		{
		case COM_DATA_STATE_NORMAL:
			if( (unsigned char)DATA_ESCAPE == ch )
			{
				com_data_state = COM_DATA_STATE_ESCAPED;
			}
			else if( (unsigned char)HEAD == ch )
			{
				com_transit( COM_STATE_READING_LENGTH );
				return;
			}
			else
			{
				//printf( "%02X ", (int)ch );
				*(buffer++) = ch;
				--(*num_bytes_to_read);
			}
			break;

		case COM_DATA_STATE_ESCAPED:
			*(buffer++) = ( ch ^ DATA_MANGLE );
			--(*num_bytes_to_read);
			com_data_state = COM_DATA_STATE_NORMAL;
			break;

		default:
			snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE,  "Invalid com_data_state=%d", (int)com_data_state);
			produce_error();
			com_transit( COM_STATE_WAITING_FOR_MESSAGE );
			return;
		}
	}

	return;
}

/*void com_sendKeepAlive( void )
{
	USB_WriteEP (CDC_DEP_IN, (unsigned char *)&com_keepAliveMessage[0], com_keepAliveMessage[0] );
}*/

unsigned char check_rx_checksum()
{
    unsigned short out = 0;
	unsigned short in = 0;
	unsigned char i;

	for( i=0; i<2; ++i )
	{
		out += com_length_buffer[i];
	}

    for( i=0; i<com_payload_length; ++i )
	{
		out += com_payload_buffer[i];
	}

	out = ( 0xffff & ( (1<<16) - out ) );

	in = com_checksum_buffer[0];
	in |= ( com_checksum_buffer[1] << 8 );

	if( out == in )
	{
		return 1;
	}
	else
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "RX checksum mismatch %x %x", in, out );
		produce_error();
		return 0;
	}
}

void com_print_rx_buffer()
{
	unsigned char i=0;
	unsigned char j=0;

	printf( "<< AA %02X %02X ", com_length_buffer[0], com_length_buffer[1] );

	for( i=0; i<com_payload_length && i<16; ++i )
	{
		printf( "%2X ", (int)com_payload_buffer[i] );
	}

	while( i<com_payload_length )
	{
		printf( "\n\r" );
		for( j=0 ; i<com_payload_length && j<20; ++i, ++j )
		{
			printf( "%02X ", (int)com_payload_buffer[i] );
		}
	}

	i=0;
	while( i<2 )
	{
		for( ; i<2 && j<20; ++i, ++j )
		{
			printf( "%02X ", (int)com_checksum_buffer[i] );
		}
		printf( "\n\r" );
	}
}

void com_print_tx_buffer( unsigned char* buffer, unsigned short bufferSize )
{
	unsigned char i;
	unsigned char col;
	unsigned char escapeFound = 0;

	printf( ">> " );
	col = 1;
	i = 0;
	while( i<bufferSize )
	{
		for( ; i<bufferSize && col<20; ++i )
		{
			if( escapeFound )
			{
				escapeFound = 0;
				printf( "%02X ", (int)( buffer[i] ^ DATA_MANGLE ) );
				++col;
			}
			else
			{
				if( DATA_ESCAPE == buffer[i] )
				{
					escapeFound = 1;
				}
				else
				{
					printf( "%02X ", (int)buffer[i] );
					++col;
				}
			}
		}
		printf( "\n\r" );
		col = 0;
	}
}

void VCOM_PC2MC(void) {
	int  numAvailByte;
	unsigned char ch;

	com_read_update();

	com_num_bytes_available( &numAvailByte );
	if( numAvailByte > 0 )
	{
		//io_set_gpio( 0, 1 );
	}

	while( numAvailByte > 0 )
	{
		switch( com_state )
		{
		case COM_STATE_WAITING_FOR_MESSAGE:
			com_read( &ch );
			if( HEAD == ch )
			{
				com_transit( COM_STATE_READING_LENGTH );
			}
			break;

		case COM_STATE_READING_LENGTH:
			read_unmangled( com_length_buffer + com_length_length - com_length_num_bytes_to_read, &com_length_num_bytes_to_read );
			if( 0 == com_length_num_bytes_to_read )
			{
				com_transit( COM_STATE_READING_PAYLOAD );
			}
			break;

		case COM_STATE_READING_PAYLOAD:
			read_unmangled( com_payload_buffer + com_payload_length - com_payload_num_bytes_to_read, &com_payload_num_bytes_to_read );
			if( 0 == com_payload_num_bytes_to_read )
			{
				com_transit( COM_STATE_READING_CHECKSUM );
			}
			break;

		case COM_STATE_READING_CHECKSUM:
			read_unmangled( com_checksum_buffer + com_checksum_length - com_checksum_num_bytes_to_read, &com_checksum_num_bytes_to_read );
			if( 0 == com_checksum_num_bytes_to_read )
			{
				if( check_rx_checksum() )
				{
					++com_rx_counter;
					parser( com_payload_buffer, com_payload_length );
				}
				else
				{
					com_print_rx_buffer();
				}

				com_transit( COM_STATE_WAITING_FOR_MESSAGE );
			}
			break;

		default:
			snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "Invalid com_state=%d", (int)com_state );
			produce_error();
			com_state = COM_STATE_WAITING_FOR_MESSAGE;
			break;
		}

		com_num_bytes_available( &numAvailByte );
	}
}

//void generate_tx_checksum( unsigned char* payloadBuffer, unsigned short payloadBufferLength, unsigned char* checksumBuffer )
//{
//	unsigned short out = 0;
//	unsigned char i;
//
//	out += payloadBufferLength & 0xFF;
//	out += ( payloadBufferLength >> 8 );
//
//	for( i=0; i<payloadBufferLength; ++i )
//	{
//		out += payloadBuffer[i];
//	}
//
//	out = ( 0xffff & ( (1<<16) - out ) );
//
//	checksumBuffer[0] = 0xFF & out;
//	checksumBuffer[1] = 0xFF & ( out >> 8 );
//}

void com_update( void )
{
	unsigned char* producer_buffer;
	unsigned short producer_buffer_length = 0;

	VCOM_PC2MC();

	producer_read_buffer( &producer_buffer, &producer_buffer_length );

	if( producer_buffer_length > 0 )
	{
#ifdef PRINTF_DEBUG
		com_print_tx_buffer( producer_buffer, producer_buffer_length );
#endif
		while( producer_buffer_length > 0 )
		{
			//printf( "buffer_length %d\n\r", buffer_length );
			if( producer_buffer_length > 64 )
			{
				com_write( producer_buffer, 64 );
				producer_buffer_length -= 64;
				producer_buffer += 64;

				wait_micro( 50 );
			}
			else
			{
				com_write( producer_buffer, producer_buffer_length );
				producer_buffer_length = 0;
			}
		}

		producer_reset();
	}

	//io_set_gpio( 0, 0 );
}
