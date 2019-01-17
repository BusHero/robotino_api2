#ifndef _RX10_H_
#define _RX10_H_

#define RX10_TXBUFFER_SIZE 16
#define RX10_RXBUFFER_SIZE 16

#define RX10_NOERROR                0
#define RX10_RXOVERFLOW             1
#define RX10_RXMESSAGELENGTHINVALID 2

void rx10_init( void );

void rx10_sendCommand_uint( unsigned char id, unsigned char instruction, const unsigned int* param, unsigned char paramSize );

void rx10_sendCommand( const unsigned char* command );

void rx64_sendActionCommand( void );

#ifndef RX10_NOEXTERN
extern const unsigned char rx10_error;
extern unsigned char rx10_statusReceived;
extern unsigned char rx10_rbuf[ RX10_RXBUFFER_SIZE ];
extern const unsigned char rx10_rMessageLength;
#endif

#endif //_RX10_H_
