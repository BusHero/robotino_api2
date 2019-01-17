#include "DeviceInfo.h"

#include <WinIoCtl.h>
#include <SetupAPI.h>

class HANDLEPtr
{
public:
	HANDLEPtr( HANDLE h )
		: _h( h )
	{
	}

	~HANDLEPtr()
	{
		if ( _h && _h != INVALID_HANDLE_VALUE )
			CloseHandle( _h );
	}

	operator HANDLE() const
	{
		return _h;
	}

private:
	HANDLE _h;
};

bool DeviceInfo::ejectDrive( QChar driveLetter )
{
	if ( driveLetter.isNull() || !driveLetter.isLetter() )
		return false;

	QString devicePath( "\\\\.\\?:" );
	devicePath[4] = driveLetter;

	DWORD bytesReturned = 0;
	HANDLEPtr vol = CreateFile( reinterpret_cast< const wchar_t* >( devicePath.utf16() ), GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );
	if( vol == INVALID_HANDLE_VALUE )
		return false;

	if( !DeviceIoControl( vol, FSCTL_LOCK_VOLUME, 0, 0, 0, 0, &bytesReturned, 0 ) )
		return false;

	if( !DeviceIoControl( vol, FSCTL_DISMOUNT_VOLUME, 0, 0, 0, 0, &bytesReturned, 0 ) )
		return false;

	DeviceIoControl( vol, IOCTL_STORAGE_EJECT_MEDIA, 0, 0, 0, 0, &bytesReturned, 0 );

	return true;
}

QChar DeviceInfo::getDriveLetterFromHardwareId( int vid, int pid )
{
	DEVINST instance = getDevInstFromUsbDev( vid, pid );
	if ( !instance )
		return QChar();

	QMap< DEVINST, long > inst2num = getDevInstsAndDevNums( &GUID_DEVINTERFACE_DISK );
	if ( !inst2num.contains( instance ) )
		return QChar();
	long num = inst2num.value( instance );

	QMap< long, QChar > num2letter = getDevNumsAndDriveLetters( DRIVE_REMOVABLE );
	return num2letter.value( num );
}

QMap< long, QChar > DeviceInfo::getDevNumsAndDriveLetters( UINT driveType )
{
	QMap< long, QChar > result;

	QString drivePath( "?:\\" );
	QString devicePath( "\\\\.\\?:" );

	for( char letter = 'A'; letter <= 'Z'; ++letter )
	{
		drivePath[0] = letter;
		UINT type = GetDriveType( reinterpret_cast< const wchar_t* >( drivePath.utf16() ) );
		if ( type != driveType )
			continue;

		devicePath[4] = letter;
		HANDLEPtr hVol = CreateFile( reinterpret_cast< const wchar_t* >( devicePath.utf16() ), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if( INVALID_HANDLE_VALUE == hVol )
			continue;

		STORAGE_DEVICE_NUMBER sdn;
		DWORD dwBytesReturned = 0;
		if( DeviceIoControl( hVol, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &sdn, sizeof( sdn ), &dwBytesReturned, NULL ) )
			result.insert( sdn.DeviceNumber, QChar( letter ) );

	}
	return result;
}

QMap< DEVINST, long > DeviceInfo::getDevInstsAndDevNums( const GUID* devClass )
{
	QMap< DEVINST, long > result;

	// Get device interface info set handle
	// for all devices attached to system
	HDEVINFO hDevInfo = SetupDiGetClassDevs( devClass, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE );
	if( hDevInfo == INVALID_HANDLE_VALUE )
		return result;

	// Retrieve a context structure for a device interface of a device information set.
	QByteArray buf;
	buf.resize( 1024 );
	PSP_DEVICE_INTERFACE_DETAIL_DATA pspdidd = reinterpret_cast< PSP_DEVICE_INTERFACE_DETAIL_DATA >( buf.data() );
	SP_DEVICE_INTERFACE_DATA spdid;
	SP_DEVINFO_DATA spdd;
	DWORD dwSize;

	spdid.cbSize = sizeof( spdid );

	// Iterate through all the interfaces and try to match one based on
	// the device number.
	for( DWORD i = 0; SetupDiEnumDeviceInterfaces(hDevInfo, NULL, devClass, i, &spdid ); i++)
	{
		// Get the device path.
		dwSize = 0;
		SetupDiGetDeviceInterfaceDetail( hDevInfo, &spdid, NULL, 0, &dwSize, NULL );
		if( dwSize == 0 )
			continue;

		if( dwSize > buf.size() )
		{
			buf.resize( dwSize );
			pspdidd = reinterpret_cast< PSP_DEVICE_INTERFACE_DETAIL_DATA >( buf.data() );
		}

		pspdidd->cbSize = sizeof( *pspdidd );
		ZeroMemory( &spdd, sizeof( spdd ) );
		spdd.cbSize = sizeof( spdd );
		if( !SetupDiGetDeviceInterfaceDetail( hDevInfo, &spdid, pspdidd, dwSize, &dwSize, &spdd ) )
			continue;

		// Open the device.
		HANDLEPtr hDrive = CreateFile( pspdidd->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );
		if( hDrive == INVALID_HANDLE_VALUE )
			continue;

		// Get the device number.
		STORAGE_DEVICE_NUMBER sdn;
		dwSize = 0;
		if( DeviceIoControl( hDrive, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &sdn, sizeof( sdn ), &dwSize, NULL ) )
		{
			result.insert( spdd.DevInst, sdn.DeviceNumber );
		}
	}

	SetupDiDestroyDeviceInfoList( hDevInfo );
	return result;
}

DEVINST DeviceInfo::getDevInstFromUsbDev( int vid, int pid )
{
	QString id = QString( "USB\\VID_%1&PID_%2" ).arg( vid, 4, 16, QChar( '0' ) ).arg( pid, 4, 16, QChar( '0' ) );

	// Get a list of hardware IDs for all USB devices.
	ULONG ulLen;
	CM_Get_Device_ID_List_Size( &ulLen, 0, CM_GETIDLIST_FILTER_NONE );
	
	QScopedArrayPointer< wchar_t > pszBuffer( new wchar_t[ulLen] );
	CM_Get_Device_ID_List( 0, pszBuffer.data(), ulLen, CM_GETIDLIST_FILTER_NONE );

	// Iterate through the list looking for our ID.
	for( wchar_t* pszDeviceID = pszBuffer.data(); *pszDeviceID; pszDeviceID += wcslen( pszDeviceID ) + 1)
	{
		QString devId = QString::fromUtf16( reinterpret_cast< const ushort* >( pszDeviceID ) );

		// Some versions of Windows have the string in upper case and other versions have it
		// in lower case so just do a case-insensitive search.
		if( devId.contains( id, Qt::CaseInsensitive ) )
		{
			// Found the device, now we want the grandchild device, which is the "generic volume"
			DEVINST MSDInst = 0;
			if( CR_SUCCESS == CM_Locate_DevNode( &MSDInst, pszDeviceID, CM_LOCATE_DEVNODE_NORMAL ) )
			{
				DEVINST DiskDriveInst = 0;
				if( CR_SUCCESS == CM_Get_Child( &DiskDriveInst, MSDInst, 0 ) )
				{
					return DiskDriveInst;
				}
			}
		}
	}

	return 0;
}
