//  Copyright (C) 2004-2011, Robotics Equipment Corporation GmbH

#include "chargertest_version.h"
#include <QtGui>
#include "MainWindow.h"

#include "CANHandler.h"

int main( int argc, char** argv )
{
	QApplication app( argc, argv );

	if( CAN_ERR_OK != CAN_Init( CAN_BAUD_125K, 0) )
	{
		QMessageBox::critical( NULL, "CAN error", "No PCAN-USB interface found" );
		return 1;
	}

	CAN_MsgFilter( 0x0, 0x7FF, 0 );

	qRegisterMetaType<CANMessage>();

	MainWindow mw;
	mw.show();

	int ret = app.exec();

	CAN_Close();

	return ret;
}
