#include "rec/nstar2/Producer.h"
#include "rec/nstar2/Com.h"
#include "rec/nstar2/utils.h"

#include "rec/serialport/SerialPort.h"

#include "rec/nstar2/tag/SetParams.h"
#include "rec/nstar2/tag/DateTime.h"
#include "rec/nstar2/tag/GetParams.h"
#include "rec/nstar2/tag/HwVersion.h"
#include "rec/nstar2/tag/MaxNumberSpots.h"
#include "rec/nstar2/tag/NumberReceptors.h"
#include "rec/nstar2/tag/ProtocolVersion.h"
#include "rec/nstar2/tag/ReportAuto.h"
#include "rec/nstar2/tag/SwVersion.h"
#include "rec/nstar2/tag/SerialNumber.h"
#include "rec/nstar2/tag/WaitNextMeas.h"
#include "rec/nstar2/tag/ReportPreScaler.h"
#include "rec/nstar2/tag/Range.h"
#include "rec/nstar2/tag/SamplePeriod.h"
#include "rec/nstar2/tag/NumberSpots.h"
#include "rec/nstar2/tag/SpotId.h"
#include "rec/nstar2/tag/SerialBaudrate.h"

#define HEAD (const char)0xaa
#define DATA_ESCAPE (const char)0x55
#define DATA_MANGLE (const char)0x20

using namespace rec::nstar2;
using namespace rec::nstar2::tag;

Producer::Producer( rec::serialport::SerialPort* serial, Com* com )
	: _serial( serial )
	, _com( com )
{
}

void Producer::send_startup()
{
	if( _com->verbosity() > 0 )
	{
		_com->log( "Sending init sensor" );
	}

	QByteArray ba = produce_startup();
	_serial->write( (const unsigned char*)ba.constData(), ba.size() );
}

void Producer::send_set_automatic_report( bool enable )
{
	if( _com->verbosity() > 0 )
	{
		if( enable )
		{
			_com->log( "Enabling automatic report" );
		}
		else
		{
			_com->log( "Disabling automatic report" );
		}
	}

	QByteArray ba = produce_set_automatic_report( enable );
	_serial->write( (const unsigned char*)ba.constData(), ba.size() );
}

void Producer::send_get_info()
{
	if( _com->verbosity() > 0 )
	{
		_com->log( "Get info" );
	}
	QByteArray ba = produce_get_info();
	_serial->write( (const unsigned char*)ba.constData(), ba.size() );
}

void Producer::send_set_nodeids( const QList<int>& nodeids )
{
	if( _com->verbosity() > 0 )
	{
		QString str( "Set nodeids: [ " );
		Q_FOREACH( int i, nodeids )
		{
			str += QString( "%1 " ).arg( i );
		}
		str += "]";
		_com->log( str.toLatin1().constData() );
	}
	QByteArray ba = produce_set_nodeids( nodeids );
	_serial->write( (const unsigned char*)ba.constData(), ba.size() );
}

void Producer::send_set_baudrate( int baudrate )
{
	if( _com->verbosity() > 0 )
	{
		_com->log( QString("set baudrate to %1").arg( baudrate ).toLatin1().constData() );
	}
	QByteArray ba = produce_set_baudrate( baudrate );
	_serial->write( (const unsigned char*)ba.constData(), ba.size() );
}

QByteArray Producer::produce( const tag::TagList& tagList ) const
{
	QByteArray unmangled;
	Q_FOREACH( TagPointer t, tagList )
	{
		unmangled += t->encode();
	}

	quint16 length = static_cast<quint16>( unmangled.size() );

	unmangled.prepend( length >> 8 );
	unmangled.prepend( 0xff & length );

	quint16 checksum = rec::nstar2::calculate_checksum( unmangled );

	unmangled.append( 0xff & checksum );
	unmangled.append( checksum >> 8 );

	QByteArray mangled;
	for( int i=0; i<unmangled.size(); ++i )
	{
		const QByteRef ch = unmangled[i];
		if( HEAD == ch || DATA_ESCAPE == ch )
		{
			mangled += DATA_ESCAPE;
			mangled += ( ch ^ DATA_MANGLE );
		}
		else
		{
			mangled += ch;
		}
	}

	mangled.prepend( HEAD );

	if( _com->verbosity() > 0 )
	{
		QString str;
		int j=0;
		while( j<mangled.size() )
		{
			str += ">>";
			for( int i=0; i<20 && j<mangled.size(); ++i )
			{
				str += QString( " %1" ).arg( quint16(0xFF) & (quint16)mangled[j], 2, 16, QChar( '0' ) );
				++j;
			}
			str += "\n";
		}
		_com->log( str.toLatin1().constData() );
	}

	return mangled;
}

QByteArray Producer::produce_startup() const
{
	TagList startup;
	startup << WaitNextMeas::create();

	GetParamsPointer getParams = GetParams::create();
	*getParams << HwVersion::create();
	*getParams << SwVersion::create();
	*getParams << ProtocolVersion::create();
	*getParams << DateTime::create();
	*getParams << NumberReceptors::create();
	*getParams << MaxNumberSpots::create();
	*getParams << SerialNumber::create();

	startup << getParams;

	return produce( startup );
};

QByteArray Producer::produce_set_automatic_report( bool enable ) const
{
	TagList l;

	SetParamsPointer setParams = SetParams::create();
	*setParams << ReportAuto::create( enable );

	l << setParams;

	return produce( l );
}

QByteArray Producer::produce_get_info() const
{
	TagList l;

	l << WaitNextMeas::create();

	GetParamsPointer getParams = GetParams::create();
	*getParams << HwVersion::create();
	*getParams << SwVersion::create();
	*getParams << ProtocolVersion::create();
	*getParams << DateTime::create();
	*getParams << NumberReceptors::create();
	*getParams << MaxNumberSpots::create();
	*getParams << SerialNumber::create();
	*getParams << ReportPreScaler::create();
	*getParams << Range::create();
	*getParams << SamplePeriod::create();
	*getParams << NumberSpots::create();
	*getParams << SpotId::create();

	l << getParams;

	return produce( l );
}

QByteArray Producer::produce_set_nodeids( const QList<int>& nodeids ) const
{
	TagList l;

	SetParamsPointer setParams = SetParams::create();
	*setParams << NumberSpots::create( nodeids.size() );
	*setParams << SpotId::create( nodeids );

	l << setParams;

	return produce( l );
}

QByteArray Producer::produce_set_baudrate( int baudrate ) const
{
	TagList l;

	SetParamsPointer setParams = SetParams::create();
	*setParams << SerialBaudrate::create( baudrate );

	l << setParams;

	return produce( l );
}
