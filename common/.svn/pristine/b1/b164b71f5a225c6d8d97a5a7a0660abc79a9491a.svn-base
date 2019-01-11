//  Copyright (C) 2004-2011, Robotics Equipment Corporation GmbH

#include "iotest_version.h"
#include <QtGui>
#include "Config.h"
#include "Com.h"
#include "Wizard.h"
#include "WelcomePage.h"
#include "ConnectJTAGPage.h"
#include "InstallFirmwarePage.h"
#include "ConnectUSBPage.h"
#include "ConnectUSBPage2.h"
#include "TestDigitalPage.h"
#include "TestAnalogPage.h"
#include "TestRelayPage.h"
#include "TestBumperPage.h"
#include "TestDistanceSensorPage.h"
#include "TestMotorPage.h"
#include "TestGyroPage.h"
#include "TestChargerPage.h"
#include "TestEthernetPage.h"
#include "SummaryPage.h"
#include "GyroscopeProgramPage.h"
#include "GyroscopeTestPage.h"

int gyroscopeTestPageId = -1;

int main( int argc, char** argv )
{
	QApplication app( argc, argv );

	Config::init();
	Com::init();

	Wizard wizard;
	wizard.addPage( new WelcomePage );
	wizard.addPage( new ConnectJTAGPage );
	wizard.addPageWithSkipButton(new InstallFirmwarePage);
	wizard.addPageWithSkipButton(new GyroscopeProgramPage);
	gyroscopeTestPageId = wizard.addPageWithSkipButton(new GyroscopeTestPage);
	wizard.addPage( new ConnectUSBPage );
	wizard.addFirstUSBPage( new ConnectUSBPage2 );
	wizard.addPageWithSkipButton( new TestDigitalPage );
	wizard.addPageWithSkipButton( new TestAnalogPage );
	wizard.addPageWithSkipButton( new TestRelayPage );
	wizard.addPageWithSkipButton( new TestBumperPage );
	wizard.addPageWithSkipButton( new TestDistanceSensorPage );
	wizard.addPageWithSkipButton( new TestMotorPage(0) );
	wizard.addPageWithSkipButton( new TestMotorPage(1) );
	wizard.addPageWithSkipButton( new TestMotorPage(2) );
	wizard.addPageWithSkipButton( new TestGyroPage );
	wizard.addPageWithSkipButton( new TestChargerPage(0) );
	wizard.addPageWithSkipButton( new TestChargerPage(1) );
	wizard.addLastUSBPage( new TestChargerPage(2) );
	wizard.addPageWithSkipButton( new TestEthernetPage );
	wizard.addPage( new SummaryPage );
	
	wizard.show();

	int ret = app.exec();

	Com::done();
	Config::done();

	return ret;
}
