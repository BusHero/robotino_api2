#include "LCDDisplay.h"

LCDDisplay::LCDDisplay()
: _text( new QTextEdit( this ) )
, _text_row( new QSpinBox( this ) )
, _text_col( new QSpinBox( this ) )
, _text_clear_before( new QCheckBox( this ) )
, _text_clear_after( new QCheckBox( this ) )
, _backlight( new QCheckBox( this ) )
, _up( new QCheckBox( this ) )
, _down( new QCheckBox( this ) )
, _back( new QCheckBox( this ) )
, _enter( new QCheckBox( this ) )
, _vbar_col( new QSpinBox( this ) )
, _vbar_row_start( new QSpinBox( this ) )
, _vbar_row_end( new QSpinBox( this ) )
, _vbar_value( new QDoubleSpinBox( this ) )
, _hbar_row( new QSpinBox( this ) )
, _hbar_col_start( new QSpinBox( this ) )
, _hbar_col_end( new QSpinBox( this ) )
, _hbar_value( new QDoubleSpinBox( this ) )
, _progress_value( new QSpinBox( this ) )
, _progress_row( new QSpinBox( this ) )
{
	_text_row->setRange( 0, 3 );
	_text_col->setRange( 0, 15 );
	_text_clear_before->setChecked( true );
	_text_clear_after->setChecked( true );

	_up->setText( "Button Up" );
	_up->setEnabled( false );
	_down->setText( "Button Down" );
	_down->setEnabled( false );
	_back->setText( "Button Back" );
	_back->setEnabled( false );
	_enter->setText( "Button Enter" );
	_enter->setEnabled( false );

	_vbar_col->setRange( 0, 15 );
	_vbar_row_start->setRange( 0, 3 );
	_vbar_row_end->setRange( 0, 3 );
	_vbar_value->setRange( 0.0, 1.0 );
	_vbar_value->setSingleStep( 0.1 );

	_hbar_row->setRange( 0, 3 );
	_hbar_col_start->setRange( 0, 15 );
	_hbar_col_end->setRange( 0, 15 );
	_hbar_value->setRange( 0.0, 1.0 );
	_hbar_value->setSingleStep( 0.1 );

	_progress_value->setRange( 0, 100 );
	_progress_row->setRange( 0, 3 );

	QVBoxLayout* layout = new QVBoxLayout;
	setLayout( layout );

	QHBoxLayout* textLayout = new QHBoxLayout;
	layout->addLayout( textLayout );

	textLayout->addWidget( _text );
	QFormLayout* textFormLayout = new QFormLayout;
	textLayout->addLayout( textFormLayout );
	textFormLayout->addRow( "row:", _text_row );
	textFormLayout->addRow( "col:", _text_col );
	textFormLayout->addRow( "clear before:", _text_clear_before );
	textFormLayout->addRow( "clear after:", _text_clear_after );
	textFormLayout->addRow( "backlight:", _backlight );

	QPushButton* clearButton = new QPushButton( this );
	clearButton->setText( "Clear" );
	textFormLayout->addRow( clearButton );

	QGroupBox* buttonGroup = new QGroupBox;
	buttonGroup->setTitle( "Buttons" );
	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonGroup->setLayout( buttonLayout );
	layout->addWidget( buttonGroup );
	buttonLayout->addWidget( _up );
	buttonLayout->addWidget( _down );
	buttonLayout->addWidget( _back );
	buttonLayout->addWidget( _enter );

	QHBoxLayout* hlayout = new QHBoxLayout;
	layout->addLayout( hlayout );

	QGroupBox* vbarGroup = new QGroupBox;
	vbarGroup->setTitle( "VBar" );
	QFormLayout* vbarLayout = new QFormLayout;
	vbarGroup->setLayout( vbarLayout );
	hlayout->addWidget( vbarGroup );
	vbarLayout->addRow( "col:", _vbar_col );
	vbarLayout->addRow( "start row:", _vbar_row_start );
	vbarLayout->addRow( "end row:", _vbar_row_end );
	vbarLayout->addRow( "value:", _vbar_value );

	QGroupBox* hbarGroup = new QGroupBox;
	hbarGroup->setTitle( "HBar" );
	QFormLayout* hbarLayout = new QFormLayout;
	hbarGroup->setLayout( hbarLayout );
	hlayout->addWidget( hbarGroup );
	hbarLayout->addRow( "row:", _hbar_row );
	hbarLayout->addRow( "start col:", _hbar_col_start );
	hbarLayout->addRow( "end col:", _hbar_col_end );
	hbarLayout->addRow( "value:", _hbar_value );

	QGroupBox* progressGroup = new QGroupBox;
	progressGroup->setTitle( "Progress" );
	QFormLayout* progressLayout = new QFormLayout;
	progressGroup->setLayout( progressLayout );
	hlayout->addWidget( progressGroup );
	progressLayout->addRow( "row:", _progress_row );
	progressLayout->addRow( "value:", _progress_value );

	bool connected = true;
	connected &= connect( _text, SIGNAL( textChanged() ), SLOT( on_text_textChanged() ) );
	connected &= connect( _text_row, SIGNAL( valueChanged( int ) ), SLOT( on_text_row_valueChanged( int ) ) );
	connected &= connect( _text_col, SIGNAL( valueChanged( int ) ), SLOT( on_text_col_valueChanged( int ) ) );
	connected &= connect( _text_clear_before, SIGNAL( stateChanged( int ) ), SLOT( on_text_clear_before_stateChanged( int ) ) );
	connected &= connect( _text_clear_after, SIGNAL( stateChanged( int ) ), SLOT( on_text_clear_after_stateChanged( int ) ) );
	connected &= connect( _backlight, SIGNAL( stateChanged( int ) ), SLOT( on_backlight_stateChanged( int ) ) );
	connected &= connect( _vbar_col, SIGNAL( valueChanged( int ) ), SLOT( on_vbar_col_valueChanged( int ) ) );
	connected &= connect( _vbar_row_start, SIGNAL( valueChanged( int ) ), SLOT( on_vbar_row_start_valueChanged( int ) ) );
	connected &= connect( _vbar_row_end, SIGNAL( valueChanged( int ) ), SLOT( on_vbar_row_end_valueChanged( int ) ) );
	connected &= connect( _vbar_value, SIGNAL( valueChanged( double ) ), SLOT( on_vbar_value_valueChanged( double ) ) );
	connected &= connect( _hbar_row, SIGNAL( valueChanged( int ) ), SLOT( on_hbar_row_valueChanged( int ) ) );
	connected &= connect( _hbar_col_start, SIGNAL( valueChanged( int ) ), SLOT( on_hbar_col_start_valueChanged( int ) ) );
	connected &= connect( _hbar_col_end, SIGNAL( valueChanged( int ) ), SLOT( on_hbar_col_end_valueChanged( int ) ) );
	connected &= connect( _hbar_value, SIGNAL( valueChanged( double ) ), SLOT( on_hbar_value_valueChanged( double ) ) );
	connected &= connect( _progress_row, SIGNAL( valueChanged( int ) ), SLOT( on_progress_row_valueChanged( int ) ) );
	connected &= connect( _progress_value, SIGNAL( valueChanged( int ) ), SLOT( on_progress_value_valueChanged( int ) ) );
	connected &= connect( clearButton, SIGNAL( clicked() ), SIGNAL( display_clear_changed() ) );
	Q_ASSERT( connected );
}

void LCDDisplay::display_buttons_changed( bool up, bool down, bool back, bool enter )
{
	_up->setChecked( up );
	_down->setChecked( down );
	_back->setChecked( back );
	_enter->setChecked( enter );
}

void LCDDisplay::on_text_textChanged()
{
	Q_EMIT display_text_changed( _text->toPlainText(), _text_row->value(), _text_col->value(), _text_clear_before->isChecked(),  _text_clear_after->isChecked() );
}

void LCDDisplay::on_text_row_valueChanged( int )
{
	Q_EMIT display_text_changed( _text->toPlainText(), _text_row->value(), _text_col->value(), _text_clear_before->isChecked(),  _text_clear_after->isChecked() );
}

void LCDDisplay::on_text_col_valueChanged( int )
{
	Q_EMIT display_text_changed( _text->toPlainText(), _text_row->value(), _text_col->value(), _text_clear_before->isChecked(),  _text_clear_after->isChecked() );
}

void LCDDisplay::on_text_clear_before_stateChanged( int )
{
	Q_EMIT display_text_changed( _text->toPlainText(), _text_row->value(), _text_col->value(), _text_clear_before->isChecked(),  _text_clear_after->isChecked() );
}

void LCDDisplay::on_text_clear_after_stateChanged( int )
{
	Q_EMIT display_text_changed( _text->toPlainText(), _text_row->value(), _text_col->value(), _text_clear_before->isChecked(),  _text_clear_after->isChecked() );
}

void LCDDisplay::on_backlight_stateChanged( int state )
{
	Q_EMIT display_backlight_changed( Qt::Checked == state );
}

void LCDDisplay::on_vbar_col_valueChanged( int value )
{
	Q_EMIT display_vbar_changed( _vbar_value->value(), _vbar_col->value(), _vbar_row_start->value(), _vbar_row_end->value() );
}

void LCDDisplay::on_vbar_row_start_valueChanged( int value )
{
	Q_EMIT display_vbar_changed( _vbar_value->value(), _vbar_col->value(), _vbar_row_start->value(), _vbar_row_end->value() );
}

void LCDDisplay::on_vbar_row_end_valueChanged( int value )
{
	Q_EMIT display_vbar_changed( _vbar_value->value(), _vbar_col->value(), _vbar_row_start->value(), _vbar_row_end->value() );
}

void LCDDisplay::on_vbar_value_valueChanged( double value )
{
	Q_EMIT display_vbar_changed( _vbar_value->value(), _vbar_col->value(), _vbar_row_start->value(), _vbar_row_end->value() );
}

void LCDDisplay::on_hbar_row_valueChanged( int value )
{
	Q_EMIT display_hbar_changed( _hbar_value->value(), _hbar_row->value(), _hbar_col_start->value(), _hbar_col_end->value() );
}

void LCDDisplay::on_hbar_col_start_valueChanged( int value )
{
	Q_EMIT display_hbar_changed( _hbar_value->value(), _hbar_row->value(), _hbar_col_start->value(), _hbar_col_end->value() );
}

void LCDDisplay::on_hbar_col_end_valueChanged( int value )
{
	Q_EMIT display_hbar_changed( _hbar_value->value(), _hbar_row->value(), _hbar_col_start->value(), _hbar_col_end->value() );
}

void LCDDisplay::on_hbar_value_valueChanged( double value )
{
	Q_EMIT display_hbar_changed( _hbar_value->value(), _hbar_row->value(), _hbar_col_start->value(), _hbar_col_end->value() );
}

void LCDDisplay::on_progress_value_valueChanged( int )
{
	Q_EMIT display_progress_changed( _progress_value->value(), _progress_row->value() );
}

void LCDDisplay::on_progress_row_valueChanged( int )
{
	Q_EMIT display_progress_changed( _progress_value->value(), _progress_row->value() );
}
