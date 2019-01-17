#ifndef _RX28_H_
#define _RX28_H_

#define RX28_TXBUFFER_SIZE 16
#define RX28_RXBUFFER_SIZE 16

#define RX28_NOERROR                0
#define RX28_RXOVERFLOW             1
#define RX28_RXMESSAGELENGTHINVALID 2

void rx28_init( void );

void rx28_sendCommand_uint( unsigned char id, unsigned char instruction, const unsigned int* param, unsigned char paramSize );

void rx28_sendCommand( const unsigned char* command );

void rx64_sendActionCommand( void );

#ifndef RX28_NOEXTERN
extern const unsigned char rx28_error;
extern unsigned char rx28_statusReceived;
extern unsigned char rx28_rbuf[ RX28_RXBUFFER_SIZE ];
extern const unsigned char rx28_rMessageLength;
#endif

#endif //_RX28_H_
