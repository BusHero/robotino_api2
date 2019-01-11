#ifndef _ABSTRACTDISPLAYDRIVER_H_
#define _ABSTRACTDISPLAYDRIVER_H_

class ParPort;

class AbstractDisplayDriver
{
public:
	AbstractDisplayDriver( unsigned int width, unsigned int height );
	virtual ~AbstractDisplayDriver();

	void setText( const char* text, unsigned int row, unsigned int col, bool clear_before, bool clear_after );

	void showVBar( float value, unsigned int col, unsigned int start_row, unsigned int end_row );

	void showHBar( float value, unsigned int row, unsigned int start_col, unsigned int end_col );

	void showProgress( unsigned int step, unsigned int row );

	void clear();

	virtual void setBacklight( bool status );

	virtual int readButtons();

	void update();

	unsigned int width() const { return _width; }
	unsigned int height() const { return _height; }

	unsigned char at( unsigned int x, unsigned int y ) const;

protected:
	virtual void setChar( unsigned int x, unsigned int y, char ch );
	virtual void toHome();

	unsigned int _width;
	unsigned int _height;
	int _curX;
	int _curY;

	/** what is currently shown at the display. size is height * width */
	unsigned char* _bufDisplay;
	/** this gets shown if update is called */
	unsigned char* _bufUpdate;
};

#endif //_ABSTRACTDISPLAYDRIVER_H_
