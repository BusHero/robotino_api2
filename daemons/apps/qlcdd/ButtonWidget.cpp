#include "ButtonWidget.h"
#include "rec/robotino/rpc/Client.h"

extern rec::robotino::rpc::Client* client;

ButtonWidget::ButtonWidget()
: _up( new QPushButton( this ) )
, _down( new QPushButton( this ) )
, _back( new QPushButton( this ) )
, _enter( new QPushButton( this ) )
{
	bool connected = true;
	connected &= connect( _up, SIGNAL( pressed() ), SLOT( on_pressed() ) );
	connected &= connect( _down, SIGNAL( pressed() ), SLOT( on_pressed() ) );
	connected &= connect( _back, SIGNAL( pressed() ), SLOT( on_pressed() ) );
	connected &= connect( _enter, SIGNAL( pressed() ), SLOT( on_pressed() ) );
	connected &= connect( _up, SIGNAL( released() ), SLOT( on_released() ) );
	connected &= connect( _down, SIGNAL( released() ), SLOT( on_released() ) );
	connected &= connect( _back, SIGNAL( released() ), SLOT( on_released() ) );
	connected &= connect( _enter, SIGNAL( released() ), SLOT( on_released() ) );
	Q_ASSERT( connected );

	int buttonSize = 36;
	_up->setText( "Up" );
	_up->setFixedSize( buttonSize, buttonSize );
	_down->setText( "Down" );
	_down->setFixedSize( buttonSize, buttonSize );
	_back->setText( "Back" );
	_back->setFixedSize( buttonSize, buttonSize );
	_enter->setText( "Enter" );
	_enter->setFixedSize( buttonSize, buttonSize );

	QGridLayout* layout = new QGridLayout;
	setLayout( layout );
	layout->addWidget( _up, 0, 1 );
	layout->addWidget( _back, 1, 0 );
	layout->addWidget( _enter, 1, 2 );
	layout->addWidget( _down, 2, 1 );
}

void ButtonWidget::on_pressed()
{
	client->set_display_buttons( sender() == _up, sender() == _down, sender() == _back, sender() == _enter );
}

void ButtonWidget::on_released()
{
	client->set_display_buttons( false, false, false, false );
}
