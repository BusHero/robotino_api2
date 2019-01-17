#ifndef _COM_H_
#define _COM_H_

void com_init( void );

void com_sendKeepAlive( void );

void com_sendError( unsigned char error );

void com_sendStatusReceiveTimeout( unsigned char channel );

void com_sendEnableMotors( unsigned char enable );

void com_sendStorePosition( void );

void com_sendServos( void );

void com_sendStatus( const unsigned char* rxBuffer );

void com_update( void );

void com_sendAllPositions( void );

void com_sendInfo( void );

#ifndef _COM_NOEXTERN_
extern unsigned char com_current_SetAllPositions_message[39];
extern unsigned char com_new_SetAllPositions_message;
#endif


#endif //_COM_H_
