#ifndef _ACTION_SIGNALHELPER_H_
#define _ACTION_SIGNALHELPER_H_

#include <QtCore>

namespace action
{
	class Action;

	class SignalHelper : public QObject
	{
		Q_OBJECT
	public:
		SignalHelper( Action* action );
		~SignalHelper();

		void setLog( const QString&, int );
		void setDisplayText( const QString& text, unsigned int row, unsigned int col, bool clear_before, bool clear_after );
		void setDisplayBacklight( bool on );
		void setDisplayVBar( float value, unsigned int col, unsigned int start_row, unsigned int end_row );
		void setDisplayHBar( float value, unsigned int row, unsigned int start_col, unsigned int end_col );
		void setDisplayProgress( unsigned int step, unsigned int row );
		void displayClear();

	Q_SIGNALS:
		void log( const QString&, int );
		void display_text_changed( const QString& text, unsigned int row, unsigned int col, bool clear_before, bool clear_after );
		void display_backlight_changed( bool on );
		void display_vbar_changed( float value, unsigned int col, unsigned int start_row, unsigned int end_row );
		void display_hbar_changed( float value, unsigned int row, unsigned int start_col, unsigned int end_col );
		void display_progress_changed( unsigned int step, unsigned int row );
		void display_clear_changed();

	private Q_SLOTS:
		void on_display_buttons_changed( bool up, bool down, bool back, bool enter );

	private:
		Action* _action;
	};
}

#endif //_ACTION_SIGNALHELPER_H_
