#ifndef _DISPLAYDRIVER_H_
#define _DISPLAYDRIVER_H_

#include "AbstractDisplayDriver.h"
#include <map>

class DisplayDriver : public AbstractDisplayDriver
{
public:
	DisplayDriver();
	~DisplayDriver();

	void setBacklight( bool status );

	int readButtons();

private:
	void setChar( unsigned int x, unsigned int y, char ch );
	void setCustomChar();
	void initialize();
	void toHome();
	void toXY( unsigned int x, unsigned int y );
	void dataOut( bool configData, unsigned char data, unsigned int execTimeMus = 42  );
	unsigned char mapChar( unsigned char ch );
	void printDisplay();
	void printUpdate();

	ParPort* _parPort;
	unsigned char _chMap[ 255 ];
	std::map< int, int > _bitToButton;
};

#endif //_DISPLAYDRIVER_H_
