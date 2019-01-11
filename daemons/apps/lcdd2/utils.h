#ifndef _UTILS_H_
#define _UTILS_H_

template< unsigned long N > struct Bin
{
	enum { value = (N % 10) + 
		2 * Bin< N / 10 > :: value } ;
};

template<> struct Bin< 0 >
{
	enum { value = 0 } ;
};

#define BV(x) ((char)Bin<x>::value)

#define CH(x) ((unsigned int) ((unsigned char) x))

#endif //_UTILS_H_
