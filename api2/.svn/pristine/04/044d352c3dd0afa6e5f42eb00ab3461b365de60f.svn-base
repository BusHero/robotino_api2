#ifndef _display_H_
#define _display_H_

#include <QtGui>

class LCDDisplay : public QWidget
{
	Q_OBJECT
public:
	LCDDisplay();

public Q_SLOTS:
	void display_buttons_changed( bool, bool, bool, bool );

Q_SIGNALS:
	void display_text_changed( const QString& text, unsigned int row, unsigned int col, bool clear_before, bool clear_after );
	void display_backlight_changed( bool on );
	void display_vbar_changed( float, unsigned int, unsigned int, unsigned int );
	void display_hbar_changed( float, unsigned int, unsigned int, unsigned int );
	void display_progress_changed( unsigned int, unsigned int );
	void display_clear_changed();

private Q_SLOTS:
	void on_text_textChanged();
	void on_text_row_valueChanged( int );
	void on_text_col_valueChanged( int );
	void on_text_clear_before_stateChanged( int );
	void on_text_clear_after_stateChanged( int );
	void on_backlight_stateChanged( int );
	void on_vbar_col_valueChanged( int );
	void on_vbar_row_start_valueChanged( int );
	void on_vbar_row_end_valueChanged( int );
	void on_vbar_value_valueChanged( double );
	void on_hbar_row_valueChanged( int );
	void on_hbar_col_start_valueChanged( int );
	void on_hbar_col_end_valueChanged( int );
	void on_hbar_value_valueChanged( double );
	void on_progress_value_valueChanged( int );
	void on_progress_row_valueChanged( int );

private:
	QTextEdit* _text;
	QSpinBox* _text_col;
	QSpinBox* _text_row;
	QCheckBox* _text_clear_before;
	QCheckBox* _text_clear_after;
	QCheckBox* _backlight;
	QCheckBox* _up;
	QCheckBox* _down;
	QCheckBox* _back;
	QCheckBox* _enter;
	QSpinBox* _vbar_col;
	QSpinBox* _vbar_row_start;
	QSpinBox* _vbar_row_end;
	QDoubleSpinBox* _vbar_value;
	QSpinBox* _hbar_row;
	QSpinBox* _hbar_col_start;
	QSpinBox* _hbar_col_end;
	QDoubleSpinBox* _hbar_value;
	QSpinBox* _progress_value;
	QSpinBox* _progress_row;
};

#endif //_display_H_
