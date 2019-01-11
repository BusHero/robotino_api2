#include "AbstractDisplayDriver.h"
#include <string.h>
#include <iostream>
#include <sstream>
#include <cmath>
#include <cassert>
#include <vector>

AbstractDisplayDriver::AbstractDisplayDriver( unsigned int width, unsigned int height )
: _width( width )
, _height( height )
, _curX( -1 )
, _curY( -1 )
{
	_bufDisplay = new unsigned char[ _width * _height ];
	_bufUpdate = new unsigned char[ _width * _height ];
	memset( (void*) _bufDisplay, 255, _width * _height );
	memset( (void*) _bufUpdate, 32, _width * _height );
}

AbstractDisplayDriver::~AbstractDisplayDriver()
{
	delete [] _bufDisplay;
	delete [] _bufUpdate;
}

void AbstractDisplayDriver::setText( const char* text, unsigned int row, unsigned int col, bool clear_before, bool clear_after )
{
	if( row >= _height )
	{
		std::cout << std::endl << "Error setText: row >= _height";
		return;
	}

	if( col >= _width )
	{
		std::cout << std::endl << "Error setText: col >= _width";
		return;
	}

	if( 0 == row )
	{
		memset( (void*) _bufDisplay, 0xFE, _width );
	}

	std::stringstream ss( text );
	std::string item;
	std::vector< std::string > lines;
	while(std::getline(ss, item, '\n'))
	{
		lines.push_back(item);
	}

	for( unsigned int i=0; i<lines.size(); ++i )
	{
		if( row+i >= _height )
		{
			break;
		}

		const std::string& str = lines[i];

		unsigned char* p = _bufUpdate + (i+row) * _width;
		if( clear_before )
		{
			memset( p, 32, col );
		}
		p+= col;
		strncpy( (char*)p, str.c_str(), std::min<int>( _width - col, str.length() ) );

		if( clear_after && col + str.length() < _width )
		{
			memset( p+str.length(), 32, _width - ( col + str.length() ) );
		}
	}

	//printUpdate();
}

void AbstractDisplayDriver::clear()
{
	memset( (void*) _bufDisplay, 255, _width * _height );
	memset( (void*) _bufUpdate, 32, _width * _height );
}

void AbstractDisplayDriver::showVBar( float value, unsigned int col, unsigned int start_row, unsigned int end_row )
{
	if( start_row < end_row )
	{
		std::cout << std::endl << "Error showVBar: start_row <= end_row";
		return;
	}

	if( start_row >= _height )
	{
		std::cout << std::endl << "Error showVBar: start_row >= _height";
		return;
	}

	if( end_row >= _height )
	{
		std::cout << std::endl << "Error showVBar: end_row >= _height";
		return;
	}

	if( col >= _width )
	{
		std::cout << std::endl << "Error showVBar: col >= _width";
		return;
	}

	value = std::max<float>( 0.0f, value );
	value = std::min<float>( 1.0f, value );

	unsigned int length = start_row - end_row + 1;

	value *= length;

	char ch;
	float step = 1.0f / (float) length;
	for( unsigned int i = 0; i < length; ++i )
	{
		if( value >= i+1 )
		{
			ch = 0x2; //full
		}
		else if( value < i )
		{
			ch = '.';
		}
		else
		{
			double intpart;
			double frac = modf( (double)value, &intpart );
			if( frac < 0.2 )
			{
				ch = 0x3;
			}
			else if( frac < 0.5 )
			{
				ch = 0x4;
			}
			else if( frac < 0.8 )
			{
				ch = 0x5;
			}
			else
			{
				ch = 0x6;
			}
		}

		// output char
		unsigned char* pUpdate = _bufUpdate + (start_row-i) * _width + col;
		*pUpdate = ch;
	}

	if( 0 == end_row )
	{
		memset( (void*) _bufDisplay, 0xFE, _width );
	}
}

void AbstractDisplayDriver::showHBar( float value, unsigned int row, unsigned int start_col, unsigned int end_col )
{
	if( start_col > end_col )
	{
		std::cout << std::endl << "Error showHBar: start_col > end_col";
		return;
	}

	if( row >= _height )
	{
		std::cout << std::endl << "Error showHBar: row >= _height";
		return;
	}

	if( start_col >= _width )
	{
		std::cout << std::endl << "Error showHBar: start_col >= _width";
		return;
	}

	if( end_col >= _width )
	{
		std::cout << std::endl << "Error showHBar: end_col >= _width";
		return;
	}

	value = std::max<float>( 0.0f, value );
	value = std::min<float>( 1.0f, value );

	unsigned int length = end_col - start_col + 1;

	value *= length;

	char ch;
	float step = 1.0f / (float) length;
	for( unsigned int i = 0; i < length; ++i )
	{
		if( value >= i+1 )
		{
			ch = 0x2; //full
		}
		else if( value < i )
		{
			ch = '.';
		}
		else
		{
			double intpart;
			double frac = modf( (double)value, &intpart );
			if( frac < 0.5 )
			{
				ch = 0x0;
			}
			else
			{
				ch = 0x1;
			}
		}

		// output char
		unsigned char* pUpdate = _bufUpdate + row * _width + start_col+i;
		*pUpdate = ch;
	}

	if( 0 == row )
	{
		memset( (void*) _bufDisplay, 0xFE, _width );
	}
}

void AbstractDisplayDriver::showProgress( unsigned int step, unsigned int row )
{
	if( row >= _height )
	{
		std::cout << std::endl << "Error showProgress: row >= _height";
		return;
	}

	const unsigned int numArrows = 4;

	step = step % _width;

	char ch;
	for( unsigned int i=0; i<_width; ++i )
	{
		ch = ' ';

		if( i < step )
		{
			if( step + numArrows > _width )
			{
				if( i < step + numArrows - _width )
				{
					ch = '>';
				}
			}
		}
		else if( i < step + numArrows )
		{
			ch = '>';
		}

		unsigned char* pUpdate = _bufUpdate + row * _width + i;
		*pUpdate = ch;
	}

	if( 0 == row )
	{
		memset( (void*) _bufDisplay, 0xFE, _width );
	}
}

void AbstractDisplayDriver::setBacklight( bool status )
{
}

int AbstractDisplayDriver::readButtons()
{
	return 0;
}

void AbstractDisplayDriver::update()
{
	toHome();
	for( unsigned int y = 0; y < _height; ++y )
	{
		for( unsigned int x = 0; x < _width; ++x )
		{
			unsigned int index = y * _width+ x;
			if( _bufDisplay[ index ] != _bufUpdate[ index ] )
			{
				setChar( x, y, _bufUpdate[ index ] );
				_bufDisplay[ index ] = _bufUpdate[ index ];
			} 
		}
	}
}

unsigned char AbstractDisplayDriver::at( unsigned int x, unsigned int y ) const
{
	assert( x < _width );
	assert( y < _height );
	return _bufUpdate[ y * _width + x ];
}

void AbstractDisplayDriver::setChar( unsigned int x, unsigned int y, char ch )
{
}

void AbstractDisplayDriver::toHome()
{
}
