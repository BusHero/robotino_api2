#ifndef _COM_H_
#define _COM_H_

void com_init( void );

//void com_sendKeepAlive( void );

void com_sendError( unsigned char error );

void com_update( void );

void com_sendInfo( void );

#endif //_COM_H_
