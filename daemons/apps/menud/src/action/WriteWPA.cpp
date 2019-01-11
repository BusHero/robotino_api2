#include "action/WriteWPA.h"
#include "action/ReadWPA.h"

using namespace action;

WriteWPA::WriteWPA( Action* parent )
: Execute( parent )
{
}

void WriteWPA::customActivate( Action* before )
{
	Action* readwpa = NULL;
	Q_FOREACH( Action* a, _parent->children() )
	{
		readwpa = dynamic_cast<action::ReadWPA*>( a );
		if( readwpa )
		{
			break;
		}
	}

	if( NULL == readwpa )
	{
		log( "WriteWPA::customActivate: Can not find ReadWPA action in parents children", 0 );
		activate( _parent );
		return;
	}

	Action* ssid = _parent->findChildByTitle( "ssid" );
	if( NULL == ssid )
	{
		log( "WriteWPA::customActivate: Can not find ssid action in parents children", 0 );
		activate( _parent );
		return;
	}

	Action* proto = _parent->findChildByTitle( "proto" );
	if( NULL == proto )
	{
		log( "WriteWPA::customActivate: Can not find proto action in parents children", 0 );
		activate( _parent );
		return;
	}

	Action* pairwise = _parent->findChildByTitle( "pairwise" );
	if( NULL == pairwise )
	{
		log( "WriteWPA::customActivate: Can not find pairwise action in parents children", 0 );
		activate( _parent );
		return;
	}

	Action* group = _parent->findChildByTitle( "group" );
	if( NULL == group )
	{
		log( "WriteWPA::customActivate: Can not find group action in parents children", 0 );
		activate( _parent );
		return;
	}

	Action* psk = _parent->findChildByTitle( "psk" );
	if( NULL == psk )
	{
		log( "WriteWPA::customActivate: Can not find psk action in parents children", 0 );
		activate( _parent );
		return;
	}

	clearParameters();

	QStringList params;
	params << readwpa->attribute( "file" );
	params << ssid->attribute( "value" );
	params << proto->attribute( "value" );
	params << pairwise->attribute( "value" );
	params << group->attribute( "value" );
	params << psk->attribute( "value" );

	setParameters( params );

	action::Execute::customActivate( before );
}
