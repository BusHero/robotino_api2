#ifndef _TLV_PARSER_H_
#define _TLV_PARSER_H_

unsigned char tlv_decode_float( unsigned char* buffer, float* value );
unsigned char tlv_decode_uint16( unsigned char* buffer, unsigned short* value );
unsigned char tlv_decode_int16( unsigned char* buffer, signed short* value );
unsigned char tlv_decode_uint32( unsigned char* buffer, unsigned int* value );
unsigned char tlv_decode_int32( unsigned char* buffer, signed int* value );

#endif //_TLV_PARSER_H_

