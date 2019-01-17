#ifndef __CAN_H
#define __CAN_H

extern void can_init( void );

extern void can_send( unsigned char length, unsigned char* data );

#endif  // __CAN_H
