#include "action/Factory.h"
#include "action/StatusScreen.h"
#include "action/Menu.h"
#include "action/SelectLanguage.h"
#include "action/FileMenu.h"
#include "action/Execute.h"
#include "action/ControlActivated.h"
#include "action/IP4Address.h"
#include "action/IP4NetMask.h"
#include "action/Cancel.h"
#include "action/Apply.h"
#include "action/EA09FirmwareUpdate.h"
#include "action/EmergencyBumper.h"
#include "action/SetEA09IP4Address.h"
#include "action/RequestShutdown.h"
#include "action/ReadWPA.h"
#include "action/WriteWPA.h"
#include "action/ComboBox.h"
#include "action/LineEdit.h"

#include "rec/robotino/daemons/Log.h"

#include "Client.h"

extern Client* client;

using namespace action;

Factory::Factory()
{
}

void Factory::load()
{
	QDomDocument doc("structure");

	QString fileName = client->getParameter( "~/structure", "/etc/robotino/menud/structure.xml" ).toString();
	QFile structureFile( fileName );
	if( structureFile.open( QIODevice::ReadOnly ) )
	{
		if( false == doc.setContent( &structureFile ) )
		{
			Q_EMIT log( QString( "Not a valid XML file: %1" ).arg( fileName ), 0 );
			return;
		}
	}
	else
	{
		Q_EMIT log( QString( "Error loading %1" ).arg( fileName ), 0 );
		return;
	}
	structureFile.close();

	QDomElement docElem = doc.documentElement();

	QDomNode n = docElem.firstChild();
	while( !n.isNull() )
	{
		QDomElement e = n.toElement(); // try to convert the node to an element.
		if( !e.isNull() )
		{
			loadAction( e, NULL );
		}
		n = n.nextSibling();
	}
}

action::Action* Factory::loadAction( QDomElement& e, action::Action* parent )
{
	action::Action* currentAction = NULL;

	if( "statusscreen" == e.tagName() )
	{
		currentAction = new action::StatusScreen( parent );
	}
	else if( "menu" == e.tagName() )
	{
		currentAction = new action::Menu( parent );
	}
	else if( "selectlanguage" == e.tagName() )
	{
		currentAction = new SelectLanguage( parent );
	}
	else if( "filemenu" == e.tagName() )
	{
		if( e.hasAttribute( "config" ) )
		{
			QString config = e.attribute( "config" );
			if( false == client->containsParameter( "~/" + config + "/title" ) )
			{
				Q_EMIT log( QString( "%1: menu.conf does not contain parameter %2" ).arg( e.attribute( "title" ) ).arg( config + "/title" ), 0 );
				return NULL;
			}
			if( false == client->containsParameter( "~/" + config + "/root" ) )
			{
				Q_EMIT log( QString( "%1: menu.conf does not contain parameter %2" ).arg( e.attribute( "title" ) ).arg( config + "/root" ), 0 );
				return NULL;
			}
			if( false == client->containsParameter( "~/" + config + "/command" ) && false == client->containsParameter( "~/" + config + "/script" ) )
			{
				Q_EMIT log( QString( "%1: menu.conf does not contain parameter %2 or %3" ).arg( e.attribute( "title" ) ).arg( config + "/command" ).arg( config + "/script" ), 0 );
				return NULL;
			}
			if( client->containsParameter( "~/" + config + "/command" ) && client->containsParameter( "~/" + config + "/script" ) )
			{
				Q_EMIT log( QString( "%1: remove menu.conf parameter %2 or %3" ).arg( e.attribute( "title" ) ).arg( config + "/command" ).arg( config + "/script" ), 0 );
				return NULL;
			}
		}
		else
		{
			if( false == e.hasAttribute( "command" ) && false == e.hasAttribute( "script" ) )
			{
				Q_EMIT log( QString( "%1: not command or script atribute given" ).arg( e.attribute( "title" ) ), 0 );
				return NULL;
			}
			if( e.hasAttribute( "command" ) && e.hasAttribute( "script" ) )
			{
				Q_EMIT log( QString( "%1: remove script or command atribute" ).arg( e.attribute( "title" ) ), 0 );
				return NULL;
			}
		}
		
		currentAction = new FileMenu( parent );
	}
	else if( "execute" == e.tagName() )
	{
		if( e.hasAttribute( "config" ) )
		{
			QString config = e.attribute( "config" );
			if( false == client->containsParameter( "~/" + config + "/title" ) )
			{
				Q_EMIT log( QString( "%1: menu.conf does not contain parameter %2" ).arg( e.attribute( "title" ) ).arg( config + "/title" ), 0 );
				return NULL;
			}
			if( false == client->containsParameter( "~/" + config + "/command" ) && false == client->containsParameter( "~/" + config + "/script" ) )
			{
				Q_EMIT log( QString( "%1: menu.conf does not contain parameter %2 or %3" ).arg( e.attribute( "title" ) ).arg( config + "/command" ).arg( config + "/script" ), 0 );
				return NULL;
			}
			if( client->containsParameter( "~/" + config + "/command" ) && client->containsParameter( "~/" + config + "/script" ) )
			{
				Q_EMIT log( QString( "%1: remove menu.conf parameter %2 or %3" ).arg( e.attribute( "title" ) ).arg( config + "/command" ).arg( config + "/script" ), 0 );
				return NULL;
			}
		}
		else
		{
			if( false == e.hasAttribute( "command" ) && false == e.hasAttribute( "script" ) )
			{
				Q_EMIT log( QString( "%1: not command or script atribute given" ).arg( e.attribute( "title" ) ), 0 );
				return NULL;
			}
			if( e.hasAttribute( "command" ) && e.hasAttribute( "script" ) )
			{
				Q_EMIT log( QString( "%1: remove script or command atribute" ).arg( e.attribute( "title" ) ), 0 );
				return NULL;
			}
		}
		
		currentAction = new Execute( parent );
	}
	else if( "controlactivated" == e.tagName() )
	{
		currentAction = new ControlActivated( parent );
		Action::_controlActivatedAction = currentAction;
	}
	else if( "ip4address" == e.tagName() )
	{
		currentAction = new IP4Address( parent );
	}
	else if( "ip4netmask" == e.tagName() )
	{
		currentAction = new IP4NetMask( parent );
	}
	else if( "cancel" == e.tagName() )
	{
		currentAction = new Cancel( parent );
	}
	else if( "apply" == e.tagName() )
	{
		currentAction = new Apply( parent );
	}
	else if( "ea09firmwareupdate" == e.tagName() )
	{
		currentAction = new EA09FirmwareUpdate( parent );
	}
	else if( "emergencybumper" == e.tagName() )
	{
		currentAction = new EmergencyBumper( parent );
	}
	else if( "setea09ip4address" == e.tagName() )
	{
		currentAction = new SetEA09IP4Address( parent );
	}
	else if( "requestshutdown" == e.tagName() )
	{
		currentAction = new RequestShutdown( parent );
	}
	else if( "readwpa" == e.tagName() )
	{
		currentAction = new ReadWPA( parent );
	}
	else if( "writewpa" == e.tagName() )
	{
		currentAction = new WriteWPA( parent );
	}
	else if( "lineedit" == e.tagName() )
	{
		currentAction = new LineEdit( parent );
	}
	else if( "combobox" == e.tagName() )
	{
		currentAction = new ComboBox( parent );
	}
	
	if( NULL == currentAction )
	{
		Q_EMIT log( QString( "Unknown action %1" ).arg( e.tagName() ), 0 );
		return NULL;
	}

	if( e.hasAttribute( "config" ) )
	{
		QString config = e.attribute( "config" );

		if( client->containsParameter( "~/" + config + "/func" ) )
		{
			currentAction->_attributes[ "func" ] = client->getParameter( "~/" + config + "/func" ).toString();
		}
		if( client->containsParameter( "~/" + config + "/title" ) )
		{
			QString str = client->getParameter( "~/" + config + "/title" ).toString();
			currentAction->_attributes[ "title" ] = str;
		}
		if( client->containsParameter( "~/" + config + "/command" ) )
		{
			QString str = client->getParameter( "~/" + config + "/command" ).toString();
			currentAction->_attributes[ "command" ] = str;
		}
		if( client->containsParameter( "~/" + config + "/script" ) )
		{
			currentAction->_attributes[ "script" ] = client->getParameter( "~/scriptpath" ).toString() + "/" + client->getParameter( "~/" + config + "/script" ).toString();
		}
		if( client->containsParameter( "~/" + config + "/iface" ) )
		{
			currentAction->_attributes[ "iface" ] = client->getParameter( "~/" + config + "/iface" ).toString();
		}
		if( client->containsParameter( "~/" + config + "/cond" ) )
		{
			currentAction->_attributes[ "cond" ] = client->getParameter( "~/" + config + "/cond" ).toString();
		}
		if( client->containsParameter( "~/" + config + "/root" ) )
		{
			currentAction->_attributes[ "root" ] = client->getParameter( "~/" + config + "/root" ).toString();
		}
		if( client->containsParameter( "~/" + config + "/filter" ) )
		{
			currentAction->_attributes[ "filter" ] = client->getParameter( "~/" + config + "/filter" ).toString();
		}

		QString paramStr = "~/" + config + "/param";
		if( client->containsParameter( paramStr ) )
		{
			currentAction->_attributes[ "param" ] = client->getParameter( paramStr ).toString();
			for( int i=1; i<8; ++i )
			{
				QString tmp = paramStr + QString::number( i );
				if( false == client->containsParameter( tmp ) )
				{
					break;
				}
				else
				{
					currentAction->_attributes[ QString( "param%1" ).arg( i ) ] = client->getParameter( tmp ).toString();
				}
			}
		}
	}
	else
	{
		QDomNamedNodeMap m = e.attributes();
		for( int i=0; i<m.size(); ++i )
		{
			QDomAttr attr = m.item( i ).toAttr();
			if( false == attr.isNull() )
			{
				QString value = attr.value();

				if( "script" == attr.name() )
				{
					currentAction->_attributes[ attr.name() ] = client->getParameter( "~/scriptpath" ).toString() + "/" + value;
				}
				else
				{
					currentAction->_attributes[ attr.name() ] = value;
				}
			}
		}
	}

	Q_EMIT log( "Loaded: " + currentAction->fullPath(), 0 );

	QDomNodeList children = e.childNodes();
	for( int i=0; i<children.size(); ++i )
	{
		QDomElement e = children.at( i ).toElement(); // try to convert the node to an element.
		if( !e.isNull() )
		{
			loadAction( e, currentAction );
		}
	}

	return currentAction;
}
