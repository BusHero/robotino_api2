#ifndef _RX64_H_
#define _RX64_H_

#define RX64_TXBUFFER_SIZE 16
#define RX64_RXBUFFER_SIZE 16

#define RX64_NOERROR                0
#define RX64_RXOVERFLOW             1
#define RX64_RXMESSAGELENGTHINVALID 2

void rx64_init( void );

void rx64_sendCommand_uint( unsigned char id, unsigned char instruction, const unsigned int* param, unsigned char paramSize );

void rx64_sendCommand( const unsigned char* command );

void rx64_sendActionCommand( void );

#ifndef RX64_NOEXTERN
extern const unsigned char rx64_error;
extern unsigned char rx64_statusReceived;
extern unsigned char rx64_rbuf[ RX64_RXBUFFER_SIZE ];
extern const unsigned char rx64_rMessageLength;
#endif

#endif //_RX64_H_
