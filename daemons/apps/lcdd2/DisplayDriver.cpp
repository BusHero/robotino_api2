#include "DisplayDriver.h"
#include "ParPort.h"
#include "utils.h"
#include "hw_settings.h"
#include "ButtonId.h"
#include <iostream>

char custChar[][7] = {
	{ BV(10000), BV(10000), BV(10000), BV(10000), BV(10000), BV(10000), BV(10000) },
	{ BV(11100), BV(11100), BV(11100), BV(11100), BV(11100), BV(11100), BV(11100) },
	{ BV(11111), BV(11111), BV(11111), BV(11111), BV(11111), BV(11111), BV(11111) },
	{ BV(00000), BV(00000), BV(00000), BV(00000), BV(00000), BV(00000), BV(11111) },
	{ BV(00000), BV(00000), BV(00000), BV(00000), BV(00000), BV(11111), BV(11111) },
	{ BV(00000), BV(00000), BV(00000), BV(11111), BV(11111), BV(11111), BV(11111) },
	{ BV(00000), BV(11111), BV(11111), BV(11111), BV(11111), BV(11111), BV(11111) }
};

DisplayDriver::DisplayDriver()
: AbstractDisplayDriver( 16, 4 )
, _parPort( new ParPort( 0x378 ) )
{
	initialize();
}

DisplayDriver::~DisplayDriver()
{
	delete _parPort;
}

void DisplayDriver::setChar( unsigned int x, unsigned int y, char ch )
{
	if( (int) x != _curX || (int) y != _curY )
	{
		toXY( x, y );
	}
	dataOut( false, mapChar( ch ), TIME_WRITE_DATA );
	_curX = x + 1;
	_curY = y + 1;
}

void DisplayDriver::setBacklight( bool status )
{
	_parPort->setControl( BIT_BACKLIGHT, status );
}

void DisplayDriver::setCustomChar()
{
	unsigned int i,j;
	for( i = 0; i < sizeof( custChar ) / sizeof( custChar[0] ); ++i )
	{
		dataOut( true, SET_CG_RAM | (i * 8), TIME_SET_CG_RAM );
		for( j = 0; j < sizeof( custChar[0] ); ++j )
		{
			dataOut( false, custChar[ i ][ j ], TIME_WRITE_DATA );
		}
		dataOut( false, 0, TIME_WRITE_DATA );
	}
}

void DisplayDriver::initialize()
{
	dataOut( true, SET_FUNCTION | SFN_INTERFACE_8_BIT, 4100 ); 
	dataOut( true, SET_FUNCTION | SFN_INTERFACE_8_BIT, 100 );
	dataOut( true, SET_FUNCTION | SFN_INTERFACE_8_BIT | SFN_2_LINES | SFN_5x7_DOTS, TIME_SET_FUNCTION );
	dataOut( true, SET_FUNCTION | SFN_INTERFACE_8_BIT | SFN_2_LINES | SFN_5x7_DOTS, TIME_SET_FUNCTION );
	// turn on display
	dataOut( true, DISPLAY_ONOFF | DOO_ON | DOO_CURSOR_OFF | DOO_BLINKING_OFF, TIME_DISPLAY_ONOFF );
	dataOut( true, DISPLAY_CLEAR, TIME_DISPLAY_CLEAR );
	dataOut( true, ENTRY_MODE_SET | EMS_CURSOR_MOVE_INC | EMS_NO_DISPLAY_SHIFT, TIME_ENTRY_MODE_SET );
	dataOut( true, RETURN_HOME, TIME_RETURN_HOME );

	for( unsigned int i = 0; i <= 255; ++i )
	{
		_chMap[ i ] = (unsigned char) i;
	}
	// Special char mapping
	// ä 11100001 0xE1
	// ö 11101111 0xEF
	// ü 11110101 0xF5
	// ß 11100010 0xE2
	_chMap[ CH('ä') ] = 0xE1;
	_chMap[ CH('Ä') ] = 0xE1;
	_chMap[ CH('ö') ] = 0xEF;
	_chMap[ CH('Ö') ] = 0xEF;
	_chMap[ CH('ü') ] = 0xF5;
	_chMap[ CH('Ü') ] = 0xF5;
	_chMap[ CH('ß') ] = 0xE2;

	_bitToButton[ (int) ParPort::SELIN ] = (int) BTN_ENTER;
	_bitToButton[ (int) ParPort::ACK ] = (int) BTN_BACK;
	_bitToButton[ (int) ParPort::PE ] = (int) BTN_DOWN;
	_bitToButton[ (int) ParPort::BUSY ] = (int) BTN_UP;

	setCustomChar();
}

void DisplayDriver::toHome()
{
	_curX = 0;
	_curY = 0;
	dataOut( true, RETURN_HOME, TIME_RETURN_HOME );
}

void DisplayDriver::toXY( unsigned int x, unsigned int y )
{
	unsigned char base = 0;
	switch( y )
	{
	case 1:
		base = 0x40;
		break;
	case 2:
		base = 0x10;
		break;
	case 3:
		base = 0x50;
		break;
	default:
		return;
	}
	dataOut( true, SET_DD_RAM | ((unsigned int)base + x), TIME_SET_DD_RAM );
}

unsigned char DisplayDriver::mapChar( unsigned char ch )
{
	return _chMap[ ch ];
}

void DisplayDriver::dataOut( bool configData, unsigned char data, unsigned int execTimeMus )
{
	_parPort->setControl( BIT_RS, !configData );
	_parPort->setData( data );
	// possible delay
	//  _parPort->waitMus( 1 );
	_parPort->setControl( BIT_EN, true );
	// possible delay
	//  _parPort->waitMus( 1 );
	_parPort->setControl( BIT_EN, false );
	_parPort->waitMus( execTimeMus );
}

int DisplayDriver::readButtons()
{
	unsigned char status = _parPort->readStatus();
	int btnStatus = 0;
	
	std::map< int, int >::const_iterator ci = _bitToButton.begin();
	while( ci != _bitToButton.end() )
	{
		if( (status & ci->first) == 0 )
		{
			btnStatus = btnStatus | ci->second;
		}
		ci++;
	}

	return btnStatus;
}

void DisplayDriver::printDisplay()
{
	std::cout << std::endl;
	for( int y = 0; y < _height; ++y )
	{
		for( int x = 0; x < _width; ++x )
		{
			unsigned int index = y * _width + x;
			std::cout << _bufDisplay[ index ];
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void DisplayDriver::printUpdate()
{
	std::cout << std::endl;
	for( int y = 0; y < _height; ++y )
	{
		for( int x = 0; x < _width; ++x )
		{
			unsigned int index = y * _width + x;
			std::cout << _bufUpdate[ index ];
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
