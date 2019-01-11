#include "action/ComboBox.h"
#include "Translator.h"
#include "Client.h"

#define SCROLL_UP   QChar( 0x5e )
#define SCROLL_DOWN QChar( 'v' )

extern Client* client;

using namespace action;

ComboBox::ComboBox( Action* parent )
: Action( parent )
, _firstItem( 0 )
, _selectedItem( 0 )
{
}

void ComboBox::customActivate( Action* before )
{
	displayClear();
	setDisplayText( TT( attribute( "title" ) ), 0, 0, true, true );

	if( _items.isEmpty() )
	{
		for( int i=0; ; ++i )
		{
			QString str = attribute( QString( "item%1" ).arg( i ) );
			if( str.isEmpty() )
			{
				break;
			}
			_items << str;
		}
	}

	_selectedItem = 0;
	int i=0;
	Q_FOREACH( const QString& str, _items )
	{
		if( str == attribute( "value" ) )
		{
			_selectedItem = i;
			break;
		}
		++i;
	}

	drawDisplay();
}

void ComboBox::drawDisplay()
{
	if( _items.isEmpty() )
	{
		return;
	}

	const int numLines = client->getParameter( "lcdd2/height", 4 ).toInt();
	const int width = client->getParameter( "lcdd2/width", 16 ).toInt();

	QStringList textLines;

	int numStaticLines = 1;
	textLines << TT( title() );

	// calculate _firstItem
	if( _selectedItem < _firstItem )
	{
		_firstItem = _selectedItem;
	}
	else if( (int)(_selectedItem - _firstItem) >= (numLines- numStaticLines) )
	{
		_firstItem = _selectedItem - (numLines - numStaticLines - 1);
	}

	for( unsigned int i = 0; i < numLines - numStaticLines; ++i )
	{
		//int line =  i + 1 + _line_offset;
		if( (int)(_firstItem + i) < _items.size() )
		{
			QString str( " " );
			str += TT( _items.at(_firstItem + i) );
			textLines << str;
			//setDisplayText( TT( _visibleChildren.at( _firstItem + i )->title() ),line, 1, true, true );
		}
		else
		{
			textLines << "";
			//setDisplayText( "", line, 0, true, true );
		}
	}

	// show cursor
	{
		QString& line = textLines[_selectedItem - _firstItem + numStaticLines];
		line.replace( 0, 1, ">" );
	}
	//setDisplayText( ">", _selectedItem - _firstItem + _line_offset + 1, 0, true, false );

	if( _firstItem > 0 )
	{
		QString& line = textLines[numStaticLines];
		if( line.length() < width-1 )
		{
			line.append( QString( " " ).repeated( width-1-line.length() ) );
		}
		line.replace( width-1, 1, SCROLL_UP );
		//setDisplayText( SCROLL_UP, 1, width-1, false, false );
	}
	if( (int) (_firstItem + numLines - numStaticLines) < _items.size() )
	{
		QString& line = textLines[numLines-1];
		if( line.length() < width-1 )
		{
			line.append( QString( " " ).repeated( width-1-line.length() ) );
		}
		line.replace( width-1, 1, SCROLL_DOWN );
		//setDisplayText( SCROLL_DOWN, numLines-1, width-1, false, false );
	}

	QString text = textLines.join( "\n" );
	//qDebug() << text;
	setDisplayText( text, 0, 0, true, true );
}

void ComboBox::back()
{
	activate( _parent );
}

void ComboBox::enter()
{
	if( _selectedItem < _items.size() )
	{
		setAttribute( "value", _items[_selectedItem] );
	}
	activate( _parent );
}

void ComboBox::up()
{
	if( _items.isEmpty() )
	{
		activate( _parent );
		return;
	}

	if( _selectedItem > 0 )
	{
		--_selectedItem;
		drawDisplay();
	}
}

void ComboBox::down()
{
	if( _items.isEmpty() )
	{
		activate( _parent );
		return;
	}

	if( (int)_selectedItem < ( _items.size() - 1 ) )
	{
		++_selectedItem;
		drawDisplay();
	}
}