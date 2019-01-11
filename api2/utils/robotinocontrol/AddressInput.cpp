//  Copyright (C) 2004-2009, Robotics Equipment Corporation GmbH

#include "AddressInput.h"

AddressInput::AddressInput( rec::robotino::rpc::Client* client )
: _client( client )
, _lineEdit( new QLineEdit )
, _connectButton( new QPushButton )
, _isConnected( false )
, _control( new QPushButton( this ) )
{
	QHBoxLayout* layout = new QHBoxLayout;
	setLayout( layout );

	QSettings settings;

	_connectButton->setText( "Connect" );
	_lineEdit->setText( settings.value( "ipaddress", "172.26.1.1" ).toString() );

	_control->setText( "Off" );

	layout->addWidget( _lineEdit );
	layout->addWidget( _connectButton );
	layout->addWidget( _control );

	bool connected = true;
	connected &= connect(
		_connectButton,
		SIGNAL( clicked() ),
		SLOT( on_connectButton_clicked() )
		);

	connected &= connect( _control, SIGNAL( clicked() ), SLOT( on_control_clicked() ) );
	Q_ASSERT( connected );
}

void AddressInput::setConnected( bool connected )
{
	_isConnected = connected;

	if( _isConnected )
	{
		_connectButton->setEnabled( true );
		_connectButton->setText( "Disconnect" );
		_lineEdit->setEnabled( false );
	}
	else
	{
		_connectButton->setText( "Connect" );
		_connectButton->setEnabled( true );
		_lineEdit->setEnabled( true );
	}
}

void AddressInput::on_connectButton_clicked()
{
	if( _isConnected )
	{
		Q_EMIT disconnectFromHost();
	}
	else
	{
		Q_EMIT connectToHost( _lineEdit->text() );
	}
}

void AddressInput::on_control_clicked()
{
	if( "Off" == _control->text() )
	{
		bool ret = _client->aquireControl( true );
		if( ret )
		{
			_control->setText( "On" );
		}
	}
	else
	{
		bool ret = _client->aquireControl( false );
		if( ret )
		{
			_control->setText( "Off" );
		}
	}
}
