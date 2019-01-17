unsigned char tlv_decode_float( unsigned char* buffer, float* value )
{
	unsigned char* p = (unsigned char*)value;
	*(p++) = *(buffer++);
	*(p++) = *(buffer++);
	*(p++) = *(buffer++);
	*(p++) = *(buffer++);
	return 4;
}

unsigned char tlv_decode_uint16( unsigned char* buffer, unsigned short* value )
{
	unsigned char* p = (unsigned char*)value;
	*(p++) = *(buffer++);
	*(p++) = *(buffer++);
	return 2;
}

unsigned char tlv_decode_int16( unsigned char* buffer, signed short* value )
{
	unsigned char* p = (unsigned char*)value;
	*(p++) = *(buffer++);
	*(p++) = *(buffer++);
	return 2;
}

unsigned char tlv_decode_uint32( unsigned char* buffer, unsigned int* value )
{
	unsigned char* p = (unsigned char*)value;
	*(p++) = *(buffer++);
	*(p++) = *(buffer++);
	*(p++) = *(buffer++);
	*(p++) = *(buffer++);
	return 4;
}

unsigned char tlv_decode_int32( unsigned char* buffer, signed int* value )
{
	unsigned char* p = (unsigned char*)value;
	*(p++) = *(buffer++);
	*(p++) = *(buffer++);
	*(p++) = *(buffer++);
	*(p++) = *(buffer++);
	return 4;
}
