#ifndef _DEVICEINFO_H_
#define _DEVICEINFO_H_

#include <QtCore>
#include <Windows.h>
#include <cfgmgr32.h>

class DeviceInfo
{
public:
	static bool ejectDrive( QChar driveLetter );
	static QChar getDriveLetterFromHardwareId( int vid, int pid );

private:
	static DEVINST getDevInstFromUsbDev( int vid, int pid );
	static QMap< long, QChar > getDevNumsAndDriveLetters( UINT driveType );
	static QMap< DEVINST, long > getDevInstsAndDevNums( const GUID* devClass );
};

#endif
