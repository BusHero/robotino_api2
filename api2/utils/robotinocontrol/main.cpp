#include <QtGui>
#include "MainWindow.h"

int main( int argc, char** argv )
{
	QApplication app( argc, argv );

	QCoreApplication::setOrganizationName("REC GmbH");
	QCoreApplication::setOrganizationDomain("servicerobotics.eu");
	QCoreApplication::setApplicationName("robotinocontrol");

	MainWindow mw;
	mw.show();

	return app.exec();
}