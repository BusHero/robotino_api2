/*
Copyright (c) 2011, REC Robotics Equipment Corporation GmbH, Planegg, Germany
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
- Neither the name of the REC Robotics Equipment Corporation GmbH nor the names of
  its contributors may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "AddressInput.h"
#include "ConnectButton.h"
#include "Client.h"

AddressInput::AddressInput( QWidget* parent )
: QWidget( parent )
, _input( new QComboBox( this ) )
, _connectButton( new ConnectButton( this ) )
{
	setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
	
	QHBoxLayout* layout = new QHBoxLayout;
	layout->setMargin( 0 );
	setLayout( layout );
	layout->addWidget( _input );
	layout->addWidget( _connectButton );

	_input->setEditable( true );

	bool success = true;
	success &= connect(
		_input->lineEdit(),
		SIGNAL( editingFinished() ),
		SLOT( on_editingFinished() )
		);

	success &= connect( Client::singleton(), SIGNAL( is_connected() ), SLOT( on_client_is_connected() ) );
	success &= connect( Client::singleton(), SIGNAL( is_disconnected() ), SLOT( on_client_is_disconnected() ) );

	success &= connect( _connectButton, SIGNAL( clicked() ), SLOT( on_connectButton_clicked() ) );

	Q_ASSERT( success );

	updateHistory();
}

void AddressInput::on_editingFinished()
{
	QString text = _input->lineEdit()->text();
	text.remove( QRegExp("s") );

	Client::singleton()->setAddress( text );
}

void AddressInput::on_client_is_connected()
{
	_input->setEnabled( false );

	QSettings settings;
	QStringList history = settings.value( "address_history" ).toStringList();
	history.removeAll( Client::singleton()->address() );
	history.prepend( Client::singleton()->address() );
	settings.setValue( "address_history", history );

	settings.setValue( "address", Client::singleton()->address() );

	updateHistory();
}

void AddressInput::on_client_is_disconnected()
{
	_input->setEnabled( true );
}

void AddressInput::on_connectButton_clicked()
{
	on_editingFinished();
	_connectButton->on_clicked();
}

void AddressInput::updateHistory()
{
	QSettings settings;
	_input->addItems( settings.value( "address_history" ).toStringList() );
	_input->setEditText( settings.value( "address" ).toString() );
}
