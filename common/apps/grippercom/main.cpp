//  Copyright (C) 2004-2011, Robotics Equipment Corporation GmbH

#include "grippercom_version.h"
#include <QtGui>
#include "MainWindow.h"

int main( int argc, char** argv )
{
	QApplication app( argc, argv );

	MainWindow mw;
	mw.show();

	return app.exec();
}
