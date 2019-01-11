#include "rec/cv_lt/rgb2bgr.h"
#include <algorithm>

void rec::cv_lt::rgb2bgr( const char* src,
			const unsigned int srcWidth,
			const unsigned int srcHeight,
			const unsigned int srcStep,
			char* dst,
			const unsigned int dstWidth,
			const unsigned int dstHeight,
			const unsigned int dstStep )
{
	unsigned int width = std::min( srcWidth, dstWidth );
	unsigned int height = std::min( srcHeight, dstHeight );

	for( unsigned int line=0; line<height; ++line )
	{
		const unsigned char* psrc = (const unsigned char*)src + srcStep * line;
		unsigned char* pdst = (unsigned char*)dst + dstStep * line;
		for( unsigned int x=0; x<width; ++x )
		{
			unsigned char r = *(psrc++);
			unsigned char g = *(psrc++);
			unsigned char b = *(psrc++);
			*(pdst++) = b;
			*(pdst++) = g;
			*(pdst++) = r;
		}
	}
}

void rec::cv_lt::rgb2bgr( char* srcdst,
			const unsigned int width,
			const unsigned int height,
			const unsigned int step )
{
	for( unsigned int line=0; line<height; ++line )
	{
		unsigned char* p = (unsigned char*)srcdst + step * line;
		for( unsigned int x=0; x<width; ++x )
		{
			unsigned char r = *p;
			*p = *(p+2);
			*(p+2) = r;
			p += 3;
		}
	}
}