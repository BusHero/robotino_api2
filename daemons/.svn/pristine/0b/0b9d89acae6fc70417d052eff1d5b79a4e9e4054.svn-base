#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <QtCore>

class DisplayDriver;

class Display : public QObject
{
	Q_OBJECT
public:
	Display( QObject* parent = NULL );
	~Display();

public Q_SLOTS:
	void set_display_text( const QString& text, unsigned int row, unsigned int col, bool clear_before, bool clear_after );
	void set_display_backlight( bool on );
	void set_display_vbar( float value, unsigned int col, unsigned int start_row, unsigned int end_row );
	void set_display_hbar( float value, unsigned int row, unsigned int start_col, unsigned int end_col );
	void set_display_progress( unsigned int step, unsigned int row );
	void set_display_clear();

Q_SIGNALS:
	void log( const QString& message, int level );
	void display_buttons_changed( bool up, bool down, bool back, bool enter );

private Q_SLOTS:
	void on_timer_timeout();
	void on_backlightTimer_timeout();

private:
	void initDriver();

	DisplayDriver* _driver;
	QString _lines[4];
	int _buttons;
	QTimer* _timer;
	QTimer* _backlightTimer;
};

#endif //_DISPLAY_H_
