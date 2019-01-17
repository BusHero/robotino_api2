/****************************************************************************
** Copyright (C) 2005 REC GmbH.  All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Contact info@RoboticsEC.com for further information
**********************************************************************/

#ifndef _EBDMACROS_H_
#define _EBDMACROS_H_

#define setBit( byte, bit, value ) {if( value > 0 ){  byte |= _BV( bit ); } else { byte &= ~_BV( bit ); }}
#define do_on(DO) {DO##_PORT |= _BV( DO##_BIT );} 
#define do_off(DO) {DO##_PORT &= ~_BV( DO##_BIT );} 
#define do_toggle(DO) {if( bit_is_clear( DO##_PIN, DO##_BIT ) ) {DO##_PORT |= _BV( DO##_BIT );} else {DO##_PORT &= ~_BV( DO##_BIT );} }
#define do_activate(DO) {DO##_DDR |= _BV( DO##_BIT ); }
#define encode_di(byte,bit) { setBit( g_tx_buffer[ byte ], bit, bit_is_set(bit##_PIN,bit##_BIT) ) }
#define dir2int( res, value, dir ) { res = value; if( dir == 0 ) { res = -res; } }
#define limit( value, max, min ) { if( value > max ) { value = max; } else if( value < min ) { value = min; } }

#endif
