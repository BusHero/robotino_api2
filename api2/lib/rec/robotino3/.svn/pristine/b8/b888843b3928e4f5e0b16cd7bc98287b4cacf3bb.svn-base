#include "rec/robotino3/serialio/Com.h"
#include "rec/robotino3/serialio/TagFwd.h"
#include "rec/robotino3/serialio/Producer.h"
#include "rec/robotino3/serialio/Parser.h"
#include "rec/robotino3/serialio/utils.h"

#include "rec/robotino3/serialio/tag/GetHwVersion.h"
#include "rec/robotino3/serialio/tag/HwVersion.h"
#include "rec/robotino3/serialio/tag/GetSwVersion.h"
#include "rec/robotino3/serialio/tag/SwVersion.h"

#include "rec/serialport/SerialPort.h"

#include <QtCore>

#ifdef WIN32
#include <windows.h>
#include <setupapi.h>
#include <winioctl.h>
#include <vector>
#include <cassert>
#endif

using namespace rec::robotino3::serialio;

Com::Com( rec::robotino3::serialio::Decoder* decoder )
	: _serial( NULL )
	, _parser( NULL )
	, _producer( NULL )
	, _decoder( decoder )
	, _verbosity( 1 )
{
}

Com::~Com()
{
	delete _serial;
	delete _parser;
	delete _producer;
	delete _decoder;
}

#ifdef WIN32
void Com::setUsbId( int vendorId, int productId )
{
	_usbId = QString( "USB\\VID_%1&PID_%2" ).arg( vendorId, 4, 16, QChar( '0' ) ).arg( productId, 4, 16,  QChar( '0' ) );
	_usbId = _usbId.toUpper();
}
#else
	void Com::setUsbName( const QString& name )
	{
		_usbName = name;
	}
#endif

bool Com::isOpen() const
{
	if( _serial )
	{
		return _serial->isOpen();
	}
	else
	{
		return false;
	}
}

bool Com::open()
{
	QString portStr;

#ifdef WIN32
	QByteArray enumStr = _usbId.toLatin1();
	HDEVINFO devInfoSet = SetupDiGetClassDevsA( &GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR, enumStr.constData(), NULL, DIGCF_PRESENT );
	if ( devInfoSet == INVALID_HANDLE_VALUE )
	{
		QString str = QString( "Error searching for %1" ).arg( _usbId );
		logCb( str.toLatin1().constData() );
		return false;
	}

	SP_DEVINFO_DATA devInfo;
	devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
	if ( !SetupDiEnumDeviceInfo( devInfoSet, 0, &devInfo ) )
	{
		QString str = QString( "Error searching for %1" ).arg( _usbId );
		logCb( str.toLatin1().constData() );
		return false;
	}

	HKEY deviceKey = SetupDiOpenDevRegKey( devInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE );
	if ( !deviceKey )
	{
		QString str = QString( "Error searching for %1" ).arg( _usbId );
		logCb( str.toLatin1().constData() );
		return false;
	}

	std::string portName( 256, '\0' );
	DWORD size = portName.size();
	DWORD type = 0;
	LSTATUS result = RegQueryValueExA(deviceKey, "PortName", NULL, &type, reinterpret_cast< LPBYTE >( &portName[0] ), &size );
	RegCloseKey( deviceKey );
	if ( ( result != ERROR_SUCCESS ) || ( type != REG_SZ ) )
	{
		QString str = QString( "Error searching for %1" ).arg( _usbId );
		logCb( str.toLatin1().constData() );
		return false;
	}

	portStr = QString::fromLatin1( portName.c_str() );

#ifdef _DEBUG
	assert( portStr.startsWith( "COM" ) );
	bool ok = false;
	portStr.right( portStr.length() - 3 ).toInt( &ok );
	assert( ok );
#endif

#else
	QDir dir( "/dev/serial/by-id/" );
	QStringList entries = dir.entryList();
	Q_FOREACH( const QString& str, entries )
	{
		if( str.contains( _usbName ) )
		{
			portStr = "/dev/serial/by-id/" + str;
			break;
		}
	}
	if( portStr.isEmpty() )
	{
		return false;
	}
	//portStr = QString( "/dev/robotino3io" );
#endif

	return open( portStr.toLatin1().constData() );
}

bool Com::open( const char* deviceName )
{
	close();

	if( verbosity() > 0 )
	{
		QString str = QString( "Connecting to %1" ).arg( deviceName );
		logCb( str.toLatin1().constData() );
	}

	_serial = new rec::serialport::SerialPort;
	_producer = new rec::robotino3::serialio::Producer( _serial, this );
	_parser = new rec::robotino3::serialio::Parser( _serial, _decoder, this );

	rec::robotino3::serialio::TagList l;

	try
	{
		_serial->open( deviceName, 115200, 0 );
		*_producer << tag::GetHwVersion::create();
		_producer->transmit();
		l = _parser->parse();
	}
	catch( const rec::serialport::SerialPortException& e )
	{
		logCb( e.what() );
		close();
		return false;
	}

	tag::HwVersionPointer hwVersion = get_hw_version( l );
	if( hwVersion )
	{
		hardwareVersionCb( hwVersion->versionStr().toLatin1().constData() );
		logCb( hwVersion->print().toLatin1().constData() );
		return true;
	}
	else
	{
		logCb( "Error reading hardware version" );
		//close();
		return true;
	}
}

void Com::close()
{
	if( isOpen() )
	{
		if( verbosity() > 0 )
		{
			logCb( "Closing connection" );
		}
	}

	if( _serial )
	{
		_serial->close();
	}

	delete _serial;
	_serial = NULL;

	delete _producer;
	_producer = NULL;

	delete _parser;
	_parser = NULL;
}

void Com::setVerbosity( int verbosity )
{
	_verbosity = verbosity;
}

int Com::verbosity() const
{
	return _verbosity;
}

void Com::setReadTimeout( unsigned int timeout )
{
	if(_serial)
	{
		_serial->setReadTimeout(timeout);
	}
}

int Com::parse()
{
	if( NULL == _parser ) return 0;

	try
	{
		rec::robotino3::serialio::TagList l =_parser->parse();
		Q_FOREACH( rec::robotino3::serialio::TagPointer p, l )
		{
			switch( p->value() )
			{
			case TAG_HW_VERSION:
				{
					rec::robotino3::serialio::tag::HwVersionPointer p2 = p.staticCast<rec::robotino3::serialio::tag::HwVersion>();
					hardwareVersionCb( p2->versionStr().toLatin1().constData() );
				}
				break;

			case TAG_SW_VERSION:
				{
					rec::robotino3::serialio::tag::SwVersionPointer p2 = p.staticCast<rec::robotino3::serialio::tag::SwVersion>();
					softwareVersionCb( p2->versionStr().toLatin1().constData() );
				}
				break;

			default:
				this->parse_i( p );
				break;
			}
		}

		return l.count();
	}
	catch( const rec::robotino3::serialio::TagException& e )
	{
		logCb( e.what() );
		return 0;
	}
}

void Com::produce( const TagHelper& p )
{
	if( _producer )
	{
		if( verbosity() > 1 )
		{
			QString str( "Producing " );
			str += p._p->print();
			logCb( str.toLatin1().constData() );
		}

		*_producer << p._p;
	}
}

void Com::getHardwareVersion() { produce( TagHelper( tag::GetHwVersion::create() ) ); }
void Com::getSoftwareVersion() { produce( TagHelper( tag::GetSwVersion::create() ) ); }

void Com::transmit()
{
	if( _producer )
	{
		try
		{
			_producer->transmit();
		}
		catch( const rec::serialport::SerialPortException& e )
		{
			logCb( e.what() );
			close();
		}
	}
}

void Com::clearTXBuffer()
{
	if( _producer )
	{
		_producer->clearTXBuffer();
	}
}

void Com::hardwareVersionCb( const char* version ){}
void Com::softwareVersionCb( const char* version ){}
void Com::logCb( const char* message ){}

int Com::parserDelegateVerbosity() const
{
	return _verbosity;
}

void Com::parserDelegateLog( const QString& message )
{
	logCb( message.toLatin1().constData() );
}

int Com::producerDelegateVerbosity() const
{
	return _verbosity;
}

void Com::producerDelegateLog( const QString& message )
{
	logCb( message.toLatin1().constData() );
}
