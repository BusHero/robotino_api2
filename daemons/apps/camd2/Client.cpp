#include "Client.h"
#include "rec/robotino/daemons/Log.h"

#include "CameraTest.h"

#ifdef WIN32
#include "win32/Camera_win32.h"
#else
#include "usb_cam/Camera_linux.h"
#endif

#include <iostream>

Client::Client( const QString& device, int channel, const QString& name, bool isTest )
: _timer( new QTimer( this ) )
, _restartTimer( new QTimer( this ) )
, _checkDeviceTimer( new QTimer( this ) )
, _camera( NULL )
, _image_width( 320 )
, _image_height( 240 )
, _image_format( "mjpg" )
, _device( device )
, _channel( channel )
, _cameraName( name )
, _isTest( isTest )
{
	setAutoReconnectEnabled( true );

	_timer->setSingleShot( false );
	_timer->setInterval( 20 );

	_restartTimer->setSingleShot( false );
	_restartTimer->setInterval( 1000 );

	if( false == _isTest )
	{
		_checkDeviceTimer->setSingleShot( false );
		_checkDeviceTimer->setInterval( 500 );
		_checkDeviceTimer->start();
	}

	bool connected = true;
	connected &= (bool)connect( _timer, SIGNAL( timeout() ), SLOT( on_timer_timeout() ) );
	connected &= (bool)connect( _restartTimer, SIGNAL( timeout() ), SLOT( on_restartTimer_timeout() ) );
	connected &= (bool)connect( _checkDeviceTimer, SIGNAL( timeout() ), SLOT( on_checkDeviceTimer_timeout() ) );

	connected &= (bool)connect( this, SIGNAL( is_connected() ), SLOT( on_is_connected() ) );
	connected &= (bool)connect( this, SIGNAL( is_disconnected() ), SLOT( on_is_disconnected() ) );

	switch( _channel )
	{
	case 0:
		connected &= (bool)connect( this, SIGNAL( set_camera0_settings_changed( unsigned int, unsigned int, const QString& ) ),
			SLOT( on_set_camera_settings_changed( unsigned int, unsigned int, QString ) ) );
		Q_ASSERT( connected );

		connected &= (bool)connect( this, SIGNAL( set_camera0_control_changed( const QString&, int ) ),
			SLOT( on_set_camera_control_changed( const QString&, int ) ) );
		Q_ASSERT( connected );

		connected &= (bool)connect( this, SIGNAL( image0_info_changed( const rec::rpc::ClientInfoSet& ) ),
			SLOT( on_image_info_changed( const rec::rpc::ClientInfoSet& ) ) );
		Q_ASSERT( connected );
		break;

	case 1:
		connected &= (bool)connect( this, SIGNAL( set_camera1_settings_changed( unsigned int, unsigned int, const QString& ) ),
			SLOT( on_set_camera_settings_changed( unsigned int, unsigned int, QString ) ) );
		Q_ASSERT( connected );

		connected &= (bool)connect( this, SIGNAL( set_camera1_control_changed( const QString&, int ) ),
			SLOT( on_set_camera_control_changed( const QString&, int ) ) );
		Q_ASSERT( connected );

		connected &= (bool)connect( this, SIGNAL( image1_info_changed( const rec::rpc::ClientInfoSet& ) ),
			SLOT( on_image_info_changed( const rec::rpc::ClientInfoSet& ) ) );
		Q_ASSERT( connected );
		break;

	case 2:
		connected &= (bool)connect( this, SIGNAL( set_camera2_settings_changed( unsigned int, unsigned int, const QString& ) ),
			SLOT( on_set_camera_settings_changed( unsigned int, unsigned int, QString ) ) );
		Q_ASSERT( connected );

		connected &= (bool)connect( this, SIGNAL( set_camera2_control_changed( const QString&, int ) ),
			SLOT( on_set_camera_control_changed( const QString&, int ) ) );
		Q_ASSERT( connected );

		connected &= (bool)connect( this, SIGNAL( image2_info_changed( const rec::rpc::ClientInfoSet& ) ),
			SLOT( on_image_info_changed( const rec::rpc::ClientInfoSet& ) ) );
		Q_ASSERT( connected );
		break;

	case 3:
		connected &= (bool)connect( this, SIGNAL( set_camera3_settings_changed( unsigned int, unsigned int, const QString& ) ),
			SLOT( on_set_camera_settings_changed( unsigned int, unsigned int, QString ) ) );
		Q_ASSERT( connected );

		connected &= (bool)connect( this, SIGNAL( set_camera3_control_changed( const QString&, int ) ),
			SLOT( on_set_camera_control_changed( const QString&, int ) ) );
		Q_ASSERT( connected );

		connected &= (bool)connect( this, SIGNAL( image3_info_changed( const rec::rpc::ClientInfoSet& ) ),
			SLOT( on_image_info_changed( const rec::rpc::ClientInfoSet& ) ) );
		Q_ASSERT( connected );
		break;

	default:
		rec::robotino::daemons::Log::singleton()->log( QString( "unsupported channel %1" ).arg( _channel ), 0 );
		return;
	}

	set_rec_robotino_rpc_set_log_level_enabled( true );
}

Client::~Client()
{
	delete _camera;
	rec::robotino::daemons::Log::singleton()->log( "camd2 stopped", 0 );
}

void Client::on_timer_timeout()
{
	if( NULL == _camera )
	{
		return;
	}

	unsigned int width;
	unsigned int height;
	unsigned int bytes_per_pixel;
	QString format;

	QByteArray ba;
	
	try
	{
		ba = _camera->grab( &width, &height, &bytes_per_pixel, &format );
	}
	catch( const CameraException& e )
	{
		rec::robotino::daemons::Log::singleton()->log( e.what(), 0 );
		restart();
		return;
	}

	if( ba.isEmpty() )
	{
		rec::robotino::daemons::Log::singleton()->log( "grab failed", 0 );
	}
	else
	{
		//rec::robotino::daemons::Log::singleton()->log( QString( "grabbed %1 bytes, width=%2, height=%3, numChannles=%4" ).arg( ba.size() ).arg( width ).arg( height ).arg( numChannels ) );
		rec::robotino::daemons::Log::singleton()->log( ".", 1 );

		set_image( _channel, ba, width, height, width * bytes_per_pixel, format );
	}
}

void Client::on_restartTimer_timeout()
{
	try
	{
		if( NULL == _camera )
		{
			if( _isTest )
			{
				_camera = new CameraTest;
			}
			else
			{
#ifdef WIN32
				_camera = new Camera_Win32;
#else
				_camera = new Camera_Linux;
#endif
			}

			bool connected = true;
			connected &= (bool)connect( _camera, SIGNAL( log( const QString&, int) ), SIGNAL( log( const QString&, int) ) );
			Q_ASSERT( connected );
		}

		_camera->setVideoDevice( _device );
		_camera->setResolution( _image_width, _image_height );
		_camera->setFormat( _image_format );

		QSize s = QSize( _image_width, _image_height );
		if( _calibrations.contains( s ) )
		{
			set_camera_calibration( _channel, _calibrations[ s ] );
		}
		else
		{
			set_camera_calibration( _channel, QVector<double>() );
		}

		QString setupSectionName = "~/" + _cameraName + "_Setup";
		double freq = 50.0f;

		if( "raw" == _image_format )
		{
			freq = getParameter( setupSectionName + "/rawfreq", 10 ).toDouble();
		}
		else
		{
			freq = getParameter( setupSectionName + "/freq", 50 ).toDouble();
		}

		if( freq <= 0.0 )
		{
			freq = 50.0;
		}

		rec::robotino::daemons::Log::singleton()->log( QString( "Setting grab frequency to %1 Hz" ).arg( freq ), 1 );
		_timer->setInterval( static_cast<int>( 1000.0 / freq ) );

		_camera->start();
		_restartTimer->stop();
		_timer->start();

		set_camera_settings( _channel, _image_width, _image_height, _image_format );
	}
	catch( const CameraException& e )
	{
		rec::robotino::daemons::Log::singleton()->log( e.what(), 0 );
		delete _camera;
		_camera = NULL;
	}
}

void Client::on_checkDeviceTimer_timeout()
{
	QFile file( _device );
	if( false == file.exists() )
	{
		rec::robotino::daemons::Log::singleton()->log( QString("%1 removed").arg( _device ), 0 );
		qApp->quit();
	}
}

void Client::on_is_connected()
{
	QString image_topic_name = QString("image%1_info").arg( _channel );
	bool ok = setTopicEnabled( image_topic_name, true );
	if( false == ok )
	{
		rec::robotino::daemons::Log::singleton()->log( QString("Failed to enable topic: ").arg( image_topic_name ), 0 );
		qApp->quit();
	}

	QString set_camera_settings_topic_name = QString("set_camera%1_settings").arg( _channel );
	ok = setTopicEnabled( set_camera_settings_topic_name, true );
	if( false == ok )
	{
		rec::robotino::daemons::Log::singleton()->log( QString("Failed to enable topic: ").arg( set_camera_settings_topic_name ), 0 );
		qApp->quit();
	}

	QString set_camera_control_topic_name = QString("set_camera%1_control").arg( _channel );
	ok = setTopicEnabled( set_camera_control_topic_name, true );
	if( false == ok )
	{
		rec::robotino::daemons::Log::singleton()->log( QString("Failed to enable topic: ").arg( set_camera_control_topic_name ), 0 );
		qApp->quit();
	}

	rec::robotino::daemons::Log::singleton()->log( "Parameters", 0 );
	QMap< QString, QVariant > p = getParameters();
	QMap< QString, QVariant >::const_iterator iter = p.constBegin();
	while( p.constEnd() != iter )
	{
		rec::robotino::daemons::Log::singleton()->log( QString("%1: %2").arg( iter.key() ).arg( iter.value().toString() ), 1 );
		++iter;
	}

	QString capabilitiesSectionName = _cameraName + "_Capabilities";

	QString mjpgResolutions = getLocalParameter( capabilitiesSectionName + "/mjpg" ).toString();
	QString rawResolutions = getLocalParameter( capabilitiesSectionName + "/raw" ).toString();

	_capabilities["mjpg"] = parseResolutions( mjpgResolutions );
	_capabilities["raw"] = parseResolutions( rawResolutions );
	QMap< QString, QVector< QSize > >::const_iterator capiter = _capabilities.constBegin();
	while( _capabilities.constEnd() != capiter )
	{
		const QVector< QSize >& sizes = capiter.value();
		for( int i=0; i<sizes.size(); ++i )
		{
			rec::robotino::daemons::Log::singleton()->log( QString("%1: %2x%3").arg( capiter.key() ).arg( sizes[i].width() ).arg( sizes[i].height() ), 1 );
		}
		++capiter;
	}

	QStringList controls;

	if( getLocalParameter( capabilitiesSectionName + "/brightness", false ).toBool() )
	{
		controls << "brightness";
	}
	if( getLocalParameter( capabilitiesSectionName + "/contrast", false ).toBool() )
	{
		controls << "contrast";
	}
	if( getLocalParameter( capabilitiesSectionName + "/saturation", false ).toBool() )
	{
		controls << "saturation";
	}
	if( getLocalParameter( capabilitiesSectionName + "/autoWhiteBalance", false ).toBool() )
	{
		controls << "autoWhiteBalance";
	}
	if( getLocalParameter( capabilitiesSectionName + "/whiteBalanceTemperature", false ).toBool() )
	{
		controls << "whiteBalanceTemperature";
	}
	if( getLocalParameter( capabilitiesSectionName + "/gain", false ).toBool() )
	{
		controls << "gain";
	}
	if( getLocalParameter( capabilitiesSectionName + "/backlightCompensation", false ).toBool() )
	{
		controls << "backlightCompensation";
	}
	if( getLocalParameter( capabilitiesSectionName + "/autoExposure", false ).toBool() )
	{
		controls << "autoExposure";
	}
	if( getLocalParameter( capabilitiesSectionName + "/exposure", false ).toBool() )
	{
		controls << "exposure";
	}
	if( getLocalParameter( capabilitiesSectionName + "/autofocus", false ).toBool() )
	{
		controls << "autofocus";
	}
	if( getLocalParameter( capabilitiesSectionName + "/focus", false ).toBool() )
	{
		controls << "focus";
	}
	if( getLocalParameter( capabilitiesSectionName + "/sharpness", false ).toBool() )
	{
		controls << "sharpness";
	}

	set_camera_capabilities( _channel, _cameraName, _capabilities, controls );

	parseCalibrations( capabilitiesSectionName );
	

	QString setupSectionName = _cameraName + "_Setup";
	_image_width = getLocalParameter( setupSectionName + "/width", 320 ).toUInt();
	_image_height = getLocalParameter( setupSectionName + "/height", 240 ).toUInt();
	_image_format = getLocalParameter( setupSectionName + "/format", "mjpg" ).toString();
	
	QString fixedResoltionStr = getLocalParameter( "~/fixedResolution" ).toString();
	QVector<QSize> fixedResolutionSizes = parseResolutions(fixedResoltionStr);
	if(1 == fixedResolutionSizes.count())
	{
		_fixedResolution = fixedResolutionSizes[0];
		_image_width = _fixedResolution.width();
		_image_height = _fixedResolution.height();
		rec::robotino::daemons::Log::singleton()->log( QString("Using fixed width:%1 height:%2 ").arg( _image_width ).arg( _image_height ), 0 );
	}
	
	_fixedFormat = getLocalParameter( "~/fixedFormat" ).toString();
	if(false == _fixedFormat.isEmpty())
	{
		rec::robotino::daemons::Log::singleton()->log( QString("Using fixed format:%1 ").arg( _fixedFormat ), 0 );
	}
}

QVector<QSize> Client::parseResolutions( const QString& resStr ) const
{
	QStringList list = resStr.split( "," );
	QVector<QSize> sizes;
	Q_FOREACH( const QString& str, list )
	{
		QStringList l = str.split( "x" );
		if( 2 != l.size() )
		{
			continue;
		}
		bool ok;
		unsigned int width = l.at(0).toUInt( &ok );
		if( false == ok )
		{
			continue;
		}
		unsigned int height = l.at(1).toUInt( &ok );
		if( false == ok )
		{
			continue;
		}

		sizes << QSize( width, height );
	}
	return sizes;
}

void Client::parseCalibrations( const QString& capabilitiesSectionName )
{
	const QMap< QString, QVariant >& params = getLocalParameters();
	QMap< QString, QVariant >::const_iterator iter = params.constBegin();
	while( params.constEnd() != iter )
	{
		const QString& key = iter.key();
		if( key.startsWith( capabilitiesSectionName + "/cal" ) )
		{
			QString resStr = key;
			resStr.remove( capabilitiesSectionName + "/cal" );
			QStringList l = resStr.split( "x" );
			if( 2 == l.size() )
			{
				bool ok;
				int w = l.first().toInt( &ok );
				if( !ok )
				{
					rec::robotino::daemons::Log::singleton()->log( QString("%1 invalid").arg( key ), 0 );
					return;
				}

				int h = l.last().toInt( &ok );
				if( !ok )
				{
					rec::robotino::daemons::Log::singleton()->log( QString("%1 invalid").arg( key ), 0 );
					return;
				}

				QStringList calDataList = iter.value().toString().split( "," );
				QVector< double > calData;
				QString dataLogStr;
				Q_FOREACH( const QString& str, calDataList )
				{
					bool ok;
					double d = str.toDouble( &ok );
					if( ok )
					{
						calData << d;
						dataLogStr += QString::number( d ) + " ";
					}
					else
					{
						rec::robotino::daemons::Log::singleton()->log( QString("%1 contains invalid value %2").arg( key ).arg( str ), 0 );
						return;
					}
				}

				_calibrations[ QSize( w, h ) ] = calData;

				rec::robotino::daemons::Log::singleton()->log( QString("Success parsing calibration %1\nwidth=%2\nheight=%3\ndata=%4").arg( key ).arg( w ).arg( h ).arg( dataLogStr ), 1 );
			}
		}

		++iter;
	}
}

void Client::on_is_disconnected()
{
	stop();
}

void Client::on_set_camera_settings_changed( unsigned int width, unsigned int height, QString format )
{
	if(false == _fixedResolution.isEmpty())
	{
		width = _fixedResolution.width();
		height = _fixedResolution.height();
	}
	
	if(false == _fixedFormat.isEmpty())
	{
		format = _fixedFormat;
	}
	
	if( _image_width == width && _image_height == height && format == _image_format )
	{
		return;
	}

	if( format != "mjpg"
		&& format != "mjpeg"
		&& format != "raw" )
	{
		rec::robotino::daemons::Log::singleton()->log( QString( "Unknown format %1" ).arg( format ), 0 );
		return;
	}

	QString myFormat = format;
	if( format == "mjpeg" )
	{
		myFormat = "mjpg";
	}

	if( false == _capabilities.contains( myFormat ) )
	{
		rec::robotino::daemons::Log::singleton()->log( QString( "Format %1 not supported" ).arg( format ), 0 );
		return;
	}

	const QVector< QSize >& sizes = _capabilities[ myFormat ];

	if( false == sizes.contains( QSize( width, height ) ) )
	{
		rec::robotino::daemons::Log::singleton()->log( QString( "Settings %1x%2 %3 not supported" ).arg( width ).arg( height ).arg( myFormat ), 0 );
		return;
	}

	rec::robotino::daemons::Log::singleton()->log( QString( "New settings %1x%2 %3" ).arg( width ).arg( height ).arg( myFormat ), 1 );

	_image_width = width;
	_image_height = height;
	_image_format = format;

	restart();
}

void Client::on_set_camera_control_changed( const QString& name, int value )
{
	rec::robotino::daemons::Log::singleton()->log( QString( "Control received %1 %2" ).arg( name ).arg( value ), 1 );
	if( _camera )
	{
		if( "brightness" == name )
		{
			_camera->setBrightness( value );
		}
		else if( "contrast" == name )
		{
			_camera->setContrast( value );
		}
		else if( "saturation" == name )
		{
			_camera->setSaturation( value );
		}
		else if( "autowhitebalance" == name )
		{
			_camera->setAutoWhiteBalanceEnabled( value > 0 ? true : false );
		}
		else if( "gain" == name )
		{
			_camera->setGain( value );
		}
		else if( "whitebalancetemperature" == name )
		{
			_camera->setWhiteBalanceTemperature( value );
		}
		else if( "backlightcompensation" == name )
		{
			_camera->setBacklightCompensation( value );
		}
		else if( "autoexposure" == name )
		{
			_camera->setAutoExposureEnabled( value > 0 ? true : false );
		}
		else if( "exposure" == name )
		{
			_camera->setExposure( value );
		}
		else if( "autofocus" == name )
		{
			_camera->setAutoFocusEnabled( value > 0 ? true : false );
		}
		else if( "focus" == name )
		{
			_camera->setFocus( value );
		}
		else if( "sharpness" == name )
		{
			_camera->setSharpness( value );
		}
		else
		{
			rec::robotino::daemons::Log::singleton()->log( "unsopported control", 1 );
		}
	}
}

void Client::on_image_info_changed( const rec::rpc::ClientInfoSet& info )
{
	if( 0 == info.size() )
	{
		stop();
	}
	else
	{
		start();
	}
}

void Client::start()
{
	rec::robotino::daemons::Log::singleton()->log( "Start", 1 );
	if( NULL == _camera )
	{
		restart();
	}
}

void Client::restart()
{
	rec::robotino::daemons::Log::singleton()->log( "Restart", 1 );
	stop();
	_restartTimer->start();
}

void Client::stop()
{
	rec::robotino::daemons::Log::singleton()->log( "Stop", 1 );
	delete _camera;
	_camera = NULL;
	_timer->stop();
	_restartTimer->stop();
}
