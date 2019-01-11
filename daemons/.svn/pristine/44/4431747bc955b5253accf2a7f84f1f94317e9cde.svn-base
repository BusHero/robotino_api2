#include "action/Action.h"
#include "action/ControlActivated.h"
#include "rec/robotino/daemons/Log.h"
#include "action/SignalHelper.h"
#include "Client.h"

extern rec::robotino::daemons::Log* logHandler;
extern Client* client;

using namespace action;

Action* Action::_rootAction = NULL;
Action* Action::_currentAction = NULL;
Action* Action::_controlActivatedAction = NULL;

QMap<QString, bool> Action::g_conditions;

Action::Action( Action* parent )
: _parent( parent )
, _signalHelper( NULL )
{
	if( NULL == _rootAction )
	{
		_rootAction = this;
	}

	if( _parent )
	{
		_parent->addChild( this );
	}
}

void Action::customEvent( QEvent* e )
{
	switch( e->type() )
	{
	case ActivateEventId:
		{
			ActivateEvent* ev = static_cast<ActivateEvent*>( e );
			activate_i( ev->before );
		}
		break;
	}
}

void Action::activate_i( Action* before )
{
	client->set_display_backlight( true );

	_currentAction = this;

	Q_ASSERT( NULL == _signalHelper );
	_signalHelper = new action::SignalHelper( this );

	customActivate( before );
}

void Action::leave( Action* next )
{
	if( this != _currentAction )
	{
		return;
	}

	customLeave( next );

	delete _signalHelper;
	_signalHelper = NULL;
}

bool Action::conditionsOk() const
{
	QStringList cond = _attributes.value( "cond" ).split( ",", QString::SkipEmptyParts );
	if( cond.isEmpty() )
	{
		return true;
	}

	Q_FOREACH( const QString& c, cond )
	{
		if( false == g_conditions.value( c, false ) )
		{
			return false;
		}
	}

	return true;
}

void Action::addChild( Action* child )
{
	_children.append( child );
}

void Action::activate( Action* next, bool force )
{
	if( false == force )
	{
		if( next == _currentAction )
		{
			return;
		}
	}

	if( NULL == next )
	{
		QString path;
		if( _currentAction )
		{
			path = _currentAction->fullPath();
		}
		else
		{
			path = "NULL";
		}
		logHandler->log( QString( "Cannot activate NULL action from %1" ).arg( path ), 1 );
		return;
	}
	else
	{
		logHandler->log( QString( "Next action: %1" ).arg( next->title() ), 0 );
	}

	if( _currentAction )
	{
		logHandler->log( QString( "Current action: %1" ).arg( _currentAction->title() ), 0 );
		_currentAction->leave( next );
	}
	else
	{
		logHandler->log( QString( "Current action is NULL" ), 0 );
	}

	next->activate_i( _currentAction );
	//qApp->postEvent( next, new ActivateEvent( _currentAction ) );
}

QStringList Action::parameters() const
{
	QStringList params;
	QMap< QString, QString >::const_iterator iter = _attributes.constBegin();
	while( _attributes.constEnd() != iter )
	{
		if( iter.key().startsWith( "param" ) )
		{
			params << iter.value();
		}
		++iter;
	}
	return params;
}

void Action::clearParameters()
{
	QMap< QString, QString >::iterator iter = _attributes.begin();
	while( _attributes.constEnd() != iter )
	{
		if( iter.key().startsWith( "param" ) )
		{
			iter = _attributes.erase( iter );	
		}
		else
		{
			++iter;
		}
	}
}

void Action::setParameters( const QStringList& params )
{
	int i=0;
	Q_FOREACH( const QString& str, params )
	{
		QString key;
		if( 0 == i )
		{
			key = "param";
		}
		else
		{
			key = QString( "param%1" ).arg( i );
		}

		_attributes[ key ] = str;
		++i;
	}
}

Action* Action::findChildByTitle( const QString& title )
{
	Action* result = NULL;
	Q_FOREACH( Action* a, _children )
	{
		if( title == a->_attributes.value( "title" ) )
		{
			result = a;
			return result;
		}
		else
		{
			result = a->findChildByTitle( title );
			if( result )
			{
				return result;
			}
		}
	}

	return result;
}

void Action::setCondition( const QString& key, bool value )
{
	g_conditions[ key ] = value;
}

bool Action::containsCondition( const QString& key )
{
	return g_conditions.contains( key );
}

Action* Action::find( const QString& title  )
{
	if( _rootAction->title() == title )
	{
		return _rootAction;
	}
	return _rootAction->findChildByTitle( title );
}

void Action::log( const QString& message, int level )
{
	rec::robotino::daemons::Log::singleton()->log( message, level );
	//if( _signalHelper )
	//{
	//	_signalHelper->setLog( message, level );
	//}
}

void Action::setDisplayText( const QString& text, unsigned int row, unsigned int col, bool clear_before, bool clear_after )
{
	if( _signalHelper )
	{
		_signalHelper->setDisplayText( text, row, col, clear_before, clear_after );
	}
}

void Action::setDisplayBacklight( bool on )
{
	if( _signalHelper )
	{
		_signalHelper->setDisplayBacklight( on );
	}
}

void Action::setDisplayVBar( float value, unsigned int col, unsigned int start_row, unsigned int end_row )
{
	if( _signalHelper )
	{
		_signalHelper->setDisplayVBar(value, col, start_row, end_row );
	}
}

void Action::setDisplayHBar( float value, unsigned int row, unsigned int start_col, unsigned int end_col )
{
	if( _signalHelper )
	{
		_signalHelper->setDisplayHBar( value, row, start_col, end_col );
	}
}

void Action::setDisplayProgress( unsigned int step, unsigned int row )
{
	if( _signalHelper )
	{
		_signalHelper->setDisplayProgress( step, row );
	}
}

void Action::displayClear()
{
	if( _signalHelper )
	{
		_signalHelper->displayClear();
	}
}

QString Action::fullPath() const
{
	QString str;
	Action* p = parent();
	while( p != NULL )
	{
		str.prepend( p->title() + "/" );
		p = p->parent();
	}
	str += title();

	return str;
}
