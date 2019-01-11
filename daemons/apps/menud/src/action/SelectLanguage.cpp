#include "action/SelectLanguage.h"
#include "Translator.h"

using namespace action;

SelectLanguage::SelectLanguage( Action* parent )
: Action( parent )
{
}

void SelectLanguage::customActivate( Action* before )
{
	QString key = attribute( "key" );
	log( QString( "Set language to %1" ).arg( key ), 0 );
	Translator::setLanguage( key );
	activate( _parent );
}
