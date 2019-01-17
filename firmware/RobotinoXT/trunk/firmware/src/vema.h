#ifndef _VEMA_H_
#define _VEMA_H_

unsigned int vema_startClosedLoop( void );
unsigned int vema_stopClosedLoop( void );

unsigned int vema_getPressures( short* pressures );
unsigned int vema_setPressures( const short* pressures );

#endif // _VEMA_H_
