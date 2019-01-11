#ifndef _DISPLAYDRIVER_H_
#define _DISPLAYDRIVER_H_

#include <QtCore>
#include "../lcdd2/AbstractDisplayDriver.h"

class DisplayDriver : public QObject
{
	Q_OBJECT
public:
	DisplayDriver( QObject* parent );

	int width() const { return _displayDriver.width(); }
	int height() const { return _displayDriver.height(); }

	unsigned char at( int x, int y ) const { return _displayDriver.at( x, y ); }

Q_SIGNALS:
	void changed();
	void backlightChanged( bool );

private Q_SLOTS:
	void set_display_text( const QString&, unsigned int, unsigned int, bool, bool );
	void set_display_backlight( bool );
	void set_display_vbar( float, unsigned int, unsigned int, unsigned int );
	void set_display_hbar( float, unsigned int, unsigned int, unsigned int );
	void set_display_progress( unsigned int, unsigned int );
	void set_display_clear();

private:
	AbstractDisplayDriver _displayDriver;
	bool _backlight;
};

#endif //_DISPLAYDRIVER_H_
