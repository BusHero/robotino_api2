#include "action/NetInput.h"
#include "action/Menu.h"
#include "Translator.h"
#include "Client.h"

extern Client* client;

using namespace action;

NetInput::NetInput( Action* parent )
: Action( parent )
, _ranges( 12, 9 )
, _editPos( 0 )
, _values( 12, 0 )
, _isInitialized( false )
{
	QString iface = attribute( "iface" );
	if( iface.isEmpty() )
	{
		iface = _parent->attribute( "param" );
	}
	if( "ea09" == iface )
	{
		bool connected = true;
		connected &= connect( client, SIGNAL( my_ea09_ip4address_changed( const QHostAddress&, const QHostAddress& ) ), SLOT( on_ea09_ip4address_changed( const QHostAddress&, const QHostAddress& ) ) );
		Q_ASSERT( connected );
	}
}

NetInput::~NetInput()
{
}

void NetInput::setValue( const QString& strValue )
{
	_values.fill( 0 );
	
	QStringList l = strValue.split( "." );
	if( l.size() != 4 )
	{
		log( QString( "%1: setValue invalid value %2" ).arg( title() ).arg( strValue ), 0 );
		return;
	}

	for( int i=0; i<4; ++i )
	{
		QString str = l.at( i );
		if( str.size() > 3 )
		{
			log( QString( "%1: setValue invalid value part %2" ).arg( title() ).arg( str ), 0 );
			return;
		}
		for( int j=0; j<str.size(); ++j )
		{
			bool ok;
			unsigned int num = QString( str.at( j ) ).toUInt( &ok );
			if( false == ok )
			{
				log( QString( "%1: setValue invalid value part %2" ).arg( title() ).arg( str ), 0 );
				return;
			}
			int index = i*3+3-str.size()+j;
			if( (int)num > _ranges[index] )
			{
				log( QString( "%1: setValue part out of range %2" ).arg( title() ).arg( str ), 0 );
				return;
			}

			_values[index] = num;
		}
	}

	log( value(), 0 );
}

QString NetInput::value() const
{
	QString str;
	for( int i=0; i<_values.size(); ++i )
	{
		switch( i%3 )
		{
		case 0:
			if( 0 == _values[i] )
			{
				continue;
			}
			break;
		
		case 1:
			if( 0 == _values[i-1] && 0 == _values[i] )
			{
				continue;
			}
			break;
		}

		str += QString::number( _values[i] );

		if( 2 == i || 5 == i || 8 == i )
		{
			str += ".";
		}
	}
	return str;
}

void NetInput::customActivate( Action* before )
{
	setValue( title() );
	_isInitialized = true;

	displayClear();
	setDisplayText( TT( attribute( "title" ) ), 0, 0, true, true );

	_editPos = 0;
	drawDisplay();
}

void NetInput::customLeave( Action* next )
{
}

QString NetInput::result() const
{
	return title();
}

void NetInput::refreshTitle()
{
	_isInitialized = false;
}

void NetInput::back()
{
	if( 0 == _editPos )
	{
		activate( _parent );
	}
	else
	{
		--_editPos;
		drawDisplay();
	}
}

void NetInput::enter()
{
	if( 11 == _editPos )
	{
		activate( _parent );
	}
	else
	{
		++_editPos;
	}
	drawDisplay();
}

void NetInput::up()
{
	++_values[_editPos];

	if( _values[_editPos] > _ranges[_editPos] )
	{
		_values[_editPos] = 0;
	}

	drawDisplay();
}

void NetInput::down()
{
	--_values[_editPos];

	if( _values[_editPos] < 0 )
	{
		_values[_editPos] = _ranges[_editPos];
	}
	drawDisplay();
}

void NetInput::drawDisplay()
{
	QString str;
	for( int i=0; i<_values.size(); ++i )
	{
		str += QString::number( _values[i] );

		if( 2 == i || 5 == i || 8 == i )
		{
			str += ".";
		}
	}

	setDisplayText( str, 1, 0, true, true );

	int edtIndex = _editPos;
	if( edtIndex < 3 )
	{
		// nothing to do
	}
	else if( edtIndex < 6 )
	{
		++edtIndex;
	}
	else if( edtIndex < 9 )
	{
		edtIndex += 2;
	}
	else
	{
		edtIndex += 3;
	}
	setDisplayText( "^", 2, edtIndex, true, true );
}

void NetInput::on_ea09_ip4address_changed( const QHostAddress&, const QHostAddress& )
{
	_isInitialized = false;

	if( action::Action::currentAction() == _parent )
	{
		Menu* m = qobject_cast<Menu*>( _parent );
		if( m )
		{
			m->drawDisplay();
		}
	}
	else if( this == action::Action::currentAction() )
	{
		setValue( title() );
		drawDisplay();
	}
}
