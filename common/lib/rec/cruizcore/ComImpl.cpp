#include "rec/cruizcore/ComImpl.h"
//#include <iostream>
#include <sstream>

#ifdef WIN32
#include <windows.h>
#include <setupapi.h>
#include <winioctl.h>
#include <vector>
#include <cassert>
#endif

#define ROBOTINO3_GYROSCOPE_VID 0x1e29
#define ROBOTINO3_GYROSCOPE_PID 0x0411
#define ROBOTINO3_GYROSCOPE_USB_NAME "REC_GmbH_Robotino_3_Gyroscope"

#define CHECK_COMMUNICATION_TIMEOUT 1000 //ms

#define PI 3.14159265f

using namespace rec::cruizcore;

ComImpl::ComImpl( Com* com )
: _com( com )
, _run( true )
, _reset( false )
, _sequence(0)
{
}

ComImpl::~ComImpl()
{
	close();
}

bool ComImpl::open( rec::serialport::Port port )
{
	if( rec::serialport::UNDEFINED == port )
	{
#ifdef WIN32
		for( int i=(int)rec::serialport::COM1; i<=(int)rec::serialport::COM11; ++i )
#else
		for( int i=(int)rec::serialport::USB0; i<(int)rec::serialport::USB7; ++i )
#endif
		{
			if( open( (rec::serialport::Port)i ) )
			{
				return true;
			}
		}
	}
	else
	{
		if( open_i( port ) )
		{
			return true;
		}
	}

	return false;
}

bool ComImpl::open( const QString& port )
{
	try
	{
		close();

		_serialPort.open( port.toStdString(), 115200, 100 );
		setParameters();

		_run = true;
		start();
		
		if (_comActiveSemaphore.tryAcquire(1, CHECK_COMMUNICATION_TIMEOUT))
		{
			return true;
		}
	}
	catch( const rec::serialport::SerialPortException& )
	{
	}

	close();
	return false;
}

bool ComImpl::open()
{
#ifdef WIN32
	const int vendorId = ROBOTINO3_GYROSCOPE_VID;
	const int productId = ROBOTINO3_GYROSCOPE_PID;
	_usbId = QString( "FTDIBUS\\VID_%1+PID_%2" ).arg( vendorId, 4, 16, QChar( '0' ) ).arg( productId, 4, 16,  QChar( '0' ) );
	_usbId = _usbId.toUpper();
#else
	_usbName = ROBOTINO3_GYROSCOPE_USB_NAME;
#endif

	QString portStr;

#ifdef WIN32
	QByteArray enumStr = _usbId.toLatin1();
	//HDEVINFO devInfoSet = SetupDiGetClassDevsA(&GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR, enumStr.constData(), NULL, DIGCF_PRESENT);
	HDEVINFO devInfoSet = SetupDiGetClassDevsA(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
	if (devInfoSet == INVALID_HANDLE_VALUE)
	{
		QString str = QString("No USB devices found");
		_com->errorReport(str.toLatin1().constData());
		return false;
	}

	SP_DEVINFO_DATA devInfo;
	ZeroMemory(&devInfo, sizeof(SP_DEVINFO_DATA));
	devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
	int deviceIndex = 0;
	bool found = false;
	while (SetupDiEnumDeviceInfo(devInfoSet, deviceIndex, &devInfo))
	{
		++deviceIndex;

		WCHAR DeviceInstanceId[1024];
		SetupDiGetDeviceInstanceId(devInfoSet, &devInfo, DeviceInstanceId, sizeof(DeviceInstanceId), NULL);
		
		QString devIdStr = QString::fromWCharArray(DeviceInstanceId);

		if (devIdStr.startsWith(_usbId))
		{
			//char buffer[256];
			//sprintf(buffer, "Guid = {%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}",
			//	devInfo.ClassGuid.Data1, devInfo.ClassGuid.Data2, devInfo.ClassGuid.Data3,
			//	devInfo.ClassGuid.Data4[0], devInfo.ClassGuid.Data4[1], devInfo.ClassGuid.Data4[2], devInfo.ClassGuid.Data4[3],
			//	devInfo.ClassGuid.Data4[4], devInfo.ClassGuid.Data4[5], devInfo.ClassGuid.Data4[6], devInfo.ClassGuid.Data4[7]);
			//_com->errorReport(buffer);
			//_com->errorReport(devIdStr.toLatin1());
			found = true;
			break;
		}
	}

	if (false == found)
	{
		QString str = QString("SetupDiEnumDeviceInfo: Error searching for %1").arg(_usbId);
		_com->errorReport(str.toLatin1().constData());
		SetupDiDestroyDeviceInfoList(devInfoSet);
		return false;
	}

	HKEY deviceKey = SetupDiOpenDevRegKey(devInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
	if (!deviceKey)
	{
		QString str = QString("SetupDiOpenDevRegKey: Error searching for %1").arg(_usbId);
		_com->errorReport(str.toLatin1().constData());
		SetupDiDestroyDeviceInfoList(devInfoSet);
		return false;
	}

	SetupDiDestroyDeviceInfoList(devInfoSet);

	std::string portName(256, '\0');
	DWORD size = portName.size();
	DWORD type = 0;
	LSTATUS result = RegQueryValueExA(deviceKey, "PortName", NULL, &type, reinterpret_cast< LPBYTE >(&portName[0]), &size);
	RegCloseKey(deviceKey);
	if ((result != ERROR_SUCCESS) || (type != REG_SZ))
	{
		QString str = QString("RegQueryValueExA: Error searching for %1").arg(_usbId);
		_com->errorReport(str.toLatin1().constData());
		return false;
	}

	portStr = QString::fromLatin1(portName.c_str());

#ifdef _DEBUG
	assert(portStr.startsWith("COM"));
	bool ok = false;
	portStr.right(portStr.length() - 3).toInt(&ok);
	assert(ok);
#endif

#else
	QDir dir("/dev/serial/by-id/");
	QStringList entries = dir.entryList();
	Q_FOREACH(const QString& str, entries)
	{
		if (str.contains(_usbName))
		{
			portStr = "/dev/serial/by-id/" + str;
			break;
		}
	}
	if (portStr.isEmpty())
	{
		return false;
	}
	//portStr = QString( "/dev/robotino3io" );
#endif

	return open(portStr.toLatin1().constData());
}

bool ComImpl::isOpen() const
{
	return _serialPort.isOpen();
}

void ComImpl::close()
{
	stop();
	_serialPort.close();
}

void ComImpl::reset()
{
	if (isOpen())
	{
		_reset = true;
	}
}

QString ComImpl::portString() const
{
	QString str = rec::serialport::friendlyName( _serialPort.port() ).c_str();
	return str;
}

rec::serialport::Port ComImpl::port() const
{
	return _serialPort.port();
}

void ComImpl::run()
{
	_sequence = 0;

	int errorCounter = 0;
	unsigned char buffer[8];
	memset(buffer, 0, sizeof(buffer));
	int bufferIndex = 0;

	QTime dataTimer;
	dataTimer.start();
	
	while( _run )
	{	
		if( errorCounter > 2 )
		{
			setParameters();
			errorCounter = 0;
			_serialPort.flush();
		}

		if (_reset)
		{
			const char* resetCommand = "$MIB,RESET*87";
			_serialPort.write((const unsigned char*)resetCommand, strlen(resetCommand));
			_reset = false;
		}

		unsigned char ch;
		int bytes_read = _serialPort.read( &ch, 1 );

		if (1 == bytes_read)
		{
			if (bufferIndex<2)
			{
				if (0xFF == ch)
				{
					buffer[bufferIndex++] = ch;
				}
			}
			else
			{
				buffer[bufferIndex++] = ch;
			}

			if (8 == bufferIndex)
			{
				float rate = 0.0f;
				float angle = 0.0f;

				if (parse_data(buffer, &rate, &angle))
				{
					errorCounter = 0;

					if (0 == _sequence)
					{
						_comActiveSemaphore.release();
					}
					++_sequence;
					dataTimer.restart();

					_com->dataReceivedEvent(rate, angle);
				}
				else
				{
					_com->errorReport("CruizCore: Error parsing data");
					++errorCounter;
				}

				bufferIndex = 0;
			}
		}
		
		if(dataTimer.elapsed() > 1000 )
		{
			_com->errorReport( "Could not read vaild data package for mor than 2000ms." );
			_run = false;
		}
	}

	_com->errorReport( "CruizCore receive thread stopped" );
	_com->stopped();
}

void ComImpl::stop()
{
	_run = false;
	wait( 1000 );
}

bool ComImpl::open_i( rec::serialport::Port port )
{
	try
	{
		close();

		_serialPort.open( port, 115200, 100 );
		setParameters();

		_run = true;
		start();

		if (_comActiveSemaphore.tryAcquire(1, CHECK_COMMUNICATION_TIMEOUT))
		{
			return true;
		}
	}
	catch( const rec::serialport::SerialPortException& )
	{
	}

	close();
	return false;
}

bool ComImpl::parse_data( const unsigned char* data, float* grate, float* gangle ) const
{
	short int rate;
	short int angle;
	short check_sum;
	//Verify packet heading information
	if( data[0] != 0xFF || data[1] != 0xFF )
	{
		_com->errorReport( "Data header error" );
		return false;
	}
	//Assemble data
	rate = ( data[2] & 0xFF) | ((data[3] << 8) & 0xFF00);
	angle = ( data[4] & 0xFF) | ((data[5] << 8) & 0XFF00);
	//Verify checksum
	check_sum = 0xFFFF + rate + angle;
	if(((unsigned char)(check_sum & 0xFF) != data[6]) ||
		((unsigned char)((check_sum>>8) & 0xFF) != data[7]))
	{
		_com->errorReport( "Checksum mismatch error" );
		return false;
	}

	//Scale and store data
	*grate = - PI * rate / 18000.0f;
	*gangle = - PI * angle / 18000.0f;

	return true;
}

void ComImpl::setParameters()
{
	unsigned char buf[30];

	buf[0] = '$';
	buf[1] = 'M';
	buf[2] = 'I';
	buf[3] = 'A';
	buf[4] = ',';
	buf[5] = 'I';
	buf[6] = ',';
	buf[7] = 'B';
	buf[8] = ',';
	buf[9] = '1';
	buf[10] = '1';
	buf[11] = '5';
	buf[12] = '2';
	buf[13] = '0';
	buf[14] = '0';
	buf[15] = ',';
	buf[16] = 'R';
	buf[17] = ',';
	buf[18] = '1';
	buf[19] = '0';
	buf[20] = '0';
	buf[21] = ',';
	buf[22] = 'D';
	buf[23] = ',';
	buf[24] = 'Y';
	buf[25] = ',';
	buf[26] = 'N';
	buf[27] = '*';
	buf[28] = 0;
	buf[29] = 0;

	unsigned int checksum = 0;
	for( int i=1; i<27; ++i )
	{
		checksum += buf[i];
	}

	checksum &= 0xFF;

	std::ostringstream os;
	os << std::hex << std::uppercase << checksum;

	std::string str = os.str();

	buf[28] = str[0];
	buf[29] = str[1];

	_serialPort.write( buf, 30 );
}
