#include "rec/cv_lt/rgb2argb32.h"

void rec::cv_lt::rgb2argb32( const char* src,
							const unsigned int width,
							const unsigned int height,
							const unsigned int srcStep,
							char* dst,
							const unsigned int dstStep,
							const unsigned char alpha
							)
{
	const unsigned int alpha_shifted = alpha << 24;
	for( unsigned int line=0; line<height; ++line )
	{
		const unsigned char* psrc = (const unsigned char*)src + srcStep * line;
		unsigned int* pdst = (unsigned int*)dst + ( dstStep / 4 ) * line;
		for( unsigned int x=0; x<width; ++x )
		{
			unsigned char r = *(psrc++);
			unsigned char g = *(psrc++);
			unsigned char b = *(psrc++);
			*(pdst++) = alpha_shifted | r << 16 | g << 8 | b;
		}
	}
}