#include "action/Menu.h"
#include "action/Cancel.h"
#include "Client.h"
#include "Translator.h"

extern Client* client;

#define SCROLL_UP   QChar( 0x5e )
#define SCROLL_DOWN QChar( 'v' )

using namespace action;

Menu::Menu( Action* parent )
: Action( parent )
, _firstItem( 0 )
, _selectedItem( 0 )
{
}

Menu::~Menu()
{
}

void Menu::customActivate( Action* before )
{
	Cancel* cancel = qobject_cast<Cancel*>( before );
	if( cancel )
	{
		activate( _parent );
		return;
	}

	Q_FOREACH( Action* a, _children )
	{
		if( "pre-activate-from-parent" == a->attribute( "func" ) && before == _parent )
		{
			a->execute( this );
		}
	}

	_visibleChildren.clear();
	Q_FOREACH( Action* a, _children )
	{
		if( a->conditionsOk() && "true" == a->attribute( "visible", "true" ) )
		{
			_visibleChildren << a;
		}
	}

	if( _visibleChildren.isEmpty() )
	{
		activate( _parent );
	}

	if( false == _visibleChildren.contains( before ) )
	{
		_selectedItem = 0;
		Q_FOREACH( Action* a, _visibleChildren )
		{
			a->refreshTitle();
		}
	}

	drawDisplay();
}

void Menu::customLeave( Action* next )
{
	if( "apply" == next->attribute( "func" ) )
	{
		QStringList params;
		params << parameters();
		Q_FOREACH( Action* action, _visibleChildren )
		{
			QString res = action->result();
			if( false == res.isEmpty() )
			{
				params << res;
			}
		}

		next->setParameters( params );
	}
}

void Menu::back()
{
	activate( _parent );
}

void Menu::enter()
{
	activate( _visibleChildren.at( _selectedItem ) );
}

void Menu::up()
{
	if( _visibleChildren.isEmpty() )
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

void Menu::down()
{
	if( _visibleChildren.isEmpty() )
	{
		activate( _parent );
		return;
	}

	if( (int)_selectedItem < ( _visibleChildren.size() - 1 ) )
	{
		++_selectedItem;
		drawDisplay();
	}
}

void Menu::drawDisplay()
{
	const int numLines = client->getParameter( "lcdd2/height", 4 ).toInt();
	const int width = client->getParameter( "lcdd2/width", 16 ).toInt();

	QStringList textLines;

	int numStaticLines = 1;
	textLines << TT( title() );

	if( false == _customLine.isEmpty() )
	{
		textLines << _customLine;
		++numStaticLines;
	}

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
		if( (int)(_firstItem + i) < _visibleChildren.size() )
		{
			QString str( " " );
			str += TT( _visibleChildren.at(_firstItem + i)->title() );
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
	if( (int) (_firstItem + numLines - numStaticLines) < _visibleChildren.size() )
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
