#include "Wizard.h"
#include "Com.h"
#include "HelpPage.h"

Wizard::Wizard()
	: _firstUSBPageId( 0 )
	, _lastUSBPageId( 0 )
{
	setMinimumSize( 800, 600 );
	this->setWizardStyle( QWizard::ModernStyle );
	this->setPixmap( QWizard::LogoPixmap, QPixmap( ":/appicon/64.png" ) );
	this->setPixmap( QWizard::BannerPixmap, QPixmap( ":/appicon/64.png" ) );

	bool ok;
	ok = connect( this, SIGNAL( currentIdChanged(int) ), SLOT( on_currentIdChanged(int) ) );
	Q_ASSERT( ok );

	ok = connect( this, SIGNAL( customButtonClicked(int) ), SLOT( on_customButtonClicked(int) ) );
	Q_ASSERT( ok );

	setButtonText( QWizard::CustomButton1, "Überspringen" );
	setOption( QWizard::HaveCustomButton1, true );
}

int Wizard::addFirstUSBPage( QWizardPage* page )
{
	_firstUSBPageId = addPageWithSkipButton( page );
	return _firstUSBPageId;
}

int Wizard::addLastUSBPage( QWizardPage* page )
{
	_lastUSBPageId = addPageWithSkipButton( page );
	return _lastUSBPageId;
}

int Wizard::addPageWithSkipButton( QWizardPage* page )
{
	int id = addPage( page );
	_pagesWithSkipButton.insert( id );
	return id;
}

void Wizard::on_currentIdChanged(int id)
{
	if( id<_firstUSBPageId || id>_lastUSBPageId )
	{
		Com::singleton()->stop();
	}
	else
	{
		Com::singleton()->start();
	}

	HelpPage* p = dynamic_cast<HelpPage*>( page( id ) );

	if( _pagesWithSkipButton.contains(id) )
	{
		setOption( QWizard::HaveCustomButton1, true );
	}
	else
	{
		setOption( QWizard::HaveCustomButton1, false );
	}
}

void Wizard::on_customButtonClicked( int which )
{
	switch( which )
	{
	case QWizard::CustomButton1:
		next();
		break;

	default:
		break;
	}
}
