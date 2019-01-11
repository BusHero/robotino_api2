#include "rec/nstar2/Com.h"
#include "rec/nstar2/Producer.h"
#include "rec/nstar2/Parser.h"
#include "rec/nstar2/utils.h"
#include "rec/serialport/SerialPort.h"
#include "rec/nstar2/tag/Status.h"
#include "rec/nstar2/tag/MeasSpotPosition.h"
#include "rec/nstar2/tag/HwVersion.h"

using namespace rec::nstar2;

Com::Com()
: _serial( NULL )
, _producer( NULL )
, _parser( NULL )
, _verbosity( 0 )
{
	_version = new char[1];
	_version[0] = '\0';
}

Com::~Com()
{
	delete [] _version;
	close();
}

void Com::setVerbosity( int verbosity )
{
	_verbosity = verbosity;
}

bool Com::open( const char* device )
{
	close();

	_serial = new rec::serialport::SerialPort;
	_producer = new Producer( _serial, this );
	_parser = new Parser( _serial, this );

	QList<quint32> speeds;
	speeds << 115200 << 57600 << 19200 << 115200;

	Q_FOREACH( quint32 baudrate, speeds )
	{
		_serial->close();
		try
		{
			_serial->open( device, baudrate, 200 );

			while( 1 )
			{
				_producer->send_startup();
				tag::TagList l = _parser->parse();

				if( l.isEmpty() )
				{
					break;
				}

				tag::HwVersionPointer ver = get_hw_version( l );
				if( ver )
				{
					QByteArray verba = ver->version().toLatin1();
					delete [] _version;
					_version = new char[ verba.size() ];
					memcpy( _version, verba.constData(), verba.size() );
					log( QString( "Connected to NStar Version %1 at %2 with speed %3" ).arg( ver->version() ).arg( device ).arg( baudrate ).toLatin1().constData() );
					return true;
				}
			}
		}
		catch( const rec::serialport::SerialPortException& e )
		{
			log( QString( "Error: %1" ).arg( e.what() ).toLatin1().constData() );
			close();
			return false;
		}
	}

	log( QString( "Could't find sensor at: %1" ).arg( device ).toLatin1().constData() );
	return false;
}

void Com::close()
{
	delete _producer;
	_producer = NULL;

	delete _parser;
	_parser = NULL;

	delete _serial;
	_serial = NULL;
}

const char* Com::version() const
{
	return _version;
}

unsigned int Com::speed() const
{
	if( _serial )
	{
		return _serial->speed();
	}
	else
	{
		return 0;
	}
}

bool Com::setAutomaticReportEnabled( bool enable )
{
	if( _producer )
	{
		try
		{
			int errorCounter = 0;
			while( errorCounter < 4 )
			{
				_producer->send_set_automatic_report( enable );
	
				tag::TagList l = _parser->parse();

				if( l.isEmpty() )
				{
					return false;
				}

				tag::StatusPointer status = get_status( l );
				if( status )
				{
					if( 0 == status->status() )
					{
						return true;
					}
					else
					{
						log( QString( "Failed to set automatic reports: %1" ).arg( status->status() ).toLatin1().constData() );
						++errorCounter;
					}
				}
			}
		}
		catch( const rec::serialport::SerialPortException& e )
		{
			log( QString( "Error: %1" ).arg( e.what() ).toLatin1().constData() );
			close();
			return false;
		}
	}

	log( "Error setting automatic report" );
	return false;
}

bool Com::getSensorInfo()
{
	if( _producer )
	{
		try
		{
			for( ;; )
			{
				_producer->send_get_info();
	
				tag::TagList l = _parser->parse();

				if( false == l.isEmpty() )
				{
					return true;
				}
			}
		}
		catch( const rec::serialport::SerialPortException& e )
		{
			log( QString( "Error: %1" ).arg( e.what() ).toLatin1().constData() );
			close();
			return false;
		}
	}

	log( "Error getting info" );
	return false;
}

bool Com::setNodeIDs( const int* data, int size )
{
	if( 0 == size || size%2 != 0 )
	{
		log( "Set node ids invalid size" );
		return false;
	}

	if( _producer )
	{
		try
		{
			QList<int> nodeids;
			for( int i=0; i<size; ++i )
			{
				nodeids << data[i];
			}
			_producer->send_set_nodeids( nodeids );

			tag::TagList l = _parser->parse();

			if( false == l.isEmpty() )
			{
				return true;
			}
		}
		catch( const rec::serialport::SerialPortException& e )
		{
			log( QString( "Error: %1" ).arg( e.what() ).toLatin1().constData() );
			close();
			return false;
		}
	}

	log( "Error getting info" );
	return false;
}

bool Com::setBaudrate( int baudrate )
{
	if( _producer )
	{
		try
		{
			_producer->send_set_baudrate( baudrate );

			tag::TagList l = _parser->parse();

			tag::StatusPointer status = get_status( l );
			if( status )
			{
				if( 0 == status->status() )
				{
					return true;
				}
			}
		}
		catch( const rec::serialport::SerialPortException& e )
		{
			log( QString( "Error: %1" ).arg( e.what() ).toLatin1().constData() );
			close();
			return false;
		}
	}

	log( "Error setting baudrate" );
	return false;
}

bool Com::parse()
{
	if( _parser )
	{
		try
		{
			tag::TagList l = _parser->parse();

			if( l.isEmpty() )
			{
				return false;
			}

			tag::MeasSpotPositionPointer p = get_meas_spot_position( l );
			if( p )
			{
				tag::MeasSpotPosition::ReadingsContainer r = p->readings();
				QVector<int> ids( r.size() );
				QVector<float> xs( r.size() );
				QVector<float> ys( r.size() );
				QVector<int> mags( r.size() );

				for( int i=0; i<r.size(); ++i )
				{
					ids[i] = r[i].frequency;
					xs[i] = r[i].x;
					ys[i] = r[i].y;
					mags[i] = r[i].magnitude;
				}

				Report reportData( ids.constData(), xs.constData(), ys.constData(), mags.constData(), r.size() );
				report( reportData );
			}

			return true;
		}
		catch( const rec::serialport::SerialPortException& e )
		{
			log( QString( "Error: %1" ).arg( e.what() ).toLatin1().constData() );
			close();
			return false;
		}
	}

	return false;
}

void Com::log( const char* message ) const
{
}

void Com::report( const Report& report )
{
}

