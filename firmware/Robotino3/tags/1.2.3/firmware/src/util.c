unsigned char encode_float( unsigned char* buffer, const float value )
{
	unsigned char* p = (unsigned char*)&value;
	*(buffer++) = *p++;
	*(buffer++) = *p++;
	*(buffer++) = *p++;
	*(buffer++) = *p++;
	return 4;
}

unsigned char encode_uint16( unsigned char* buffer, unsigned short value )
{
	unsigned char* p = (unsigned char*)&value;
	*(buffer++) = *p++;
	*(buffer++) = *p++;
	return 2;
}

unsigned char encode_int16( unsigned char* buffer, signed short value )
{
	unsigned char* p = (unsigned char*)&value;
	*(buffer++) = *p++;
	*(buffer++) = *p++;
	return 2;
}

unsigned char encode_uint32( unsigned char* buffer, unsigned int value )
{
	unsigned char* p = (unsigned char*)&value;
	*(buffer++) = *p++;
	*(buffer++) = *p++;
	*(buffer++) = *p++;
	*(buffer++) = *p++;
	return 4;
}

unsigned char encode_int32( unsigned char* buffer, signed int value )
{
	unsigned char* p = (unsigned char*)&value;
	*(buffer++) = *p++;
	*(buffer++) = *p++;
	*(buffer++) = *p++;
	*(buffer++) = *p++;
	return 4;
}

unsigned char decode_float( unsigned char* buffer, float* value )
{
	unsigned char* p = (unsigned char*)value;
	*(p++) = *(buffer++);
	*(p++) = *(buffer++);
	*(p++) = *(buffer++);
	*(p++) = *(buffer++);
	return 4;
}

unsigned char decode_uint16( unsigned char* buffer, unsigned short* value )
{
	unsigned char* p = (unsigned char*)value;
	*(p++) = *(buffer++);
	*(p++) = *(buffer++);
	return 2;
}

unsigned char decode_int16( unsigned char* buffer, signed short* value )
{
	unsigned char* p = (unsigned char*)value;
	*(p++) = *(buffer++);
	*(p++) = *(buffer++);
	return 2;
}

unsigned char decode_uint32( unsigned char* buffer, unsigned int* value )
{
	unsigned char* p = (unsigned char*)value;
	*(p++) = *(buffer++);
	*(p++) = *(buffer++);
	*(p++) = *(buffer++);
	*(p++) = *(buffer++);
	return 4;
}

unsigned char decode_int32( unsigned char* buffer, signed int* value )
{
	unsigned char* p = (unsigned char*)value;
	*(p++) = *(buffer++);
	*(p++) = *(buffer++);
	*(p++) = *(buffer++);
	*(p++) = *(buffer++);
	return 4;
}
