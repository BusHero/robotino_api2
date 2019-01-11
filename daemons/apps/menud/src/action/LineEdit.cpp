#include "action/LineEdit.h"
#include "Translator.h"
#include "Client.h"

extern Client* client;

using namespace action;

LineEdit::LineEdit( Action* parent )
: Action( parent )
, _editPos( 0 )
{
}

void LineEdit::customActivate( Action* before )
{
	displayClear();
	setDisplayText( TT( attribute( "title" ) ), 0, 0, true, true );

	_newValue = attribute( "value" );

	_editPos = 0;
	drawDisplay();
}

void LineEdit::drawDisplay()
{
	const int width = client->getParameter( "lcdd2/width", 16 ).toInt();

	int start = ( _editPos / width ) * width;

	setDisplayText( _newValue.mid( start ), 1, 0, true, true );

	setDisplayText( "^", 2, _editPos - start, true, true );
}

QChar LineEdit::inc( const QChar ch ) const
{
	const QString chars = attribute( "chars" );
	
	if( chars.isEmpty() )
	{
		return ' ';
	}
	
	if( false == chars.contains( ch ) )
	{
		return chars[0];
	}

	int index = chars.indexOf( ch );
	++index;

	if( index >= chars.length() )
	{
		return ' ';
	}
	else
	{
		return chars[index];
	}
}

QChar LineEdit::dec( const QChar ch ) const
{
	const QString chars = attribute( "chars" );
	
	if( chars.isEmpty() )
	{
		return ' ';
	}
	
	if( false == chars.contains( ch ) )
	{
		return chars[chars.length()-1];
	}

	int index = chars.indexOf( ch );
	--index;

	if( index < 0 )
	{
		return ' ';
	}
	else
	{
		return chars[index];
	}
}

void LineEdit::back()
{
	int pos = _newValue.indexOf( ' ' );
	if( pos > -1 )
	{
		_newValue.truncate( pos );
	}

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

void LineEdit::enter()
{
	int pos = _newValue.indexOf( ' ' );
	if( pos > -1 )
	{
		_newValue.truncate( pos );
	}

	if( _editPos >= _newValue.length() )
	{
		setAttribute( "value", _newValue );
		activate( _parent );
	}

	++_editPos;

	drawDisplay();
}

void LineEdit::up()
{
	if( _editPos < _newValue.length() )
	{
		_newValue[_editPos] = inc( _newValue[_editPos] );
	}
	else
	{
		_newValue.append( inc( 0 ) );
	}

	drawDisplay();
}

void LineEdit::down()
{
	if( _editPos < _newValue.length() )
	{
		_newValue[_editPos] = dec( _newValue[_editPos] );
	}
	else
	{
		_newValue.append( dec( 0 ) );
	}

	drawDisplay();
}
