//  Copyright (C) 2004-2011, Robotics Equipment Corporation GmbH

#include <QtCore>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

int main( int argc, char** argv )
{
	QApplication app( argc, argv );

	QString verStr = "not available";

	QFile verFile( "/version.txt" );
	if( verFile.open( QIODevice::ReadOnly ) )
	{
		QTextStream stream( &verFile );
		verStr = stream.readLine();

		verFile.close();
	}
	
	QMessageBox::about( NULL, "Robotino image", "Version: " + verStr );

	return 0;
}
