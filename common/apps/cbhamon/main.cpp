#include <QtCore>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "MainWindow.h"

int main( int argc, char** argv )
{
	QApplication app( argc, argv );

	QCoreApplication::setOrganizationName("REC GmbH");
	QCoreApplication::setOrganizationDomain("servicerobotics.eu");
	QCoreApplication::setApplicationName("cbhamon");

	MainWindow mw;
	mw.show();

	return app.exec();
}