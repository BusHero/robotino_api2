#include "action/FileMenu.h"
#include "action/Execute.h"

using namespace action;

FileMenu::FileMenu( Action* parent )
: Menu( parent )
{
}

FileMenu::FileMenu( const FileMenu& other )
: Menu( other._parent )
{
	setAttributes( other._attributes );
}

void FileMenu::customActivate( Action* before )
{
	if( _children.contains( before ) )
	{
		before = NULL;
	}

	qDeleteAll( _children );
	_children.clear();

	QDir dir( attribute( "root" ) );

	QFileInfoList il = dir.entryInfoList( QDir::Dirs | QDir::NoDotAndDotDot );
	Q_FOREACH( const QFileInfo& info, il )
	{
		if( info.isDir() )
		{
			FileMenu* a = new FileMenu( *this );
			a->setAttribute( "title", info.fileName() );
			a->setAttribute( "root", info.absoluteFilePath() );
		}
	}

	dir.setNameFilters( QStringList() << attribute( "filter" ) );

	QStringList l = dir.entryList( QDir::Files );
	Q_FOREACH( const QString& str, l )
	{
		Execute* a = new Execute( this );
		a->setAttribute( "title", str );

		QStringList params = parameters();

		if( _attributes.contains( "command" ) )
		{
			a->setAttribute( "command", attribute( "command" ) );
			a->setAttribute( "wd", attribute( "wd" ) );
			params << dir.absolutePath() + "/" + str;
		}
		else
		{
			a->setAttribute( "command", str );
			a->setAttribute( "wd", dir.absolutePath() );
		}

		a->setParameters( params );
	}

	Menu::customActivate( before );
}
