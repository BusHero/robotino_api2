#ifndef _UTIL_H_
#define _UTIL_H_

#define _BV( bit ) ( (unsigned int)1 << bit )

#define setBit( reg, bit ) ( reg |= _BV( bit ) )
#define setBit2( reg, b0, b1 ) \
( reg |= ( _BV( b0 ) | _BV( b1 ) ) )
#define setBit3( reg, b0, b1, b2 ) \
( reg |= ( _BV( b0 ) | _BV( b1 ) | _BV( b2 ) ) )
#define setBit4( reg, b0, b1, b2, b3 ) \
( reg |= ( _BV( b0 ) | _BV( b1 ) | _BV( b2 ) | _BV( b3 ) ) )
#define setBit5( reg, b0, b1, b2, b3, b4 ) \
( reg |= ( _BV( b0 ) | _BV( b1 ) | _BV( b2 ) | _BV( b3 ) | _BV( b4 ) ) )
#define setBit6( reg, b0, b1, b2, b3, b4, b5 ) \
( reg |= ( _BV( b0 ) | _BV( b1 ) | _BV( b2 ) | _BV( b3 ) | _BV( b4 ) | _BV( b5 ) ) )
#define setBit7( reg, b0, b1, b2, b3, b4, b5, b6 ) \
( reg |= ( _BV( b0 ) | _BV( b1 ) | _BV( b2 ) | _BV( b3 ) | _BV( b4 ) | _BV( b5 ) | _BV( b6 ) ) )
#define setBit8( reg, b0, b1, b2, b3, b4, b5, b6, b7 ) \
( reg |= ( _BV( b0 ) | _BV( b1 ) | _BV( b2 ) | _BV( b3 ) | _BV( b4 ) | _BV( b5 ) | _BV( b6 ) | _BV( b7 ) ) )
#define setBit9( reg, b0, b1, b2, b3, b4, b5, b6, b7, b8 ) \
( reg |= ( _BV( b0 ) | _BV( b1 ) | _BV( b2 ) | _BV( b3 ) | _BV( b4 ) | _BV( b5 ) | _BV( b6 ) | _BV( b7 ) | _BV( b8 ) ) )
#define setBit10( reg, b0, b1, b2, b3, b4, b5, b6, b7, b8, b9 ) \
( reg |= ( _BV( b0 ) | _BV( b1 ) | _BV( b2 ) | _BV( b3 ) | _BV( b4 ) | _BV( b5 ) | _BV( b6 ) | _BV( b7 ) | _BV( b8 ) | _BV( b9 ) ) )
#define setBit11( reg, b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10 ) \
( reg |= ( _BV( b0 ) | _BV( b1 ) | _BV( b2 ) | _BV( b3 ) | _BV( b4 ) | _BV( b5 ) | _BV( b6 ) | _BV( b7 ) | _BV( b8 ) | _BV( b9 ) | _BV( b10 ) ) )

#define clearBit( reg, bit ) ( reg &= ~_BV( bit ) )

#define bitIsSet( reg, b0 ) \
( ( reg & ( (unsigned int)1 << b0 ) ) > 0 ? 1 : 0 )
#define bitIsSet2( reg, b0, b1 ) \
( ( ( (unsigned int)1 << b0 ) | ( (unsigned int)1 << b1 ) ) == ( reg & ( ( (unsigned int)1 << b0 ) | ( (unsigned int)1 << b1 ) ) ) )

#define bitIsClear( reg, b0 ) \
( 0 == bitIsSet( reg, b0 ) )
#define bitIsClear2( reg, b0, b1 ) \
( 0 == bitIsSet2( reg, b0, b1 ) )

#define RAD2DEG( x ) ( 57.29577951f * x )
#define DEG2RAD( x ) ( 0.017453292f * x )

#ifdef PRINTF_DEBUG
extern void dbgOut( const char*, ... );
#else
#define dbgOut( ... )
#endif

#endif //_UTIL_H_
