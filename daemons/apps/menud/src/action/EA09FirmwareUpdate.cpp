#include "action/EA09FirmwareUpdate.h"
#include "action/Execute.h"
#include "Client.h"
#include "Translator.h"

extern Client* client;

using namespace action;

EA09FirmwareUpdate::EA09FirmwareUpdate( Action* parent )
: Menu( parent )
, _firmware_major( 0 )
, _firmware_minor( 0 )
, _firmware_patch( 0 )
, _package_major( 0 )
, _package_minor( 0 )
, _package_patch( 0 )
{
	_customLine = " ";

	bool connected = true;
	connected &= connect( client, SIGNAL( ea_version_changed( const QString&, quint16, quint16, quint16 ) ), SLOT( on_ea_version_changed( const QString&, quint16, quint16, quint16 ) ) );
	Q_ASSERT( connected );
}

void EA09FirmwareUpdate::customActivate( Action* before )
{
	_firstItem = 0;
	_selectedItem = 0;

	_packageFileName.clear();
	_package_major = 0;
	_package_minor = 0;
	_package_patch = 0;

	QDir dir( attribute( "packages_dir" ) );
	QStringList l = dir.entryList( QDir::Files );
	Q_FOREACH( const QString& fileName, l )
	{
		if( fileName.startsWith( attribute( "prefix" ) ) && fileName.endsWith( attribute( "postfix" ) ) )
		{
			QString str = fileName;
			str.remove( attribute( "prefix" ) );
			str.remove( attribute( "postfix" ) );
			QStringList strl = str.split( "." );
			if( 3 == strl.size() )
			{
				_package_major = strl.at( 0 ).toShort();
				_package_minor = strl.at( 1 ).toShort();
				_package_patch = strl.at( 2 ).toShort();

				_packageFileName = dir.absoluteFilePath( fileName );
			}
		}
	}

	if( _board.isEmpty() )
	{
		_customLine = TT( "Nicht verfügbar" );
		drawDisplay();
	}
	else
	{
		updateFromToStr();

		Q_FOREACH( Action* a, _children )
		{
			action::Execute* e = qobject_cast<Execute*>( a );
			if( e )
			{
				e->setAttribute( "command", attribute( "command" ) );
				QStringList params = parameters();
				params << _packageFileName;
				e->setParameters( params );
			}
		}
	}

	Menu::customActivate( before );
}

QString EA09FirmwareUpdate::title() const
{
	if( _board.isEmpty() )
	{
		return Action::title();
	}
	else
	{
		return _board + " " + TT( "Update" );
	}
}

void EA09FirmwareUpdate::on_ea_version_changed( const QString& board, quint16 firmware_major, quint16 firmware_minor, quint16 firmware_patch )
{
	_board = board.toUpper();

	_firmware_major = firmware_major;
	_firmware_minor = firmware_minor;
	_firmware_patch = firmware_patch;

	if( this == Action::currentAction() )
	{
		updateFromToStr();
	}
}

void EA09FirmwareUpdate::updateFromToStr()
{
	_customLine = QString("%1.%2.%3->%4.%5.%6").arg( _firmware_major ).arg( _firmware_minor ).arg( _firmware_patch ).arg( _package_major ).arg( _package_minor ).arg( _package_patch );
	drawDisplay();
}
