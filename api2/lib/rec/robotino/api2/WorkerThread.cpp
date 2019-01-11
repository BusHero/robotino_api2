//Copyright (c) ...
//
//REC Robotics Equipment Corporation GmbH, Planegg, Germany. All rights reserved.
//Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//
//THIS SOFTWARE IS PROVIDED BY REC ROBOTICS EQUIPMENT CORPORATION GMBH �AS IS� AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL REC ROBOTICS EQUIPMENT CORPORATION GMBH
//BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
//GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
//LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//Copyright (c) ...
//
//REC Robotics Equipment Corporation GmbH, Planegg, Germany. Alle Rechte vorbehalten.
//Weiterverbreitung und Verwendung in nichtkompilierter oder kompilierter Form, mit oder ohne Ver�nderung, sind unter den folgenden Bedingungen zul�ssig:
//1) Weiterverbreitete nichtkompilierte Exemplare m�ssen das obige Copyright, diese Liste der Bedingungen und den folgenden Haftungsausschluss im Quelltext enthalten.
//2) Weiterverbreitete kompilierte Exemplare m�ssen das obige Copyright, diese Liste der Bedingungen und den folgenden Haftungsausschluss in der Dokumentation und/oder anderen Materialien, die mit dem Exemplar verbreitet werden, enthalten.
//
//DIESE SOFTWARE WIRD VON REC ROBOTICS EQUIPMENT CORPORATION GMBH OHNE JEGLICHE SPEZIELLE ODER IMPLIZIERTE GARANTIEN ZUR VERF�GUNG GESTELLT, DIE UNTER
//ANDEREM EINSCHLIESSEN: DIE IMPLIZIERTE GARANTIE DER VERWENDBARKEIT DER SOFTWARE F�R EINEN BESTIMMTEN ZWECK. AUF KEINEN FALL IST REC ROBOTICS EQUIPMENT CORPORATION GMBH
//F�R IRGENDWELCHE DIREKTEN, INDIREKTEN, ZUF�LLIGEN, SPEZIELLEN, BEISPIELHAFTEN ODER FOLGESCH�DEN (UNTER ANDEREM VERSCHAFFEN VON ERSATZG�TERN ODER -DIENSTLEISTUNGEN;
//EINSCHR�NKUNG DER NUTZUNGSF�HIGKEIT; VERLUST VON NUTZUNGSF�HIGKEIT; DATEN; PROFIT ODER GESCH�FTSUNTERBRECHUNG), WIE AUCH IMMER VERURSACHT UND UNTER WELCHER VERPFLICHTUNG
//AUCH IMMER, OB IN VERTRAG, STRIKTER VERPFLICHTUNG ODER UNERLAUBTER HANDLUNG (INKLUSIVE FAHRL�SSIGKEIT) VERANTWORTLICH, AUF WELCHEM WEG SIE AUCH IMMER DURCH DIE BENUTZUNG
//DIESER SOFTWARE ENTSTANDEN SIND, SOGAR, WENN SIE AUF DIE M�GLICHKEIT EINES SOLCHEN SCHADENS HINGEWIESEN WORDEN SIND.

#include "rec/robotino/api2/WorkerThread.hpp"
#include "rec/robotino/api2/Client.hpp"
#include "rec/robotino/api2/Com.h"
#include "rec/robotino/api2/RobotinoException.h"

#include "rec/robotino/api2/MotorReader.hpp"
#include "rec/robotino/api2/MotorDebugReader.hpp"
#include "rec/robotino/api2/ParameterReader.hpp"
#include "rec/robotino/api2/BumperReader.hpp"
#include "rec/robotino/api2/AnalogInputReader.hpp"
#include "rec/robotino/api2/DigitalInputReader.hpp"
#include "rec/robotino/api2/DistanceSensorsReader.hpp"
#include "rec/robotino/api2/PowerManagementReader.hpp"
#include "rec/robotino/api2/CameraReader.hpp"
#include "rec/robotino/api2/CompactBHAReader.hpp"
#include "rec/robotino/api2/ElectricalGripperReader.hpp"
#include "rec/robotino/api2/GrapplerReader.hpp"
#include "rec/robotino/api2/LaserRangeFinderReader.hpp"
#include "rec/robotino/api2/NorthStarReader.hpp"
#include "rec/robotino/api2/OdometryReader.hpp"
#include "rec/robotino/api2/PoseReader.hpp"
#include "rec/robotino/api2/KinectReader.hpp"
#include "rec/robotino/api2/ChargerReader.hpp"
#include "rec/robotino/api2/PowerButtonReader.hpp"
#include "rec/robotino/api2/CustomMessageReader.hpp"
#include "rec/robotino/api2/GyroscopeReader.hpp"
#include "rec/robotino/api2/GyroscopeExtReader.hpp"
#include "rec/robotino/api2/Factory4Reader.hpp"
#include "rec/robotino/api2/DigitalOutputReaderReader.hpp"

#include "rec/robotino/api2/comevent/ConnectedEvent.h"
#include "rec/robotino/api2/comevent/DisconnectedEvent.h"
#include "rec/robotino/api2/comevent/ErrorEvent.h"
#include "rec/robotino/api2/comevent/LogEvent.h"

#define JSON_TOPIC_SLOT(TOPICNAME,READERCLASS) \
	void WorkerThread::on_##TOPICNAME##_changed(const QByteArray& jsonData) \
	{ \
		QMutexLocker lk(&_mutex); \
		++_num_##TOPICNAME##_events; \
		Q_FOREACH(READERCLASS* r, _##READERCLASS##s) \
		{ \
			r->set_##TOPICNAME(jsonData); \
		} \
	}

#define JSON_WITH_DATA_TOPIC_SLOT(TOPICNAME,READERCLASS) \
	void WorkerThread::on_##TOPICNAME##_changed(const QByteArray& jsonData, const QByteArray& data) \
	{ \
	QMutexLocker lk(&_mutex); \
	++_num_##TOPICNAME##_events; \
	Q_FOREACH(READERCLASS* r, _##READERCLASS##s) \
		{ \
		r->set_##TOPICNAME(jsonData,data); \
		} \
	}

#define GENERIC_PROCESS(TOPICNAME,READERCLASS) \
	if (_num_##TOPICNAME##_events > 0) \
	{ \
		QMutexLocker lk(&_mutex); \
		Q_FOREACH(READERCLASS* r, _##READERCLASS##s) \
		{ \
			r->process_##TOPICNAME##_event(); \
		} \
		_num_##TOPICNAME##_events = 0; \
	}

#define JSON_TOPIC_CONNECTION(TOPICNAME) \
	connected &= (bool)connect(_client \
	, SIGNAL(TOPICNAME##_changed(const QByteArray&)) \
	, SLOT(on_##TOPICNAME##_changed(const QByteArray&)), Qt::DirectConnection); \
	Q_ASSERT(connected);

#define JSON_WITH_DATA_TOPIC_CONNECTION(TOPICNAME) \
	connected &= (bool)connect(_client \
	, SIGNAL(TOPICNAME##_changed(const QByteArray&, const QByteArray&)) \
	, SLOT(on_##TOPICNAME##_changed(const QByteArray&, const QByteArray&)), Qt::DirectConnection); \
	Q_ASSERT(connected);

#define STRING_SIGNAL_CONNECTION(TOPICNAME)\
	connected &= (bool)connect(this \
	, SIGNAL(set_##TOPICNAME##_signal(const QString&))\
	, _client \
	, SLOT(set_##TOPICNAME(const QString&)), Qt::QueuedConnection); \
	Q_ASSERT(connected);

#define JSON_SIGNAL_CONNECTION(TOPICNAME)\
	connected &= (bool)connect(this \
	, SIGNAL(set_##TOPICNAME##_signal(const QByteArray&))\
	, _client \
	, SLOT(set_##TOPICNAME(const QByteArray&)), Qt::QueuedConnection); \
	Q_ASSERT(connected);

#define JSON_WITH_DATA_SIGNAL_CONNECTION(TOPICNAME)\
	connected &= (bool)connect(this \
	, SIGNAL(set_##TOPICNAME##_signal(const QByteArray&, const QByteArray&))\
	, _client \
	, SLOT(set_##TOPICNAME(const QByteArray&, const QByteArray&)), Qt::QueuedConnection); \
	Q_ASSERT(connected);

using namespace rec::robotino::api2;

#ifdef QT_VERSION_4
Q_DECLARE_METATYPE( QAbstractSocket::SocketState )
#endif //QT_VERSION_4

Q_DECLARE_METATYPE( QVector< rec::robotino::rpc::GrapplerServoInfo > )
Q_DECLARE_METATYPE( QVector< double > )
Q_DECLARE_METATYPE( QVector< bool > )

QMap<ComId, WorkerThread* >* WorkerThread::_instances = NULL;
QMutex* WorkerThread::_instancesMutex = NULL;

void WorkerThread::shutdown()
{
    if (_instancesMutex) {
        QMutexLocker lk( _instancesMutex );
        Q_FOREACH( WorkerThread* p, _instances->values() )
        {
            p->exit();
            p->wait();
        }
    }
    
    delete _instancesMutex;
    _instancesMutex = NULL;
    
    delete _instances;
    _instances = NULL;
}

WorkerThread* WorkerThread::instance( const ComId& comID )
{
    if (_instancesMutex) {
        QMutexLocker lk( _instancesMutex );
        
        WorkerThread* p = _instances->value( comID, NULL );
        
        if( NULL == p )
        {
            throw RobotinoException( "Invalid comID" );
        }
        else
        {
            return p;
        }
    }
    else {
        throw RobotinoException( "Invalid comID" );
    }
}

void WorkerThread::initQt()
{
	if( NULL == QCoreApplication::instance() )
	{
		static int argc = 1;
		static char name[] = "rec::robotino::api2";
		static char* argv[1] = { name };
		new QCoreApplication( argc, argv );
	}

	qRegisterMetaType<QAbstractSocket::SocketState>();
	qRegisterMetaType< QVector< rec::robotino::rpc::GrapplerServoInfo> >();
	qRegisterMetaType< QVector< double > >();
	qRegisterMetaType< QVector< bool > >();
}

WorkerThread::WorkerThread( Com* com )
: comid( ComId::g_id++ )
, _com( com )
, _numBumperEvents( 0 )
, _numAnalogInputEvents( 0 )
, _numDigitalInputEvents( 0 )
, _numDistanceSensorsEvents( 0 )
, _numBatteryEvents( 0 )
, _numMotorReadingsEvent( 0 )
, _numMotorDebugEvents(0)
, _numParameterEvent( 0 )
, _numCompactBHAReadingsEvent( 0 )
, _numElectricalGrippersEvent( 0 )
, _numGrapplerStorePositionsEvent( 0 )
, _numGrapplerReadingsEvent( 0 )
, _numGrapplerServosEvent( 0 )
, _numGrapplerToggleTorqueEvent( 0 )
, _numNorthStarEvents( 0 )
, _numOdometryEvents(0)
, _numPoseEvents(0)
, _numPowerButtonEvents( 0 )
, _numCustomMessageEvents( 0 )
, _numGyroscopeEvents(0)
, _numGyroscopeExtEvents(0)
, _num_smartlicense_events(0)
, _num_smartdockingvis_events(0)
, _numDigitalOutputReaderEvents(0)

/*Factory 4*/
, _num_mclayout_events(0)
, _num_mcstatus_events(0)
, _num_pathnetwork_events(0)
, _num_localizationMode_events(0)
, _num_smartlog_events(0)
, _num_festool_charger_info_events(0)
, _num_smartnavigationplan_events(0)
, _num_smartlocations_events(0)
, _num_smartstations_events(0)
, _num_smartrobotinfo_events(0)
, _num_smartmyrobotid_events(0)
, _num_smartjoblist_events(0)
, _num_mapDir_events(0)
, _num_mapDirWithData_events(0)
, _num_fleetcom_response_events(0)
, _num_mapDir_response_events(0)
, _num_map_events(0)
, _num_mapPlanner_events(0)
, _num_smartNavigationPlanner_events(0)
, _num_smartNavigationPlanner_response_events(0)
, _num_smartNavigationCostMap_events(0)
, _num_smartCdl_events(0)
, _num_customVis_events(0)
, _num_smartPersonTracker_events(0)
, _odometrySequence(0)
{
	for( int i=0; i<numCameras; ++i )
	{
		_numImageEvent[i] = 0;
		_numCameraCapabilitiesEvent[i] = 0;
		_numCameraSettingsEvent[i] = 0;
		_numCameraCalibrationsEvent[i] = 0;
	}

	for( int i=0; i<numRangefinders; ++i )
	{
		_numScanEvents[i] = 0;
	}

	for( int i=0; i<numKinects; ++i )
	{
		_numDepthEvents[i] = 0;
		_numVideoEvents[i] = 0;
		_numTiltEvents[i] = 0;
		_numAccelEvents[i] = 0;
	}

	for( int i=0; i<numChargers; ++i )
	{
		_numChargerInfoEvents[i] = 0;
		_numChargerVersionEvents[i] = 0;
		_numChargerErrorEvents[i] = 0;
	}

    if (NULL == _instancesMutex) {
        _instancesMutex = new QMutex;
        _instances = new QMap< ComId, WorkerThread* >;
    }
    
	QMutexLocker lk( _instancesMutex );
	(*_instances)[ comid ] = this;

	start();
	_startSemaphore.acquire();

	setName( QString::null );
}

WorkerThread::~WorkerThread()
{
    if (_instancesMutex) {
        QMutexLocker lk( _instancesMutex );
        _instances->remove( comid );
    }

	exit();
	wait();
}

void WorkerThread::connectToServer( bool isBlocking )
{
	if ( isBlocking )
	{
		if ( false == _client->connectToServerWrapper( isBlocking ) )
		{
			throw rec::robotino::api2::RobotinoException( "The connection has been refused." );
		}
	}
	else
	{
		_client->connectToServerWrapper( isBlocking );
	}
}

void WorkerThread::disconnectFromServer()
{
	_client->disconnectFromServerWrapper();
}

bool WorkerThread::isConnected() const
{
	return _client->isConnected();
}

void WorkerThread::setAutoReconnectEnabled( bool enable )
{
	_client->setAutoReconnectEnabledWrapper( enable );
}

void WorkerThread::setAddress( const QString& address )
{
	_client->setAddress( address );
}

const char* WorkerThread::address() const
{
	return _client->address();
}

bool WorkerThread::isLocalConnection() const
{
	QString str = _client->rec::robotino::rpc::Client::address();
	str = str.split( ":" ).first();

	QHostAddress addr( str );

	if( QHostAddress( QHostAddress::LocalHost ) == addr )
	{
		return true;
	}
	
	if( QNetworkInterface::allAddresses().contains( addr ) )
	{
		return true;
	}

	return false;
}

void WorkerThread::setLocalIPCEnabled( bool enabled )
{
	_client->setLocalIPCEnabledWrapper( enabled );
}

void WorkerThread::setName( const QString& name )
{
	if( name.isEmpty() )
	{
		_client->setNameWrapper( QString( "rec::robotino::api2 %1 - " ).arg( comid ) + QTime::currentTime().toString( "hh:mm:ss.zzz" ) );
	}
	else
	{
		_client->setNameWrapper( name );
	}
}

void WorkerThread::setMultiThreadedSerializationEnabled( bool enabled )
{
	_client->setMultiThreadedSerializationEnabledWrapper( enabled );
}

unsigned int WorkerThread::elapsed() const
{
	return _client->elapsed();
}

void WorkerThread::processEvents()
{
	processComEvents();

	if( _numMotorReadingsEvent > 0 )
	{
		QMutexLocker lk( &_mutex );
		Q_FOREACH( MotorReader* m, _motorReaders )
		{
			m->processMotorEvent();
		}
		_numMotorReadingsEvent = 0;
	}

	if (_numMotorDebugEvents > 0)
	{
		QMutexLocker lk(&_mutex);
		Q_FOREACH(MotorDebugReader* m, _motorDebugReaders)
		{
			m->processMotorDebugEvent();
		}
		_numMotorDebugEvents = 0;
	}

	if( _numParameterEvent > 0 )
	{
		QMutexLocker lk( &_mutex );
		Q_FOREACH( ParameterReader* m, _parameterReaders )
		{
			m->processParametersEvent();
		}
		_numParameterEvent = 0;
	}

	for( unsigned int i=0; i<numCameras; ++i )
	{
		processCameraEvents( i );
	}

	for( unsigned int i=0; i<numKinects; ++i )
	{
		processKinectEvents( i );
	}

	for( unsigned int i=0; i<numRangefinders; ++i )
	{
		processScanEvents( i );
	}

	processBumperEvents();
	processAnalogInputEvents();
	processDigitalInputEvents();
	processDistanceSensorsEvents();
	processPowerManagementEvents();

	
	processChargerEvents();
	
	if( _numCompactBHAReadingsEvent > 0 )
	{
		QMutexLocker lk( &_mutex );
		Q_FOREACH( CompactBHAReader* r, _compactBHAReaders )
		{
			r->processReadingsEvent();
		}
		_numCompactBHAReadingsEvent = 0;
	}

	if( _numElectricalGrippersEvent > 0 )
	{
		QMutexLocker lk( &_mutex );
		Q_FOREACH( ElectricalGripperReader* r, _electricalGripperReaders )
		{
			r->processEvent();
		}
		_numElectricalGrippersEvent = 0;
	}

	processGrapplerEvents();

	if( _numNorthStarEvents > 0 )
	{
		QMutexLocker lk( &_mutex );
		Q_FOREACH( NorthStarReader* r, _northStarReaders )
		{
			r->processNorthStarEvent();
		}
		_numNorthStarEvents = 0;
	}

	if( _numOdometryEvents > 0 )
	{
		QMutexLocker lk( &_mutex );
		Q_FOREACH( OdometryReader* r, _odometryReaders )
		{
			r->processOdometryEvent();
		}
		_numOdometryEvents = 0;
	}

	if (_numPoseEvents > 0)
	{
		QMutexLocker lk(&_mutex);
		Q_FOREACH(PoseReader* r, _poseReaders)
		{
			r->processPoseEvent();
		}
		_numPoseEvents = 0;
	}

	processPowerButtonEvents();
	processCustomMessageEvents();
	processGyroscopeEvents();
	processGyroscopeExtEvents();
	processSmartDockingVisEvents();
	processFactory4Events();
	processSmartNavigationPlannerEvents();
	processSmartCdlEvents();
	processCustomVisEvents();
	processSmartPersonTrackerEvents();
	processSmartLogEvents();
	processFestoolChargerEvents();

	if (_numDigitalOutputReaderEvents > 0)
	{
		QMutexLocker lk(&_mutex);
		Q_FOREACH(DigitalOutputReaderReader* r, _DigitalOutputReaderReaders)
		{
			r->processDigitalOutputReaderEvent();
		}
		_numDigitalOutputReaderEvents = 0;
	}
}

void WorkerThread::processComEvents()
{
	QList<comevent::Event*> tmp;

	{
		QMutexLocker lk( &_mutex );
		tmp = _events;
		_events.clear();
	}

	Q_FOREACH( comevent::Event* e, tmp )
	{
		switch( e->type )
		{
		case comevent::ConnectedEventId:
			_com->connectedEvent();
			break;

		case comevent::DisconnectedEventId:
			_com->connectionClosedEvent();
			break;

		case comevent::ErrorEventId:
			{
				comevent::ErrorEvent* ev = static_cast<comevent::ErrorEvent*>( e );
				std::string errorStr = ev->message.toLatin1().constData();
				_com->errorEvent( errorStr.c_str() );
			}
			break;

		case comevent::LogEventId:
			{
				comevent::LogEvent* ev = static_cast<comevent::LogEvent*>( e );
				std::string message = ev->message.toLatin1().constData();
				_com->logEvent( message.c_str(), ev->level );
			}
			break;
		}

		delete e;
	}
}

void WorkerThread::processGrapplerEvents()
{
	if( _numGrapplerStorePositionsEvent > 0 )
	{
		QMutexLocker lk( &_mutex );
		Q_FOREACH( GrapplerReader* r, _grapplerReaders )
		{
			r->processGrapplerStorePositionsEvent();
		}
		_numGrapplerStorePositionsEvent = 0;
	}

	if( _numGrapplerReadingsEvent > 0 )
	{
		QMutexLocker lk( &_mutex );
		Q_FOREACH( GrapplerReader* r, _grapplerReaders )
		{
			r->processGrapplerReadingsEvent();
		}
		_numGrapplerReadingsEvent = 0;
	}

	if( _numGrapplerServosEvent > 0 )
	{
		QMutexLocker lk( &_mutex );
		Q_FOREACH( GrapplerReader* r, _grapplerReaders )
		{
			r->processGrapplerServosEvent();
		}
		_numGrapplerServosEvent = 0;
	}

	if( _numGrapplerToggleTorqueEvent > 0 )
	{
		QMutexLocker lk( &_mutex );
		Q_FOREACH( GrapplerReader* r, _grapplerReaders )
		{
			r->processGrapplerToggleTorqueEvent();
		}
		_numGrapplerToggleTorqueEvent = 0;
	}
}

void WorkerThread::processCameraEvents( unsigned int number )
{
	Q_ASSERT( number < numCameras );

	if( _numImageEvent[number] > 0 )
	{
		QMutexLocker lk( &_mutex );
		Q_FOREACH( CameraReader* m, _cameraReaders[number] )
		{
			m->processImageEvent();
		}
		_numImageEvent[number] = 0;
	}

	if( _numCameraCapabilitiesEvent[number] > 0 )
	{
		QMutexLocker lk( &_mutex );
		Q_FOREACH( CameraReader* m, _cameraReaders[number] )
		{
			m->processCapabilitiesEvent();
		}
		_numCameraCapabilitiesEvent[number] = 0;
	}

	if( _numCameraSettingsEvent[number] > 0 )
	{
		QMutexLocker lk( &_mutex );
		Q_FOREACH( CameraReader* m, _cameraReaders[number] )
		{
			m->processSettingsEvent();
		}
		_numCameraSettingsEvent[number] = 0;
	}

	if( _numCameraCalibrationsEvent[number] > 0 )
	{
		QMutexLocker lk( &_mutex );
		Q_FOREACH( CameraReader* m, _cameraReaders[number] )
		{
			m->processCalibrationEvent();
		}
		_numCameraCalibrationsEvent[number] = 0;
	}
}

void WorkerThread::processScanEvents( unsigned int number )
{
	Q_ASSERT( number < numRangefinders );

	if( _numScanEvents[number] > 0 )
	{
		QMutexLocker lk( &_mutex );
		Q_FOREACH( LaserRangeFinderReader* r, _laserRangeFinderReaders[number] )
		{
			r->processScanEvent();
		}
		_numScanEvents[number] = 0;
	}
}

void WorkerThread::processBumperEvents()
{
	QMutexLocker lk( &_mutex );
	if( _numBumperEvents > 0 )
	{
		Q_FOREACH( BumperReader* r, _bumperReaders )
		{
			r->processBumperEvent();
		}
		_numBumperEvents = 0;
	}
}

void WorkerThread::processAnalogInputEvents()
{
	QMutexLocker lk( &_mutex );
	if( _numAnalogInputEvents > 0 )
	{
		Q_FOREACH( AnalogInputReader* r, _analogInputReaders )
		{
			r->processAnalogInputEvent();
		}
		_numAnalogInputEvents = 0;
	}
}

void WorkerThread::processDigitalInputEvents()
{
	QMutexLocker lk( &_mutex );
	if( _numDigitalInputEvents > 0 )
	{
		Q_FOREACH( DigitalInputReader* r, _digitalInputReaders )
		{
			r->processDigitalInputEvent();
		}
		_numDigitalInputEvents = 0;
	}
}

void WorkerThread::processDistanceSensorsEvents()
{
	QMutexLocker lk( &_mutex );
	if( _numDistanceSensorsEvents > 0 )
	{
		Q_FOREACH( DistanceSensorsReader* r, _distanceSensorsReaders )
		{
			r->processDistanceSensorsEvent();
		}
		_numDistanceSensorsEvents = 0;
	}
}

void WorkerThread::processPowerManagementEvents()
{
	QMutexLocker lk( &_mutex );
	if( _numBatteryEvents > 0 )
	{
		Q_FOREACH( PowerManagementReader* r, _powerManagementReaders )
		{
			r->processPowerManagementEvent();
		}
		_numBatteryEvents = 0;
	}
}

void WorkerThread::processKinectEvents( unsigned int number )
{
	Q_ASSERT( number < numKinects );

	if( _numDepthEvents[number] > 0 )
	{
		QMutexLocker lk( &_mutex );
		Q_FOREACH( KinectReader* r, _kinectReaders[number] )
		{
			r->processDepthEvent();
		}
		_numDepthEvents[number] = 0;
	}
	if( _numVideoEvents[number] > 0 )
	{
		QMutexLocker lk( &_mutex );
		Q_FOREACH( KinectReader* r, _kinectReaders[number] )
		{
			r->processVideoEvent();
		}
		_numVideoEvents[number] = 0;
	}
	if( _numTiltEvents[number] > 0 )
	{
		QMutexLocker lk( &_mutex );
		Q_FOREACH( KinectReader* r, _kinectReaders[number] )
		{
			r->processTiltEvent();
		}
		_numTiltEvents[number] = 0;
	}
	if( _numAccelEvents[number] > 0 )
	{
		QMutexLocker lk( &_mutex );
		Q_FOREACH( KinectReader* r, _kinectReaders[number] )
		{
			r->processAccelEvent();
		}
		_numAccelEvents[number] = 0;
	}
}

void WorkerThread::processChargerEvents()
{
	for( unsigned int number=0; number<numChargers; ++number )
	{
		if( _numChargerInfoEvents[number] > 0 )
		{
			QMutexLocker lk( &_mutex );
			Q_FOREACH( ChargerReader* r, _chargerReaders[number] )
			{
				r->processChargerInfoEvent();
			}
			_numChargerInfoEvents[number] = 0;
		}

		if( _numChargerVersionEvents[number] > 0 )
		{
			QMutexLocker lk( &_mutex );
			Q_FOREACH( ChargerReader* r, _chargerReaders[number] )
			{
				r->processChargerVersionEvent();
			}
			_numChargerVersionEvents[number] = 0;
		}

		if( _numChargerErrorEvents[number] > 0 )
		{
			QMutexLocker lk( &_mutex );
			Q_FOREACH( ChargerReader* r, _chargerReaders[number] )
			{
				r->processChargerErrorEvent();
			}
			_numChargerErrorEvents[number] = 0;
		}
	}
}

void WorkerThread::processPowerButtonEvents()
{
	if( _numPowerButtonEvents > 0 )
	{
		QMutexLocker lk( &_mutex );
		Q_FOREACH( PowerButtonReader* r, _powerButtonReaders )
		{
			r->processPowerButtonEvent();
		}
		_numPowerButtonEvents = 0;
	}
}

void WorkerThread::processCustomMessageEvents()
{
	if( _numCustomMessageEvents > 0 )
	{
		QMutexLocker lk( &_mutex );
		Q_FOREACH( CustomMessageReader* r, _customMessageReaders )
		{
			r->processCustomMessageEvent();
		}
		_numCustomMessageEvents = 0;
	}
}

void WorkerThread::processGyroscopeEvents()
{
	if( _numGyroscopeEvents > 0 )
	{
		QMutexLocker lk( &_mutex );
		Q_FOREACH( GyroscopeReader* r, _gyroscopeReaders )
		{
			r->processGyroscopeEvent();
		}
		_numGyroscopeEvents = 0;
	}
}

void WorkerThread::processGyroscopeExtEvents()
{
	if (_numGyroscopeExtEvents > 0)
	{
		QMutexLocker lk(&_mutex);
		Q_FOREACH(GyroscopeExtReader* r, _gyroscopeExtReaders)
		{
			r->processGyroscopeExtEvent();
		}
		_numGyroscopeExtEvents = 0;
	}
}

void WorkerThread::processSmartDockingVisEvents()
{
	GENERIC_PROCESS(smartdockingvis, SmartDockingVisReader)
}

void WorkerThread::processFactory4Events()
{
	GENERIC_PROCESS(mclayout,Factory4Reader)
	GENERIC_PROCESS(mcstatus, Factory4Reader)
	GENERIC_PROCESS(pathnetwork, Factory4Reader)
	GENERIC_PROCESS(localizationMode, Factory4Reader)
	GENERIC_PROCESS(smartlog, Factory4Reader)
	GENERIC_PROCESS(smartnavigationplan, Factory4Reader)
	GENERIC_PROCESS(smartlocations, Factory4Reader)
	GENERIC_PROCESS(smartstations, Factory4Reader)
	GENERIC_PROCESS(smartrobotinfo, Factory4Reader)
	GENERIC_PROCESS(smartmyrobotid, Factory4Reader)
	GENERIC_PROCESS(smartjoblist, Factory4Reader)
	GENERIC_PROCESS(mapDir, Factory4Reader)
	GENERIC_PROCESS(mapDirWithData, Factory4Reader)
	GENERIC_PROCESS(fleetcom_response, Factory4Reader)
	GENERIC_PROCESS(mapDir_response, Factory4Reader)
	GENERIC_PROCESS(smartlicense, Factory4Reader)
	GENERIC_PROCESS(map, Factory4Reader)
	GENERIC_PROCESS(mapPlanner, Factory4Reader)	
}

void WorkerThread::processSmartNavigationPlannerEvents()
{
	GENERIC_PROCESS(smartNavigationPlanner, SmartNavigationPlannerReader)
	GENERIC_PROCESS(smartNavigationPlanner_response, SmartNavigationPlannerReader)
	GENERIC_PROCESS(smartNavigationCostMap, SmartNavigationPlannerReader)
}

void WorkerThread::processSmartCdlEvents()
{
	GENERIC_PROCESS(smartCdl, SmartCdlReader);
}

void WorkerThread::processCustomVisEvents()
{
	GENERIC_PROCESS(customVis, CustomVisReader);
}

void WorkerThread::processSmartPersonTrackerEvents()
{
	GENERIC_PROCESS(smartPersonTracker, SmartPersonTrackerReader);
}

void WorkerThread::processSmartLogEvents()
{
	GENERIC_PROCESS(smartLog, SmartLogReader);
}

void WorkerThread::processFestoolChargerEvents()
{
	GENERIC_PROCESS(festool_charger_info, FestoolChargerReader);
}

void WorkerThread::addMotorReader( MotorReader* m )
{
	QMutexLocker lk( &_mutex );
	if( _motorReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "motor_readings", true );
	}

	_motorReaders.append( m );
}

void WorkerThread::removeMotorReader( MotorReader* m )
{
	QMutexLocker lk( &_mutex );
	_motorReaders.removeAll( m );

	if( _motorReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "motor_readings", false );
	}
}

void WorkerThread::addMotorDebugReader(MotorDebugReader* m)
{
	QMutexLocker lk(&_mutex);
	if (_motorDebugReaders.isEmpty())
	{
		Q_EMIT setTopicEnabled_signal("motor_debug", true);
	}

	_motorDebugReaders.append(m);
}

void WorkerThread::removeMotorDebugReader(MotorDebugReader* m)
{
	QMutexLocker lk(&_mutex);
	_motorDebugReaders.removeAll(m);

	if (_motorDebugReaders.isEmpty())
	{
		Q_EMIT setTopicEnabled_signal("motor_debug", false);
	}
}

void WorkerThread::addParameterReader( ParameterReader* m )
{
	QMutexLocker lk( &_mutex );
	if( _parameterReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "parameters", true );
	}

	_parameterReaders.append( m );
}

void WorkerThread::removeParameterReader( ParameterReader* m )
{
	QMutexLocker lk( &_mutex );
	_parameterReaders.removeAll( m );

	if( _parameterReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "parameters", false );
	}
}

void WorkerThread::addBumperReader( BumperReader* r )
{
	QMutexLocker lk( &_mutex );
	if( _bumperReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "bumper", true );
	}

	_bumperReaders.append( r );
}

void WorkerThread::removeBumperReader( BumperReader* r )
{
	QMutexLocker lk( &_mutex );
	_bumperReaders.removeAll( r );

	if( _bumperReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "bumper", false );
	}
}

void WorkerThread::addAnalogInputReader( AnalogInputReader* r )
{
	QMutexLocker lk( &_mutex );
	if( _analogInputReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "analog_input", true );
	}

	_analogInputReaders.append( r );
}

void WorkerThread::removeAnalogInputReader( AnalogInputReader* r )
{
	QMutexLocker lk( &_mutex );
	_analogInputReaders.removeAll( r );

	if( _analogInputReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "analog_input", false );
	}
}

void WorkerThread::addDigitalInputReader( DigitalInputReader* r )
{
	QMutexLocker lk( &_mutex );
	if( _digitalInputReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "digital_input", true );
	}

	_digitalInputReaders.append( r );
}

void WorkerThread::removeDigitalInputReader( DigitalInputReader* r )
{
	QMutexLocker lk( &_mutex );
	_digitalInputReaders.removeAll( r );

	if( _digitalInputReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "digital_input", false );
	}
}

void WorkerThread::addDistanceSensorsReader( DistanceSensorsReader* r )
{
	QMutexLocker lk( &_mutex );
	if( _distanceSensorsReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "distance_sensors", true );
	}

	_distanceSensorsReaders.append( r );
}

void WorkerThread::removeDistanceSensorsReader( DistanceSensorsReader* r )
{
	QMutexLocker lk( &_mutex );
	_distanceSensorsReaders.removeAll( r );

	if( _distanceSensorsReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "distance_sensors", false );
	}
}

void WorkerThread::addPowerManagementReader( PowerManagementReader* r )
{
	QMutexLocker lk( &_mutex );
	if( _powerManagementReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "battery", true );
	}

	_powerManagementReaders.append( r );
}

void WorkerThread::removePowerManagementReader( PowerManagementReader* r )
{
	QMutexLocker lk( &_mutex );
	_powerManagementReaders.removeAll( r );

	if( _powerManagementReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "battery", false );
	}
}

void WorkerThread::addCameraReader( CameraReader* m, unsigned int number )
{
	QMutexLocker lk( &_mutex );
	_cameraReaders[number].append( m );
	if( 1 == _cameraReaders[number].size() )
	{
		QString imageTopic = QString( "image%1" ).arg( number );
		QString camera_settingsTopic = QString( "camera%1_settings" ).arg( number );
		QString camera_capabilitiesTopic = QString( "camera%1_capabilities" ).arg( number );
		QString camera_calibrationTopic = QString( "camera%1_calibration" ).arg( number );

		Q_EMIT setTopicEnabled_signal( imageTopic, true );
		Q_EMIT setTopicEnabled_signal( camera_settingsTopic, true );
		Q_EMIT setTopicEnabled_signal( camera_calibrationTopic, true );
		Q_EMIT setTopicEnabled_signal( camera_capabilitiesTopic, true );
	}
	else
	{
		CameraReader* first = _cameraReaders[number].first();
		first->clone( m );
		m->processCapabilitiesEvent();
		m->processCalibrationEvent();
		m->processSettingsEvent();
	}
}

void WorkerThread::removeCameraReader( CameraReader* m )
{
	QMutexLocker lk( &_mutex );

	for( int i=0; i<numCameras; ++i )
	{
		if( _cameraReaders[i].isEmpty() )
		{
			continue;
		}

		_cameraReaders[i].removeAll( m );

		if( _cameraReaders[i].isEmpty() )
		{
			QString imageTopic = QString( "image%1" ).arg( i );
			QString camera_settingsTopic = QString( "camera%1_settings" ).arg( i );
			QString camera_capabilitiesTopic = QString( "camera%1_capabilities" ).arg( i );
			QString camera_calibrationTopic = QString( "camera%1_calibration" ).arg( i );

			Q_EMIT setTopicEnabled_signal( imageTopic, false );
			Q_EMIT setTopicEnabled_signal( camera_settingsTopic, false );
			Q_EMIT setTopicEnabled_signal( camera_capabilitiesTopic, false );
			Q_EMIT setTopicEnabled_signal( camera_calibrationTopic, false );
		}
	}
}

void WorkerThread::addCompactBHAReader( CompactBHAReader* r )
{
	QMutexLocker lk( &_mutex );
	if( _compactBHAReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "cbha_readings", true );
	}

	_compactBHAReaders.append( r );
}

void WorkerThread::removeCompactBHAReader( CompactBHAReader* r )
{
	QMutexLocker lk( &_mutex );
	_compactBHAReaders.removeAll( r );

	if( _compactBHAReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "cbha_readings", false );
	}
}

void WorkerThread::addElectricalGripperReader( ElectricalGripperReader* r )
{
	QMutexLocker lk( &_mutex );
	if( _electricalGripperReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "gripper_state", true );
	}

	_electricalGripperReaders.append( r );
}

void WorkerThread::removeElectricalGripperReader( ElectricalGripperReader* r )
{
	QMutexLocker lk( &_mutex );
	_electricalGripperReaders.removeAll( r );

	if( _electricalGripperReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "gripper_state", false );
	}
}

void WorkerThread::addGrapplerReader( GrapplerReader* r )
{
	QMutexLocker lk( &_mutex );
	if( _grapplerReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "grappler_store_positions", true );
		Q_EMIT setTopicEnabled_signal( "grappler_servos", true );
		Q_EMIT setTopicEnabled_signal( "grappler_readings", true );
		Q_EMIT setTopicEnabled_signal( "grappler_toggle_torque", true );
	}

	_grapplerReaders.append( r );
}

void WorkerThread::removeGrapplerReader( GrapplerReader* r )
{
	QMutexLocker lk( &_mutex );
	_grapplerReaders.removeAll( r );

	if( _grapplerReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "grappler_store_positions", false );
		Q_EMIT setTopicEnabled_signal( "grappler_servos", false );
		Q_EMIT setTopicEnabled_signal( "grappler_readings", false );
		Q_EMIT setTopicEnabled_signal( "grappler_toggle_torque", false );
	}
}

void WorkerThread::addLaserRangeFinderReader( LaserRangeFinderReader* r, unsigned int number )
{
	QMutexLocker lk( &_mutex );
	_laserRangeFinderReaders[number].append( r );
	if( 1 == _laserRangeFinderReaders[number].size() )
	{
		Q_EMIT setTopicEnabled_signal( QString("scan%1").arg( number ), true );
	}
	else
	{
		LaserRangeFinderReader* first = _laserRangeFinderReaders[number].first();
		first->clone( r );
		r->processScanEvent();
	}
}

void WorkerThread::removeLaserRangeFinderReader( LaserRangeFinderReader* r )
{
	QMutexLocker lk( &_mutex );

	for( int i=0; i<numRangefinders; ++i )
	{
		if( _laserRangeFinderReaders[i].isEmpty() )
		{
			continue;
		}

		_laserRangeFinderReaders[i].removeAll( r );

		if( _laserRangeFinderReaders[i].isEmpty() )
		{
			Q_EMIT setTopicEnabled_signal( QString("scan%1").arg( i ), false );
		}
	}
}

void WorkerThread::addNorthStarReader( NorthStarReader* r )
{
	QMutexLocker lk( &_mutex );
	if( _northStarReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "northstar", true );
	}

	_northStarReaders.append( r );
}

void WorkerThread::removeNorthStarReader( NorthStarReader* r )
{
	QMutexLocker lk( &_mutex );
	_northStarReaders.removeAll( r );

	if( _northStarReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "northstar", false );
	}
}

void WorkerThread::addOdometryReader( OdometryReader* r )
{
	QMutexLocker lk( &_mutex );
	if( _odometryReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "odometry", true );
	}

	_odometryReaders.append( r );
}

void WorkerThread::removeOdometryReader( OdometryReader* r )
{
	QMutexLocker lk( &_mutex );
	_odometryReaders.removeAll( r );

	if( _odometryReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "odometry", false );
	}
}

void WorkerThread::addPoseReader(PoseReader* r)
{
	QMutexLocker lk(&_mutex);
	if (_poseReaders.isEmpty())
	{
		Q_EMIT setTopicEnabled_signal("pose", true);
	}

	_poseReaders.append(r);
}

void WorkerThread::removePoseReader(PoseReader* r)
{
	QMutexLocker lk(&_mutex);
	_poseReaders.removeAll(r);

	if (_poseReaders.isEmpty())
	{
		Q_EMIT setTopicEnabled_signal("pose", false);
	}
}

void WorkerThread::addKinectReader( KinectReader* r, unsigned int number )
{
	QMutexLocker lk( &_mutex );
	_kinectReaders[number].append( r );
	if( 1 == _kinectReaders[number].size() )
	{
		QString depthTopic = QString( "kinect%1_depth" ).arg( number );
		QString videoTopic = QString( "kinect%1_video" ).arg( number );
		QString tiltTopic = QString( "kinect%1_tilt" ).arg( number );
		QString accelTopic = QString( "kinect%1_accel" ).arg( number );

		Q_EMIT setTopicEnabled_signal( depthTopic, true );
		Q_EMIT setTopicEnabled_signal( videoTopic, true );
		Q_EMIT setTopicEnabled_signal( tiltTopic, true );
		Q_EMIT setTopicEnabled_signal( accelTopic, true );
	}
}

void WorkerThread::removeKinectReader( KinectReader* r )
{
	QMutexLocker lk( &_mutex );

	for( int i=0; i<numKinects; ++i )
	{
		if( _kinectReaders[i].isEmpty() )
		{
			continue;
		}

		_kinectReaders[i].removeAll( r );

		if( _kinectReaders[i].isEmpty() )
		{
			QString depthTopic = QString( "kinect%1_depth" ).arg( i );
			QString videoTopic = QString( "kinect%1_video" ).arg( i );
			QString tiltTopic = QString( "kinect%1_tilt" ).arg( i );
			QString accelTopic = QString( "kinect%1_accel" ).arg( i );

			Q_EMIT setTopicEnabled_signal( depthTopic, false );
			Q_EMIT setTopicEnabled_signal( videoTopic, false );
			Q_EMIT setTopicEnabled_signal( tiltTopic, false );
			Q_EMIT setTopicEnabled_signal( accelTopic, false );
		}
	}
}

void WorkerThread::addChargerReader( ChargerReader* r, unsigned int number )
{
	QMutexLocker lk( &_mutex );
	_chargerReaders[number].append( r );
	if( 1 == _chargerReaders[number].size() )
	{
		QString infoTopic = QString( "charger%1_info" ).arg( number );
		QString versionTopic = QString( "charger%1_version" ).arg( number );
		QString errorTopic = QString( "charger%1_error" ).arg( number );

		Q_EMIT setTopicEnabled_signal( infoTopic, true );
		Q_EMIT setTopicEnabled_signal( versionTopic, true );
		Q_EMIT setTopicEnabled_signal( errorTopic, true );
	}
}

void WorkerThread::removeChargerReader( ChargerReader* r )
{
	QMutexLocker lk( &_mutex );

	for( int i=0; i<numChargers; ++i )
	{
		if( _chargerReaders[i].isEmpty() )
		{
			continue;
		}

		_chargerReaders[i].removeAll( r );

		if( _chargerReaders[i].isEmpty() )
		{
			QString infoTopic = QString( "charger%1_info" ).arg( i );
			QString versionTopic = QString( "charger%1_version" ).arg( i );
			QString errorTopic = QString( "charger%1_error" ).arg( i );

			Q_EMIT setTopicEnabled_signal( infoTopic, false );
			Q_EMIT setTopicEnabled_signal( versionTopic, false );
			Q_EMIT setTopicEnabled_signal( errorTopic, false );
		}
	}
}

void WorkerThread::addPowerButtonReader( PowerButtonReader* r )
{
	QMutexLocker lk( &_mutex );
	if( _powerButtonReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "power_button", true );
	}
	_powerButtonReaders.append( r );
}

void WorkerThread::removePowerButtonReader( PowerButtonReader* r )
{
	QMutexLocker lk( &_mutex );
	_powerButtonReaders.removeAll( r );

	if( _powerButtonReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "power_button", false );
	}
}

void WorkerThread::addCustomMessageReader( CustomMessageReader* r )
{
	QMutexLocker lk( &_mutex );
	if( _customMessageReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "custom_message", true );
	}
	_customMessageReaders.append( r );
}

void WorkerThread::removeCustomMessageReader( CustomMessageReader* r )
{
	QMutexLocker lk( &_mutex );
	_customMessageReaders.removeAll( r );

	if( _customMessageReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "custom_message", false );
	}
}

void WorkerThread::addGyroscopeReader( GyroscopeReader* r )
{
	QMutexLocker lk( &_mutex );
	if( _gyroscopeReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "gyroscope", true );
	}
	_gyroscopeReaders.append( r );
}

void WorkerThread::removeGyroscopeReader( GyroscopeReader* r )
{
	QMutexLocker lk( &_mutex );
	_gyroscopeReaders.removeAll( r );

	if( _gyroscopeReaders.isEmpty() )
	{
		Q_EMIT setTopicEnabled_signal( "gyroscope", false );
	}
}

void WorkerThread::addGyroscopeExtReader(GyroscopeExtReader* r)
{
	QMutexLocker lk(&_mutex);
	if (_gyroscopeExtReaders.isEmpty())
	{
		Q_EMIT setTopicEnabled_signal("gyroscope_ext", true);
	}
	_gyroscopeExtReaders.append(r);
}

void WorkerThread::removeGyroscopeExtReader(GyroscopeExtReader* r)
{
	QMutexLocker lk(&_mutex);
	_gyroscopeExtReaders.removeAll(r);

	if (_gyroscopeExtReaders.isEmpty())
	{
		Q_EMIT setTopicEnabled_signal("gyroscope_ext", false);
	}
}

void WorkerThread::addSmartDockingVisReader(SmartDockingVisReader* r)
{
	QMutexLocker lk(&_mutex);
	if (_SmartDockingVisReaders.isEmpty())
	{
		Q_EMIT setTopicEnabled_signal("smartdockingvis", true);
	}
	_SmartDockingVisReaders.append(r);
}

void WorkerThread::removeSmartDockingVisReader(SmartDockingVisReader* r)
{
	QMutexLocker lk(&_mutex);
	_SmartDockingVisReaders.removeAll(r);

	if (_SmartDockingVisReaders.isEmpty())
	{
		Q_EMIT setTopicEnabled_signal("smartdockingvis", false);
	}
}

void WorkerThread::enableFactory4Topic(bool enable)
{
	Q_EMIT setTopicEnabled_signal("mclayout", enable);
	Q_EMIT setTopicEnabled_signal("mcstatus", enable);
	Q_EMIT setTopicEnabled_signal("pathnetwork", enable);
	Q_EMIT setTopicEnabled_signal("localizationMode", enable);
	Q_EMIT setTopicEnabled_signal("smartlog", enable);
	Q_EMIT setTopicEnabled_signal("smartnavigationplan", enable);
	Q_EMIT setTopicEnabled_signal("smartlocations", enable);
	Q_EMIT setTopicEnabled_signal("smartstations", enable);
	Q_EMIT setTopicEnabled_signal("smartrobotinfo", enable);
	Q_EMIT setTopicEnabled_signal("smartmyrobotid", enable);
	Q_EMIT setTopicEnabled_signal("smartjoblist", enable);
	Q_EMIT setTopicEnabled_signal("mapDir", enable);
	Q_EMIT setTopicEnabled_signal("mapDirWithData", enable);
	Q_EMIT setTopicEnabled_signal("fleetcom_response", enable);
	Q_EMIT setTopicEnabled_signal("mapDir_response", enable);
	Q_EMIT setTopicEnabled_signal("map", enable);
	Q_EMIT setTopicEnabled_signal("mapPlanner", enable);
}

void WorkerThread::addFactory4Reader(Factory4Reader* r)
{
	QMutexLocker lk(&_mutex);
	if (_Factory4Readers.isEmpty())
	{
		enableFactory4Topic(true);
	}
	_Factory4Readers.append(r);
}

void WorkerThread::removeFactory4Reader(Factory4Reader* r)
{
	QMutexLocker lk(&_mutex);
	_Factory4Readers.removeAll(r);

	if (_Factory4Readers.isEmpty())
	{
		enableFactory4Topic(false);
	}
}

void WorkerThread::addDigitalOutputReaderReader(DigitalOutputReaderReader* r)
{
	QMutexLocker lk(&_mutex);
	if (_DigitalOutputReaderReaders.isEmpty())
	{
		Q_EMIT setTopicEnabled_signal("set_digital_output", true);
		Q_EMIT setTopicEnabled_signal("set_digital_output_array", true);
	}
	_DigitalOutputReaderReaders.append(r);
}

void WorkerThread::removeDigitalOutputReaderReader(DigitalOutputReaderReader* r)
{
	QMutexLocker lk(&_mutex);
	_DigitalOutputReaderReaders.removeAll(r);

	if (_DigitalOutputReaderReaders.isEmpty())
	{
		Q_EMIT setTopicEnabled_signal("set_digital_output", false);
		Q_EMIT setTopicEnabled_signal("set_digital_output_array", false);
	}
}


void WorkerThread::addSmartNavigationPlannerReader(SmartNavigationPlannerReader* r)
{
	QMutexLocker lk(&_mutex);
	if (_SmartNavigationPlannerReaders.isEmpty())
	{
		Q_EMIT setJsonWithDataTopicEnabled_signal(rec::robotino::rpc::smartNavigationPlannner_topic, true);
		Q_EMIT setJsonWithDataTopicEnabled_signal(rec::robotino::rpc::smartNavigationPlanner_response_topic, true);
		Q_EMIT setJsonWithDataTopicEnabled_signal(rec::robotino::rpc::smartNavigationCostMap_topic, true);
	}
	_SmartNavigationPlannerReaders.append(r);
}

void WorkerThread::removeSmartNavigationPlannerReader(SmartNavigationPlannerReader* r)
{
	QMutexLocker lk(&_mutex);
	_SmartNavigationPlannerReaders.removeAll(r);

	if (_SmartNavigationPlannerReaders.isEmpty())
	{
		Q_EMIT setJsonWithDataTopicEnabled_signal(rec::robotino::rpc::smartNavigationPlannner_topic, false);
		Q_EMIT setJsonWithDataTopicEnabled_signal(rec::robotino::rpc::smartNavigationPlanner_response_topic, false);
		Q_EMIT setJsonWithDataTopicEnabled_signal(rec::robotino::rpc::smartNavigationCostMap_topic, false);
	}
}

void WorkerThread::addSmartLogReader(SmartLogReader* r)
{
	QMutexLocker lk(&_mutex);
	if (_SmartLogReaders.isEmpty())
	{
		Q_EMIT setJsonWithDataTopicEnabled_signal(rec::robotino::rpc::fileSystem_response_topic, true);
	}
	_SmartLogReaders.append(r);
}

void WorkerThread::removeSmartLogReader(SmartLogReader* r)
{
	QMutexLocker lk(&_mutex);
	_SmartLogReaders.removeAll(r);

	if (_SmartLogReaders.isEmpty())
	{
		Q_EMIT setJsonWithDataTopicEnabled_signal(rec::robotino::rpc::fileSystem_response_topic, false);
	}
}

void WorkerThread::addFestoolChargerReader(FestoolChargerReader* r)
{
	QMutexLocker lk(&_mutex);
	if (_FestoolChargerReaders.isEmpty())
	{
		Q_EMIT setTopicEnabled_signal("festool_charger_info",true);
	}
	_FestoolChargerReaders.append(r);
}

void WorkerThread::removeFestoolChargerReader(FestoolChargerReader* r)
{
	QMutexLocker lk(&_mutex);
	_FestoolChargerReaders.removeAll(r);

	if (_FestoolChargerReaders.isEmpty())
	{
		Q_EMIT setTopicEnabled_signal("festool_charger_info", false);
	}
}

void WorkerThread::addSmartCdlReader(SmartCdlReader* r)
{
	QMutexLocker lk(&_mutex);
	if (_SmartCdlReaders.isEmpty())
	{
		Q_EMIT setJsonWithDataTopicEnabled_signal(rec::robotino::rpc::smartCdl_topic, true);
	}
	_SmartCdlReaders.append(r);
}

void WorkerThread::removeSmartCdlReader(SmartCdlReader* r)
{
	QMutexLocker lk(&_mutex);
	_SmartCdlReaders.removeAll(r);

	if (_SmartCdlReaders.isEmpty())
	{
		Q_EMIT setJsonWithDataTopicEnabled_signal(rec::robotino::rpc::smartCdl_topic, false);
	}
}

void WorkerThread::addSmartPersonTrackerReader(SmartPersonTrackerReader* r)
{
	QMutexLocker lk(&_mutex);
	if (_SmartPersonTrackerReaders.isEmpty())
	{
		Q_EMIT setJsonWithDataTopicEnabled_signal(rec::robotino::rpc::smartPersonTracker_topic, true);
	}
	_SmartPersonTrackerReaders.append(r);
}

void WorkerThread::removeSmartPersonTrackerReader(SmartPersonTrackerReader* r)
{
	QMutexLocker lk(&_mutex);
	_SmartPersonTrackerReaders.removeAll(r);

	if (_SmartPersonTrackerReaders.isEmpty())
	{
		Q_EMIT setJsonWithDataTopicEnabled_signal(rec::robotino::rpc::smartPersonTracker_topic, false);
	}
}

void WorkerThread::addCustomVisReader(CustomVisReader* r)
{
	QMutexLocker lk(&_mutex);
	if (_CustomVisReaders.isEmpty())
	{
		Q_EMIT setJsonWithDataTopicEnabled_signal(rec::robotino::rpc::customVis_topic, true);
	}
	_CustomVisReaders.append(r);
}

void WorkerThread::removeCustomVisReader(CustomVisReader* r)
{
	QMutexLocker lk(&_mutex);
	_CustomVisReaders.removeAll(r);

	if (_CustomVisReaders.isEmpty())
	{
		Q_EMIT setJsonWithDataTopicEnabled_signal(rec::robotino::rpc::customVis_topic, false);
	}
}

void WorkerThread::set_omnidrive( float vx, float vy, float omega )
{
	Q_EMIT set_omnidrive_signal( vx, vy, omega );
}

void WorkerThread::set_motor0_setpoint( float speed )
{
	Q_EMIT set_motor0_setpoint_signal( speed );
}

void WorkerThread::motor0_reset_position( int position )
{
	Q_EMIT motor0_reset_position_signal( position );
}

void WorkerThread::set_set_motor0_mode( const QString& mode )
{
	Q_EMIT set_set_motor0_mode_signal( mode );
}

void WorkerThread::set_motor1_setpoint( float speed )
{
	Q_EMIT set_motor1_setpoint_signal( speed );
}

void WorkerThread::motor1_reset_position( int position )
{
	Q_EMIT motor1_reset_position_signal( position );
}

void WorkerThread::set_set_motor1_mode( const QString& mode )
{
	Q_EMIT set_set_motor1_mode_signal( mode );
}

void WorkerThread::set_motor2_setpoint( float speed )
{
	Q_EMIT set_motor2_setpoint_signal( speed );
}

void WorkerThread::motor2_reset_position( int position )
{
	Q_EMIT motor2_reset_position_signal( position );
}

void WorkerThread::set_set_motor2_mode( const QString& mode )
{
	Q_EMIT set_set_motor2_mode_signal( mode );
}

void WorkerThread::set_motor3_setpoint( float speed )
{
	Q_EMIT set_motor3_setpoint_signal( speed );
}

void WorkerThread::motor3_reset_position( int position )
{
	Q_EMIT motor3_reset_position_signal( position );
}

void WorkerThread::set_set_motor3_mode( const QString& mode )
{
	Q_EMIT set_set_motor3_mode_signal( mode );
}

void WorkerThread::set_motor_setpoints( float m0, float m1, float m2 )
{
	Q_EMIT set_motor_setpoints_signal( m0, m1, m2 );
}


void WorkerThread::set_set_pid_parameters( unsigned int motor, float kp, float ki, float kd )
{
	Q_EMIT set_set_pid_parameters_signal( motor, kp, ki, kd );
}

void WorkerThread::set_set_northstar_parameters( unsigned int roomId, float ceilingCal )
{
	Q_EMIT set_set_northstar_parameters_signal( roomId, ceilingCal );
}

bool WorkerThread::set_set_odometry( double x, double y, double phi, int timeout )
{
	if( 0 != timeout )
	{
		QMutexLocker lk( &_odometrySetMutex );
		Q_EMIT set_set_odometry_signal( x, y, phi );
		if(timeout < 0)
		{
			return _odometrySetCondition.wait( &_odometrySetMutex );
		}
		else
		{
			return _odometrySetCondition.wait( &_odometrySetMutex, timeout );
		}
	}
	else
	{
		Q_EMIT set_set_odometry_signal( x, y, phi );
		return true;
	}
}

void WorkerThread::set_kinect_set_tilt( unsigned int id, double tilt )
{
	Q_EMIT set_kinect_set_tilt_signal( id, tilt );
}

void WorkerThread::set_kinect_set_led( unsigned int id, unsigned int state )
{
	Q_EMIT set_kinect_set_led_signal( id, state );
}

void WorkerThread::set_kinect_set_depth_format( unsigned int id, unsigned int format )
{
	Q_EMIT set_kinect_set_depth_format_signal( id, format );
}

void WorkerThread::set_kinect_set_video_format( unsigned int id, unsigned int format )
{
	Q_EMIT set_kinect_set_video_format_signal( id, format );
}

void WorkerThread::set_cbha_set_pressure( const QVector<float>& pressures )
{
	Q_EMIT set_cbha_set_pressure_signal( pressures );
}

void WorkerThread::set_cbha_set_compressors_enabled( bool enabled )
{
	Q_EMIT set_cbha_set_compressors_enabled_signal( enabled );
}

void WorkerThread::set_cbha_set_water_drain_valve( bool open )
{
	Q_EMIT set_cbha_set_water_drain_valve_signal( open );
}

void WorkerThread::set_cbha_set_gripper_valve1( bool open )
{
	Q_EMIT set_cbha_set_gripper_valve1_signal( open );
}

void WorkerThread::set_cbha_set_gripper_valve2( bool open )
{
	Q_EMIT set_cbha_set_gripper_valve2_signal( open );
}

void WorkerThread::set_grappler_set_power( int line, bool power )
{
	Q_EMIT set_grappler_set_power_signal( line, power );
}

void WorkerThread::set_grappler_set_positions( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data )
{
	Q_EMIT set_grappler_set_positions_signal( data );
}

void WorkerThread::set_grappler_toggle_torque()
{
	Q_EMIT set_grappler_toggle_torque_signal();
}

void WorkerThread::set_set_camera_settings( unsigned int cameraNumber, unsigned int width, unsigned int height, const QString& format )
{
	Q_EMIT set_set_camera_settings_signal( cameraNumber, width, height, format );
}

void WorkerThread::set_set_camera_control( unsigned int cameraNumber, const QString& name, int value )
{
	Q_EMIT set_set_camera_control_signal( cameraNumber, name, value );
}

void WorkerThread::set_image( unsigned int cameraNumber,
									const QByteArray& data,
									unsigned int width,
									unsigned int height,
									unsigned int step,
									const QString& format )
{
	Q_EMIT set_image_signal( cameraNumber, data, width, height, step, format );
}

void WorkerThread::set_set_digital_output( unsigned int index, bool state )
{
	Q_EMIT set_set_digital_output_signal( index, state );
}

void WorkerThread::set_set_digital_output_array( const QVector< bool >& data )
{
	Q_EMIT set_set_digital_output_array_signal( data );
}

void WorkerThread::set_set_relay( unsigned int index, bool state )
{
	Q_EMIT set_set_relay_signal( index, state );
}

void WorkerThread::set_set_relay_array( const QVector< bool >& data )
{
	Q_EMIT set_set_relay_array_signal( data );
}

void WorkerThread::set_set_shutdown( bool shutdown )
{
	Q_EMIT set_set_shutdown_signal( shutdown );
}

void WorkerThread::set_custom_message( unsigned int id, const QByteArray& data )
{
	Q_EMIT set_custom_message_signal( id, data );
}

void WorkerThread::start_hotswap()
{
	Q_EMIT start_hotswap_signal();
}

void WorkerThread::start_motor_debug(int motor)
{
	Q_EMIT start_motor_debug_signal(motor);
}

void WorkerThread::set_upload_program(const QByteArray& infoData, const QByteArray& data)
{
	Q_EMIT set_upload_program_signal(infoData, data);
}

void WorkerThread::charger_clear_error( unsigned int number )
{
	switch( number )
	{
	case 0:
		Q_EMIT charger0_clear_error_signal();
		break;

	case 1:
		Q_EMIT charger1_clear_error_signal();
		break;

	case 2:
		Q_EMIT charger2_clear_error_signal();
		break;

	default:
		break;
	}
}

void WorkerThread::set_shutdown()
{
	Q_EMIT set_shutdown_signal( true );
}

/*Factory 4*******************************/

void WorkerThread::set_fleetcom_request(const QString& message)
{
	Q_EMIT set_fleetcom_request_signal(message);
}

void WorkerThread::set_mapDir_request(const QByteArray& infoData, const QByteArray& data)
{
	Q_EMIT set_mapDir_request_signal(infoData, data);
}

void WorkerThread::set_pathnetwork_edited(const QByteArray& pathnetwork)
{
	Q_EMIT set_pathnetwork_edited_signal(pathnetwork);
}

void WorkerThread::set_smartlocations_edited(const QByteArray& locations)
{
	Q_EMIT set_smartlocations_edited_signal(locations);
}

void WorkerThread::set_smartstations_edited(const QByteArray& stations)
{
	Q_EMIT set_smartstations_edited_signal(stations);
}

void WorkerThread::set_mapPlannerEdited(const QByteArray& image)
{
	Q_EMIT set_mapPlannerEdited_signal(image, 0, 0, 0, 0, 0);
}

void WorkerThread::set_json_message_with_data(int topic, const QByteArray& jsonData, const QByteArray& data)
{
	Q_EMIT set_json_message_with_data_signal(topic, jsonData, data);
}


void WorkerThread::on_log( const QString& message, int level )
{
	QMutexLocker lk( &_mutex );
	if( _events.size() > 1000 )
	{
		comevent::Event* e = _events.takeLast();
		delete e;
	}
	_events.append( new comevent::LogEvent( message, level ) );
}

void WorkerThread::on_is_connected()
{
	QMutexLocker lk( &_mutex );
	if( _events.size() > 1000 )
	{
		comevent::Event* e = _events.takeLast();
		delete e;
	}
	_events.append( new comevent::ConnectedEvent );
}

void WorkerThread::on_is_disconnected()
{
	QMutexLocker lk( &_mutex );
	if( _events.size() > 1000 )
	{
		comevent::Event* e = _events.takeLast();
		delete e;
	}
	_events.append( new comevent::DisconnectedEvent );
}

void WorkerThread::on_error( QAbstractSocket::SocketError socketError, const QString& errorString )
{
	QMutexLocker lk( &_mutex );
	if( _events.size() > 1000 )
	{
		comevent::Event* e = _events.takeLast();
		delete e;
	}
	_events.append( new comevent::ErrorEvent( errorString ) );
}

void WorkerThread::on_parameters_changed( const QMap< QString, QVariant >& values )
{
	QMutexLocker lk( &_mutex );
	++_numParameterEvent;
	Q_FOREACH( ParameterReader* m, _parameterReaders )
	{
		m->setParameters( values );
	}
}

void WorkerThread::on_motor_readings_changed( const QVector< float >& speeds, const QVector< int >& positions, const QVector< float >& currents, float time_delta )
{
	QMutexLocker lk( &_mutex );
	++_numMotorReadingsEvent;
	Q_FOREACH( MotorReader* r, _motorReaders )
	{
		r->setMotorReadings( speeds, positions, currents, time_delta );
	}
}

void WorkerThread::on_digital_input_changed( const QVector< bool >& data )
{
	QMutexLocker lk( &_mutex );
	++_numDigitalInputEvents;
	Q_FOREACH( DigitalInputReader* r, _digitalInputReaders )
	{
		r->setDigitalInput( data );
	}
}

void WorkerThread::on_analog_input_changed( const QVector< float >& data )
{
	QMutexLocker lk( &_mutex );
	++_numAnalogInputEvents;
	Q_FOREACH( AnalogInputReader* r, _analogInputReaders )
	{
		r->setAnalogInput( data );
	}
}

void WorkerThread::on_distance_sensors_changed( const QVector< float >& data )
{
	QMutexLocker lk( &_mutex );
	++_numDistanceSensorsEvents;
	Q_FOREACH( DistanceSensorsReader* r, _distanceSensorsReaders )
	{
		r->setDistanceSensors( data );
	}
}

void WorkerThread::on_bumper_changed( bool bump )
{
	QMutexLocker lk( &_mutex );
	++_numBumperEvents;
	Q_FOREACH( BumperReader* r, _bumperReaders )
	{
		r->setBumper( bump );
	}
}

void WorkerThread::on_battery_changed( float battery_voltage, float system_current, bool ext_power, int num_chargers, const QString& batteryType, bool batteryLow, int batteryLowShutdownCounter )
{
	QMutexLocker lk( &_mutex );
	++_numBatteryEvents;
	Q_FOREACH( PowerManagementReader* r, _powerManagementReaders )
	{
		r->setBattery( battery_voltage, system_current, ext_power, num_chargers, batteryType, batteryLow, batteryLowShutdownCounter );
	}
}

void WorkerThread::on_image0_changed( const QByteArray& data,
									unsigned int width,
									unsigned int height,
									unsigned int step,
									const QString& format )
{
	QMutexLocker lk( &_mutex );
	++_numImageEvent[0];
	Q_FOREACH( CameraReader* m, _cameraReaders[0] )
	{
		m->setImage( data, width, height, step, format );
	}
}

void WorkerThread::on_camera0_settings_changed( unsigned int width, unsigned int height, const QString& format )
{
	QMutexLocker lk( &_mutex );
	++_numCameraSettingsEvent[0];
	Q_FOREACH( CameraReader* m, _cameraReaders[0] )
	{
		m->setSettings( width, height, format );
	}
}

void WorkerThread::on_camera0_capabilities_changed( const QString& cameraName, const QMap<QString, QVector<QSize> >& capabilities, const QStringList& controls )
{
	QMutexLocker lk( &_mutex );
	++_numCameraCapabilitiesEvent[0];
	Q_FOREACH( CameraReader* m, _cameraReaders[0] )
	{
		m->setCapabilities( cameraName, capabilities, controls );
	}
}

void WorkerThread::on_camera0_calibration_changed( const QVector<double>& calibration )
{
	QMutexLocker lk( &_mutex );
	++_numCameraCalibrationsEvent[0];
	Q_FOREACH( CameraReader* m, _cameraReaders[0] )
	{
		m->setCalibration( calibration );
	}
}

void WorkerThread::on_image1_changed( const QByteArray& data,
									unsigned int width,
									unsigned int height,
									unsigned int step,
									const QString& format )
{
	QMutexLocker lk( &_mutex );
	++_numImageEvent[1];
	Q_FOREACH( CameraReader* m, _cameraReaders[1] )
	{
		m->setImage( data, width, height, step, format );
	}
}

void WorkerThread::on_camera1_settings_changed( unsigned int width, unsigned int height, const QString& format )
{
	QMutexLocker lk( &_mutex );
	++_numCameraSettingsEvent[1];
	Q_FOREACH( CameraReader* m, _cameraReaders[1] )
	{
		m->setSettings( width, height, format );
	}
}

void WorkerThread::on_camera1_capabilities_changed( const QString& cameraName, const QMap<QString, QVector<QSize> >& capabilities, const QStringList& controls )
{
	QMutexLocker lk( &_mutex );
	++_numCameraCapabilitiesEvent[1];
	Q_FOREACH( CameraReader* m, _cameraReaders[1] )
	{
		m->setCapabilities( cameraName, capabilities, controls );
	}
}

void WorkerThread::on_camera1_calibration_changed( const QVector<double>& calibration )
{
	QMutexLocker lk( &_mutex );
	++_numCameraCalibrationsEvent[1];
	Q_FOREACH( CameraReader* m, _cameraReaders[1] )
	{
		m->setCalibration( calibration );
	}
}

void WorkerThread::on_image2_changed( const QByteArray& data,
									unsigned int width,
									unsigned int height,
									unsigned int step,
									const QString& format )
{
	QMutexLocker lk( &_mutex );
	++_numImageEvent[2];
	Q_FOREACH( CameraReader* m, _cameraReaders[2] )
	{
		m->setImage( data, width, height, step, format );
	}
}

void WorkerThread::on_camera2_settings_changed( unsigned int width, unsigned int height, const QString& format )
{
	QMutexLocker lk( &_mutex );
	++_numCameraSettingsEvent[2];
	Q_FOREACH( CameraReader* m, _cameraReaders[2] )
	{
		m->setSettings( width, height, format );
	}
}

void WorkerThread::on_camera2_capabilities_changed( const QString& cameraName, const QMap<QString, QVector<QSize> >& capabilities, const QStringList& controls )
{
	QMutexLocker lk( &_mutex );
	++_numCameraCapabilitiesEvent[2];
	Q_FOREACH( CameraReader* m, _cameraReaders[2] )
	{
		m->setCapabilities( cameraName, capabilities, controls );
	}
}

void WorkerThread::on_camera2_calibration_changed( const QVector<double>& calibration )
{
	QMutexLocker lk( &_mutex );
	++_numCameraCalibrationsEvent[2];
	Q_FOREACH( CameraReader* m, _cameraReaders[2] )
	{
		m->setCalibration( calibration );
	}
}

void WorkerThread::on_image3_changed( const QByteArray& data,
									unsigned int width,
									unsigned int height,
									unsigned int step,
									const QString& format )
{
	QMutexLocker lk( &_mutex );
	++_numImageEvent[3];
	Q_FOREACH( CameraReader* m, _cameraReaders[3] )
	{
		m->setImage( data, width, height, step, format );
	}
}

void WorkerThread::on_camera3_settings_changed( unsigned int width, unsigned int height, const QString& format )
{
	QMutexLocker lk( &_mutex );
	++_numCameraSettingsEvent[3];
	Q_FOREACH( CameraReader* m, _cameraReaders[3] )
	{
		m->setSettings( width, height, format );
	}
}

void WorkerThread::on_camera3_capabilities_changed( const QString& cameraName, const QMap<QString, QVector<QSize> >& capabilities, const QStringList& controls )
{
	QMutexLocker lk( &_mutex );
	++_numCameraCapabilitiesEvent[3];
	Q_FOREACH( CameraReader* m, _cameraReaders[3] )
	{
		m->setCapabilities( cameraName, capabilities, controls );
	}
}

void WorkerThread::on_camera3_calibration_changed( const QVector<double>& calibration )
{
	QMutexLocker lk( &_mutex );
	++_numCameraCalibrationsEvent[3];
	Q_FOREACH( CameraReader* m, _cameraReaders[3] )
	{
		m->setCalibration( calibration );
	}
}

void WorkerThread::on_cbha_readings_changed( const QVector<float>& pressures, bool pressureSensor, const QVector<float>& stringPots, float foilPot )
{
	QMutexLocker lk( &_mutex );
	++_numCompactBHAReadingsEvent;
	Q_FOREACH( CompactBHAReader* r, _compactBHAReaders )
	{
		r->set( pressures, pressureSensor, stringPots, foilPot );
	}
}

void WorkerThread::on_gripper_state_changed( int state )
{
	QMutexLocker lk( &_mutex );
	++_numElectricalGrippersEvent;
	Q_FOREACH( ElectricalGripperReader* r, _electricalGripperReaders )
	{
		r->set( state );
	}
}

void WorkerThread::on_grappler_store_positions_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data )
{
	QMutexLocker lk( &_mutex );
	++_numGrapplerStorePositionsEvent;
	Q_FOREACH( GrapplerReader* r, _grapplerReaders )
	{
		r->setGrapplerStorePositions( data );
	}
}

void WorkerThread::on_grappler_servos_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data )
{
	QMutexLocker lk( &_mutex );
	++_numGrapplerServosEvent;
	Q_FOREACH( GrapplerReader* r, _grapplerReaders )
	{
		r->setGrapplerServos( data );
	}
}

void WorkerThread::on_grappler_readings_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data )
{
	QMutexLocker lk( &_mutex );
	++_numGrapplerReadingsEvent;
	Q_FOREACH( GrapplerReader* r, _grapplerReaders )
	{
		r->setGrapplerReadings( data );
	}
}

void WorkerThread::on_grappler_toggle_torque_changed()
{
	QMutexLocker lk( &_mutex );
	++_numGrapplerToggleTorqueEvent;
	Q_FOREACH( GrapplerReader* r, _grapplerReaders )
	{
		r->setGrapplerToggleTorque();
	}
}

void WorkerThread::on_scan0_changed( const QVector< float >& ranges, const QVector< float >& intensities, const QVariantMap& parameters )
{
	QMutexLocker lk( &_mutex );
	++_numScanEvents[0];
	Q_FOREACH( LaserRangeFinderReader* r, _laserRangeFinderReaders[0] )
	{
		r->setScan( ranges, intensities, parameters );
	}
}

void WorkerThread::on_scan1_changed( const QVector< float >& ranges, const QVector< float >& intensities, const QVariantMap& parameters )
{
	QMutexLocker lk( &_mutex );
	++_numScanEvents[1];
	Q_FOREACH( LaserRangeFinderReader* r, _laserRangeFinderReaders[1] )
	{
		r->setScan( ranges, intensities, parameters );
	}
}

void WorkerThread::on_scan2_changed( const QVector< float >& ranges, const QVector< float >& intensities, const QVariantMap& parameters )
{
	QMutexLocker lk( &_mutex );
	++_numScanEvents[2];
	Q_FOREACH( LaserRangeFinderReader* r, _laserRangeFinderReaders[2] )
	{
		r->setScan( ranges, intensities, parameters );
	}
}

void WorkerThread::on_scan3_changed( const QVector< float >& ranges, const QVector< float >& intensities, const QVariantMap& parameters )
{
	QMutexLocker lk( &_mutex );
	++_numScanEvents[3];
	Q_FOREACH( LaserRangeFinderReader* r, _laserRangeFinderReaders[3] )
	{
		r->setScan( ranges, intensities, parameters );
	}
}

void WorkerThread::on_northstar_changed( unsigned int sequenceNumber,
										unsigned int roomId,
										unsigned int numSpotsVisible,
										float posX,
										float posY,
										float posTheta,
										unsigned int magSpot0,
										unsigned int magSpot1 )
{
	QMutexLocker lk( &_mutex );
	++_numNorthStarEvents;
	Q_FOREACH( NorthStarReader* r, _northStarReaders )
	{
		r->setNorthStarReadings( sequenceNumber, roomId, numSpotsVisible, posX, posY, posTheta, magSpot0, magSpot1 );
	}
}

void WorkerThread::on_odometry_changed( double x, double y, double phi, float vx, float vy, float omega, unsigned int sequence )
{
	QMutexLocker lk( &_mutex );
	++_numOdometryEvents;
	Q_FOREACH( OdometryReader* r, _odometryReaders )
	{
		r->setOdometryReadings( x, y, phi, vx, vy, omega, sequence );
	}

	if( 0 == sequence || sequence < _odometrySequence )
	{
		QMutexLocker lk( &_odometrySetMutex );
		_odometrySetCondition.wakeAll();
	}
	_odometrySequence = sequence;
}

void WorkerThread::on_pose_changed(double x, double y, double phi, double errx, double erry, double errphi, unsigned int sequence)
{
	QMutexLocker lk(&_mutex);
	++_numPoseEvents;
	Q_FOREACH(PoseReader* r, _poseReaders)
	{
		r->setPoseReadings(x, y, phi, errx, erry, errphi, sequence);
	}
}

void WorkerThread::on_kinect0_depth_changed( const QByteArray& data, const QByteArray& object_data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	QMutexLocker lk( &_mutex );
	++_numDepthEvents[0];
	Q_FOREACH( KinectReader* r, _kinectReaders[0] )
	{
		r->setDepth( data, object_data, width, height, format, stamp );
	}
}

void WorkerThread::on_kinect1_depth_changed( const QByteArray& data, const QByteArray& object_data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	QMutexLocker lk( &_mutex );
	++_numDepthEvents[1];
	Q_FOREACH( KinectReader* r, _kinectReaders[1] )
	{
		r->setDepth( data, object_data, width, height, format, stamp );
	}
}

void WorkerThread::on_kinect2_depth_changed( const QByteArray& data, const QByteArray& object_data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	QMutexLocker lk( &_mutex );
	++_numDepthEvents[2];
	Q_FOREACH( KinectReader* r, _kinectReaders[2] )
	{
		r->setDepth( data, object_data, width, height, format, stamp );
	}
}

void WorkerThread::on_kinect3_depth_changed( const QByteArray& data, const QByteArray& object_data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	QMutexLocker lk( &_mutex );
	++_numDepthEvents[3];
	Q_FOREACH( KinectReader* r, _kinectReaders[3] )
	{
		r->setDepth( data, object_data, width, height, format, stamp );
	}
}

void WorkerThread::on_kinect0_video_changed( const QByteArray& data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	QMutexLocker lk( &_mutex );
	++_numVideoEvents[0];
	Q_FOREACH( KinectReader* r, _kinectReaders[0] )
	{
		r->setVideo( data, width, height, format, stamp );
	}
}

void WorkerThread::on_kinect1_video_changed( const QByteArray& data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	QMutexLocker lk( &_mutex );
	++_numVideoEvents[1];
	Q_FOREACH( KinectReader* r, _kinectReaders[1] )
	{
		r->setVideo( data, width, height, format, stamp );
	}
}

void WorkerThread::on_kinect2_video_changed( const QByteArray& data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	QMutexLocker lk( &_mutex );
	++_numVideoEvents[2];
	Q_FOREACH( KinectReader* r, _kinectReaders[2] )
	{
		r->setVideo( data, width, height, format, stamp );
	}
}

void WorkerThread::on_kinect3_video_changed( const QByteArray& data, unsigned int width, unsigned int height, unsigned int format, unsigned int stamp )
{
	QMutexLocker lk( &_mutex );
	++_numVideoEvents[3];
	Q_FOREACH( KinectReader* r, _kinectReaders[3] )
	{
		r->setVideo( data, width, height, format, stamp );
	}
}


void WorkerThread::on_kinect0_tilt_changed( double tilt )
{
	QMutexLocker lk( &_mutex );
	++_numTiltEvents[0];
	Q_FOREACH( KinectReader* r, _kinectReaders[0] )
	{
		r->setTilt( tilt );
	}
}

void WorkerThread::on_kinect1_tilt_changed( double tilt )
{
	QMutexLocker lk( &_mutex );
	++_numTiltEvents[1];
	Q_FOREACH( KinectReader* r, _kinectReaders[1] )
	{
		r->setTilt( tilt );
	}
}

void WorkerThread::on_kinect2_tilt_changed( double tilt )
{
	QMutexLocker lk( &_mutex );
	++_numTiltEvents[2];
	Q_FOREACH( KinectReader* r, _kinectReaders[2] )
	{
		r->setTilt( tilt );
	}
}

void WorkerThread::on_kinect3_tilt_changed( double tilt )
{
	QMutexLocker lk( &_mutex );
	++_numTiltEvents[3];
	Q_FOREACH( KinectReader* r, _kinectReaders[3] )
	{
		r->setTilt( tilt );
	}
}


void WorkerThread::on_kinect0_accel_changed( double x, double y, double z )
{
	QMutexLocker lk( &_mutex );
	++_numAccelEvents[0];
	Q_FOREACH( KinectReader* r, _kinectReaders[0] )
	{
		r->setAccel( x, y, z );
	}
}

void WorkerThread::on_kinect1_accel_changed( double x, double y, double z )
{
	QMutexLocker lk( &_mutex );
	++_numAccelEvents[1];
	Q_FOREACH( KinectReader* r, _kinectReaders[1] )
	{
		r->setAccel( x, y, z );
	}
}

void WorkerThread::on_kinect2_accel_changed( double x, double y, double z )
{
	QMutexLocker lk( &_mutex );
	++_numAccelEvents[2];
	Q_FOREACH( KinectReader* r, _kinectReaders[2] )
	{
		r->setAccel( x, y, z );
	}
}

void WorkerThread::on_kinect3_accel_changed( double x, double y, double z )
{
	QMutexLocker lk( &_mutex );
	++_numAccelEvents[3];
	Q_FOREACH( KinectReader* r, _kinectReaders[3] )
	{
		r->setAccel( x, y, z );
	}
}

void WorkerThread::on_charger0_info_changed( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state )
{
	QMutexLocker lk( &_mutex );
	++_numChargerInfoEvents[0];
	Q_FOREACH( ChargerReader* r, _chargerReaders[0] )
	{
		r->setChargerInfo( time, batteryVoltage, chargingCurrent, bat1temp, bat2temp, state_number, state );
	}
}

void WorkerThread::on_charger0_version_changed( int major, int minor, int patch )
{
	QMutexLocker lk( &_mutex );
	++_numChargerVersionEvents[0];
	Q_FOREACH( ChargerReader* r, _chargerReaders[0] )
	{
		r->setChargerVersion( major, minor, patch );
	}
}

void WorkerThread::on_charger0_error_changed( unsigned int time, const QString& message )
{
	QMutexLocker lk( &_mutex );
	++_numChargerErrorEvents[0];
	Q_FOREACH( ChargerReader* r, _chargerReaders[0] )
	{
		r->setChargerError( time, message );
	}
}

void WorkerThread::on_charger1_info_changed( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state )
{
	QMutexLocker lk( &_mutex );
	++_numChargerInfoEvents[1];
	Q_FOREACH( ChargerReader* r, _chargerReaders[1] )
	{
		r->setChargerInfo( time, batteryVoltage, chargingCurrent, bat1temp, bat2temp, state_number, state );
	}
}

void WorkerThread::on_charger1_version_changed( int major, int minor, int patch )
{
	QMutexLocker lk( &_mutex );
	++_numChargerVersionEvents[1];
	Q_FOREACH( ChargerReader* r, _chargerReaders[1] )
	{
		r->setChargerVersion( major, minor, patch );
	}
}

void WorkerThread::on_charger1_error_changed( unsigned int time, const QString& message )
{
	QMutexLocker lk( &_mutex );
	++_numChargerErrorEvents[1];
	Q_FOREACH( ChargerReader* r, _chargerReaders[1] )
	{
		r->setChargerError( time, message );
	}
}

void WorkerThread::on_charger2_info_changed( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state )
{
	QMutexLocker lk( &_mutex );
	++_numChargerInfoEvents[2];
	Q_FOREACH( ChargerReader* r, _chargerReaders[2] )
	{
		r->setChargerInfo( time, batteryVoltage, chargingCurrent, bat1temp, bat2temp, state_number, state );
	}
}

void WorkerThread::on_charger2_version_changed( int major, int minor, int patch )
{
	QMutexLocker lk( &_mutex );
	++_numChargerVersionEvents[2];
	Q_FOREACH( ChargerReader* r, _chargerReaders[2] )
	{
		r->setChargerVersion( major, minor, patch );
	}
}

void WorkerThread::on_charger2_error_changed( unsigned int time, const QString& message )
{
	QMutexLocker lk( &_mutex );
	++_numChargerErrorEvents[2];
	Q_FOREACH( ChargerReader* r, _chargerReaders[2] )
	{
		r->setChargerError( time, message );
	}
}

void WorkerThread::on_power_button_changed( bool pressed )
{
	QMutexLocker lk( &_mutex );
	++_numPowerButtonEvents;
	Q_FOREACH( PowerButtonReader* r, _powerButtonReaders )
	{
		r->setPowerButton( pressed );
	}
}

void WorkerThread::on_custom_message_changed( unsigned int id, const QByteArray& data )
{
	QMutexLocker lk( &_mutex );
	++_numCustomMessageEvents;
	Q_FOREACH( CustomMessageReader* r, _customMessageReaders )
	{
		r->setCustomMessage( id, data );
	}
}

void WorkerThread::on_gyroscope_changed( double phi, double rate, const rec::rpc::ClientInfo& )
{
	QMutexLocker lk( &_mutex );
	++_numGyroscopeEvents;
	Q_FOREACH( GyroscopeReader* r, _gyroscopeReaders )
	{
		r->setGyroscope( phi, rate );
	}
}

void WorkerThread::on_gyroscope_ext_changed(double phi, double rate, const rec::rpc::ClientInfo&)
{
	QMutexLocker lk(&_mutex);
	++_numGyroscopeExtEvents;
	Q_FOREACH(GyroscopeExtReader* r, _gyroscopeExtReaders)
	{
		r->setGyroscopeExt(phi, rate);
	}
}

JSON_TOPIC_SLOT(smartdockingvis, SmartDockingVisReader)
JSON_TOPIC_SLOT(festool_charger_info, FestoolChargerReader)

void WorkerThread::on_map_changed(const QByteArray& data, int width, int height, float resolution, float offsetx, float offsety)
{
	QMutexLocker lk(&_mutex);
	++_num_map_events;
	Q_FOREACH(Factory4Reader* r, _Factory4Readers)
	{
		r->set_map(data, width, height, resolution, offsetx, offsety);
	}
}

void WorkerThread::on_mapPlanner_changed(const QByteArray& data, int width, int height, float resolution, float offsetx, float offsety)
{
	QMutexLocker lk(&_mutex);
	++_num_mapPlanner_events;
	Q_FOREACH(Factory4Reader* r, _Factory4Readers)
	{
		r->set_mapPlanner(data, width, height, resolution, offsetx, offsety);
	}
}

JSON_TOPIC_SLOT(fleetcom_response,Factory4Reader)
JSON_TOPIC_SLOT(mclayout, Factory4Reader)
JSON_TOPIC_SLOT(mcstatus, Factory4Reader)
JSON_TOPIC_SLOT(pathnetwork, Factory4Reader)
JSON_TOPIC_SLOT(localizationMode, Factory4Reader)
JSON_TOPIC_SLOT(smartlog, Factory4Reader)
JSON_TOPIC_SLOT(smartnavigationplan, Factory4Reader)
JSON_TOPIC_SLOT(smartrobotinfo, Factory4Reader)
JSON_TOPIC_SLOT(smartlocations, Factory4Reader)
JSON_TOPIC_SLOT(smartstations, Factory4Reader)
JSON_TOPIC_SLOT(smartmyrobotid, Factory4Reader)
JSON_TOPIC_SLOT(smartjoblist, Factory4Reader)
JSON_TOPIC_SLOT(mapDir, Factory4Reader)
JSON_TOPIC_SLOT(smartlicense, Factory4Reader)
JSON_WITH_DATA_TOPIC_SLOT(mapDir_response, Factory4Reader)

void WorkerThread::on_json_message_with_data_changed(int topic, const QByteArray& jsonData, const QByteArray& data)
{
	switch (topic)
	{
	case rec::robotino::rpc::smartNavigationPlannner_topic:
		++_num_smartNavigationPlanner_events;
		Q_FOREACH(SmartNavigationPlannerReader* r, _SmartNavigationPlannerReaders)
		{
			r->set_smartNavigationPlanner(jsonData, data);
		}
		break;

	case rec::robotino::rpc::smartNavigationPlanner_response_topic:
		++_num_smartNavigationPlanner_response_events;
		Q_FOREACH(SmartNavigationPlannerReader* r, _SmartNavigationPlannerReaders)
		{
			r->set_smartNavigationPlanner_response(jsonData, data);
		}
		break;

	case rec::robotino::rpc::smartNavigationCostMap_topic:
		++_num_smartNavigationCostMap_events;
		Q_FOREACH(SmartNavigationPlannerReader* r, _SmartNavigationPlannerReaders)
		{
			r->set_smartNavigationCostMap(jsonData, data);
		}
		break;

	case rec::robotino::rpc::smartCdl_topic:
		++_num_smartCdl_events;
		Q_FOREACH(SmartCdlReader* r, _SmartCdlReaders)
		{
			r->set_smartCdl(jsonData, data);
		}
		break;
		
	case rec::robotino::rpc::customVis_topic:
		++_num_customVis_events;
		Q_FOREACH(CustomVisReader* r, _CustomVisReaders)
		{
			r->set_customVis(jsonData, data);
		}
		break;

	case rec::robotino::rpc::smartPersonTracker_topic:
		++_num_smartPersonTracker_events;
		Q_FOREACH(SmartPersonTrackerReader* r, _SmartPersonTrackerReaders)
		{
			r->set_smartPersonTracker(jsonData, data);
		}
		break;

	case rec::robotino::rpc::mapDirWithData_topic:
		++_num_mapDirWithData_events;
		Q_FOREACH(Factory4Reader* r, _Factory4Readers)
		{
			r->set_mapDirWithData(jsonData, data);
		}
		break;

	case rec::robotino::rpc::fileSystem_response_topic:
		++_num_smartLog_events;
		Q_FOREACH(SmartLogReader* r, _SmartLogReaders)
		{
			r->set_smartLog(jsonData, data);
		}
		break;

	default:
		break;
	}
}

void WorkerThread::on_set_digital_output_changed(unsigned int index, bool state)
{
	QMutexLocker lk(&_mutex);
	++_numDigitalOutputReaderEvents;
	Q_FOREACH(DigitalOutputReaderReader* r, _DigitalOutputReaderReaders)
	{
		r->setDigitalOutput(index, state);
	}
}

void WorkerThread::on_set_digital_output_array_changed(const QVector< bool >& data)
{
	QMutexLocker lk(&_mutex);
	++_numDigitalOutputReaderEvents;
	Q_FOREACH(DigitalOutputReaderReader* r, _DigitalOutputReaderReaders)
	{
		r->setDigitalOutputArray(data);
	}
}

void WorkerThread::on_motor_debug_changed(int motor, const QVector<unsigned int>& times, const QVector<float>& speed_setpoints, const QVector<float>& speeds, const QVector<float>& currents, const QVector<float>& control_points)
{
	QMutexLocker lk(&_mutex);
	++_numMotorDebugEvents;
	Q_FOREACH(MotorDebugReader* r, _motorDebugReaders)
	{
		r->setMotorDebug(motor, times, speed_setpoints, speeds, currents, control_points);
	}
}

void WorkerThread::run()
{
	_client = new Client;

	bool connected = true;

	connected &= (bool)connect( _client, SIGNAL( is_connected() ), this, SLOT( on_is_connected() ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( is_disconnected() ), this, SLOT( on_is_disconnected() ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( error( QAbstractSocket::SocketError, const QString& ) )
		, this, SLOT( on_error( QAbstractSocket::SocketError, const QString& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( motor_readings_changed( const QVector< float >&, const QVector< int >&, const QVector< float >&, float ) )
		, this, SLOT( on_motor_readings_changed( const QVector< float >&, const QVector< int >&, const QVector< float >&, float ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( digital_input_changed( const QVector< bool >& ) )
		, this, SLOT( on_digital_input_changed( const QVector< bool >& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( analog_input_changed( const QVector< float >& ) )
		, this, SLOT( on_analog_input_changed( const QVector< float >& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( distance_sensors_changed( const QVector< float >& ) )
		, this, SLOT( on_distance_sensors_changed( const QVector< float >& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( bumper_changed( bool ) )
		, this, SLOT( on_bumper_changed( bool ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( battery_changed( float, float, bool, int, const QString&, bool, int ) )
		, this, SLOT( on_battery_changed( float, float, bool, int, const QString&, bool, int ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( parameters_changed( const QMap< QString, QVariant >& ) )
		, this, SLOT( on_parameters_changed( const QMap< QString, QVariant >& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( image0_changed( const QByteArray&,
									unsigned int,
									unsigned int,
									unsigned int,
									const QString& ) )
		, this, SLOT( on_image0_changed( const QByteArray&,
									unsigned int,
									unsigned int,
									unsigned int,
									const QString& ) )
		, Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( camera0_settings_changed( unsigned int, unsigned int, const QString& ) )
		, this, SLOT( on_camera0_settings_changed( unsigned int, unsigned int, const QString& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( camera0_capabilities_changed( const QString&, const QMap<QString, QVector<QSize> >&, const QStringList& ) )
		, this, SLOT( on_camera0_capabilities_changed( const QString&, const QMap<QString, QVector<QSize> >&, const QStringList& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( camera0_calibration_changed( const QVector<double>& ) )
		, this, SLOT( on_camera0_calibration_changed( const QVector<double>& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( image1_changed( const QByteArray&,
									unsigned int,
									unsigned int,
									unsigned int,
									const QString& ) )
		, this, SLOT( on_image1_changed( const QByteArray&,
									unsigned int,
									unsigned int,
									unsigned int,
									const QString& ) )
		, Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( camera1_settings_changed( unsigned int, unsigned int, const QString& ) )
		, this, SLOT( on_camera1_settings_changed( unsigned int, unsigned int, const QString& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( camera1_capabilities_changed( const QString&, const QMap<QString, QVector<QSize> >&, const QStringList& ) )
		, this, SLOT( on_camera1_capabilities_changed( const QString&, const QMap<QString, QVector<QSize> >&, const QStringList& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( camera1_calibration_changed( const QVector<double>& ) )
		, this, SLOT( on_camera1_calibration_changed( const QVector<double>& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( image2_changed( const QByteArray&,
									unsigned int,
									unsigned int,
									unsigned int,
									const QString& ) )
		, this, SLOT( on_image2_changed( const QByteArray&,
									unsigned int,
									unsigned int,
									unsigned int,
									const QString& ) )
		, Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( camera2_settings_changed( unsigned int, unsigned int, const QString& ) )
		, this, SLOT( on_camera2_settings_changed( unsigned int, unsigned int, const QString& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( camera2_capabilities_changed( const QString&, const QMap<QString, QVector<QSize> >&, const QStringList& ) )
		, this, SLOT( on_camera2_capabilities_changed( const QString&, const QMap<QString, QVector<QSize> >&, const QStringList& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( camera2_calibration_changed( const QVector<double>& ) )
		, this, SLOT( on_camera2_calibration_changed( const QVector<double>& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( image3_changed( const QByteArray&,
									unsigned int,
									unsigned int,
									unsigned int,
									const QString& ) )
		, this, SLOT( on_image3_changed( const QByteArray&,
									unsigned int,
									unsigned int,
									unsigned int,
									const QString& ) )
		, Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( camera3_settings_changed( unsigned int, unsigned int, const QString& ) )
		, this, SLOT( on_camera3_settings_changed( unsigned int, unsigned int, const QString& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( camera3_capabilities_changed( const QString&, const QMap<QString, QVector<QSize> >&, const QStringList& ) )
		, this, SLOT( on_camera3_capabilities_changed( const QString&, const QMap<QString, QVector<QSize> >&, const QStringList& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( camera3_calibration_changed( const QVector<double>& ) )
		, this, SLOT( on_camera3_calibration_changed( const QVector<double>& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( cbha_readings_changed( const QVector<float>&, bool, const QVector<float>&, float ) )
		, this, SLOT( on_cbha_readings_changed( const QVector<float>&, bool, const QVector<float>&, float ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( gripper_state_changed( int ) )
		, this, SLOT( on_gripper_state_changed( int ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( grappler_store_positions_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& ) )
		, this, SLOT( on_grappler_store_positions_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( grappler_servos_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& ) )
		, this, SLOT( on_grappler_servos_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( grappler_readings_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& ) )
		, this, SLOT( on_grappler_readings_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( grappler_toggle_torque_changed() )
		, this, SLOT( on_grappler_toggle_torque_changed() ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( scan0_changed( const QVector< float >&, const QVector< float >&, const QVariantMap& ) )
		, this, SLOT( on_scan0_changed( const QVector< float >&, const QVector< float >&, const QVariantMap& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( scan1_changed( const QVector< float >&, const QVector< float >&, const QVariantMap& ) )
		, this, SLOT( on_scan1_changed( const QVector< float >&, const QVector< float >&, const QVariantMap& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( scan2_changed( const QVector< float >&, const QVector< float >&, const QVariantMap& ) )
		, this, SLOT( on_scan2_changed( const QVector< float >&, const QVector< float >&, const QVariantMap& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( scan3_changed( const QVector< float >&, const QVector< float >&, const QVariantMap& ) )
		, this, SLOT( on_scan3_changed( const QVector< float >&, const QVector< float >&, const QVariantMap& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( northstar_changed( unsigned int,
		unsigned int,
		unsigned int,
		float,
		float,
		float,
		unsigned int,
		unsigned int ) )
		,this, SLOT( on_northstar_changed( unsigned int,
		unsigned int,
		unsigned int,
		float,
		float,
		float,
		unsigned int,
		unsigned int ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( odometry_changed( double, double, double, float, float, float, unsigned int ) )
		,this, SLOT( on_odometry_changed( double, double, double, float, float, float, unsigned int ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect(_client, SIGNAL(pose_changed(double /*x*/, double /*y*/, double /*phi*/, double /*errx*/, double /*erry*/, double /*errphi*/, unsigned int /*sequence*/))
		, this, SLOT(on_pose_changed(double /*x*/, double /*y*/, double /*phi*/, double /*errx*/, double /*erry*/, double /*errphi*/, unsigned int /*sequence*/)), Qt::DirectConnection);
	Q_ASSERT(connected);

	connected &= (bool)connect( _client, SIGNAL( kinect0_depth_changed( const QByteArray&, const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) )
		,this, SLOT( on_kinect0_depth_changed( const QByteArray&, const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( kinect1_depth_changed( const QByteArray&, const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) )
		,this, SLOT( on_kinect1_depth_changed( const QByteArray&, const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( kinect2_depth_changed( const QByteArray&, const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) )
		,this, SLOT( on_kinect2_depth_changed( const QByteArray&, const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ), Qt::DirectConnection );
	Q_ASSERT( (bool)connected );

	connected &= (bool)connect( _client, SIGNAL( kinect3_depth_changed( const QByteArray&, const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) )
		,this, SLOT( on_kinect3_depth_changed( const QByteArray&, const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( kinect0_video_changed( const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) )
		,this, SLOT( on_kinect0_video_changed( const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( kinect1_video_changed( const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) )
		,this, SLOT( on_kinect1_video_changed( const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( kinect2_video_changed( const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) )
		,this, SLOT( on_kinect2_video_changed( const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( kinect3_video_changed( const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) )
		,this, SLOT( on_kinect3_video_changed( const QByteArray&, unsigned int, unsigned int, unsigned int, unsigned int ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( kinect0_tilt_changed( double ) )
		,this, SLOT( on_kinect0_tilt_changed( double ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( kinect1_tilt_changed( double ) )
		,this, SLOT( on_kinect1_tilt_changed( double ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( kinect2_tilt_changed( double ) )
		,this, SLOT( on_kinect2_tilt_changed( double ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( kinect3_tilt_changed( double ) )
		,this, SLOT( on_kinect3_tilt_changed( double ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( kinect0_accel_changed( double, double, double ) )
		,this, SLOT( on_kinect0_accel_changed( double, double, double ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( kinect1_accel_changed( double, double, double ) )
		,this, SLOT( on_kinect1_accel_changed( double, double, double ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( kinect2_accel_changed( double, double, double ) )
		,this, SLOT( on_kinect2_accel_changed( double, double, double ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( kinect3_accel_changed( double, double, double ) )
		,this, SLOT( on_kinect3_accel_changed( double, double, double ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( log( const QString&, int ) )
		,this, SLOT( on_log( const QString&, int ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( charger0_info_changed( unsigned int, float, float, float, float, int, const QString& ) ),
		this, SLOT( on_charger0_info_changed( unsigned int, float, float, float, float, int, const QString& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( charger0_version_changed( int, int, int ) ),
		this, SLOT( on_charger0_version_changed( int, int, int ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( charger0_error_changed( unsigned int, const QString& ) ),
		this, SLOT( on_charger0_error_changed( unsigned int, const QString& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( charger1_info_changed( unsigned int, float, float, float, float, int, const QString& ) ),
		this, SLOT( on_charger1_info_changed( unsigned int, float, float, float, float, int, const QString& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( charger1_version_changed( int, int, int ) ),
		this, SLOT( on_charger1_version_changed( int, int, int ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( charger1_error_changed( unsigned int, const QString& ) ),
		this, SLOT( on_charger1_error_changed( unsigned int, const QString& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( charger2_info_changed( unsigned int, float, float, float, float, int, const QString& ) ),
		this, SLOT( on_charger2_info_changed( unsigned int, float, float, float, float, int, const QString& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( charger2_version_changed( int, int, int ) ),
		this, SLOT( on_charger2_version_changed( int, int, int ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( charger2_error_changed( unsigned int, const QString& ) ),
		this, SLOT( on_charger2_error_changed( unsigned int, const QString& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client, SIGNAL( power_button_changed( bool ) ),
		this, SLOT( on_power_button_changed( bool ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client
		, SIGNAL( custom_message_changed( unsigned int, const QByteArray& ) )
		, SLOT ( on_custom_message_changed( unsigned int, const QByteArray& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _client
		, SIGNAL( gyroscope_changed( double, double, const rec::rpc::ClientInfo& ) )
		, SLOT ( on_gyroscope_changed( double, double, const rec::rpc::ClientInfo& ) ), Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect(_client
		, SIGNAL(gyroscope_ext_changed(double, double, const rec::rpc::ClientInfo&))
		, SLOT(on_gyroscope_ext_changed(double, double, const rec::rpc::ClientInfo&)), Qt::DirectConnection);
	Q_ASSERT(connected);

	JSON_TOPIC_CONNECTION(smartlicense)
	JSON_TOPIC_CONNECTION(smartdockingvis)

	connected &= (bool)connect(_client
		, SIGNAL(map_changed(const QByteArray&, int, int, float, float, float))
		, SLOT(on_map_changed(const QByteArray&, int, int, float, float, float)), Qt::DirectConnection);
	Q_ASSERT(connected);

	connected &= (bool)connect(_client
		, SIGNAL(mapPlanner_changed(const QByteArray&, int, int, float, float, float))
		, SLOT(on_mapPlanner_changed(const QByteArray&, int, int, float, float, float)), Qt::DirectConnection);
	Q_ASSERT(connected);

	JSON_TOPIC_CONNECTION(mclayout)
	JSON_TOPIC_CONNECTION(mcstatus)
	JSON_TOPIC_CONNECTION(pathnetwork)
	JSON_TOPIC_CONNECTION(localizationMode)
	JSON_TOPIC_CONNECTION(smartlog)
	JSON_TOPIC_CONNECTION(festool_charger_info)
	JSON_TOPIC_CONNECTION(smartnavigationplan)
	JSON_TOPIC_CONNECTION(smartrobotinfo)
	JSON_TOPIC_CONNECTION(smartlocations)
	JSON_TOPIC_CONNECTION(smartstations)
	JSON_TOPIC_CONNECTION(smartmyrobotid)
	JSON_TOPIC_CONNECTION(smartjoblist)
	JSON_TOPIC_CONNECTION(mapDir)
	JSON_TOPIC_CONNECTION(fleetcom_response)
	JSON_WITH_DATA_TOPIC_CONNECTION(mapDir_response)

	connected &= (bool)connect(_client
	, SIGNAL(json_message_with_data_changed(int, const QByteArray&, const QByteArray&))
	, SLOT(on_json_message_with_data_changed(int, const QByteArray&, const QByteArray&)), Qt::DirectConnection);
	Q_ASSERT(connected);

	connected &= (bool)connect(_client
		, SIGNAL(set_digital_output_changed(unsigned int /*index*/, bool /*state*/))
		, SLOT(on_set_digital_output_changed(unsigned int /*index*/, bool /*state*/)), Qt::DirectConnection);
	Q_ASSERT(connected);

	connected &= (bool)connect(_client
		, SIGNAL(set_digital_output_array_changed(const QVector< bool >&))
		, SLOT(on_set_digital_output_array_changed(const QVector< bool >&)), Qt::DirectConnection);
	Q_ASSERT(connected);

	connected &= (bool)connect(_client
		, SIGNAL(motor_debug_changed(int /*motor*/, const QVector<unsigned int>& /*times*/, const QVector<float>& /*speed_setpoints*/, const QVector<float>& /*speeds*/, const QVector<float>& /*currents*/, const QVector<float>& /*control_points*/))
		, SLOT(on_motor_debug_changed(int /*motor*/, const QVector<unsigned int>& /*times*/, const QVector<float>& /*speed_setpoints*/, const QVector<float>& /*speeds*/, const QVector<float>& /*currents*/, const QVector<float>& /*control_points*/)), Qt::DirectConnection);
	Q_ASSERT(connected);

	//******************************

	connected &= (bool)connect( this, SIGNAL( set_omnidrive_signal( float, float, float ) )
		,_client, SLOT( set_omnidrive( float, float, float ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( setTopicEnabled_signal( const QString&, bool ) )
		, _client, SLOT(setTopicEnabled(const QString&, bool)), Qt::QueuedConnection);
	Q_ASSERT( connected );

	connected &= (bool)connect(this, SIGNAL(setJsonWithDataTopicEnabled_signal(int, bool))
		, _client, SLOT(set_json_message_with_data_topic_Enabled(int, bool)), Qt::QueuedConnection);
	Q_ASSERT(connected);

	connected &= (bool)connect( this, SIGNAL( set_motor0_setpoint_signal( float ) )
		,_client, SLOT( set_motor0_setpoint( float ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( motor0_reset_position_signal( int ) )
		,_client, SLOT( set_motor0_reset_position( int ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_set_motor0_mode_signal( const QString& ) )
		,_client, SLOT( set_set_motor0_mode( const QString& ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_motor1_setpoint_signal( float ) )
		,_client, SLOT( set_motor1_setpoint( float ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( motor1_reset_position_signal( int ) )
		,_client, SLOT( set_motor1_reset_position( int ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_set_motor1_mode_signal( const QString& ) )
		,_client, SLOT( set_set_motor1_mode( const QString& ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_motor2_setpoint_signal( float ) )
		,_client, SLOT( set_motor2_setpoint( float ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( motor2_reset_position_signal( int ) )
		,_client, SLOT( set_motor2_reset_position( int ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_set_motor2_mode_signal( const QString& ) )
		,_client, SLOT( set_set_motor2_mode( const QString& ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_motor3_setpoint_signal( float ) )
		,_client, SLOT( set_motor3_setpoint( float ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( motor3_reset_position_signal( int ) )
		,_client, SLOT( set_motor3_reset_position( int ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_set_motor3_mode_signal( const QString& ) )
		,_client, SLOT( set_set_motor3_mode( const QString& ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_motor_setpoints_signal( float, float, float ) )
		,_client, SLOT( set_motor_setpoints( float, float, float ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_set_pid_parameters_signal( unsigned int, float, float, float ) )
		,_client, SLOT( set_set_pid_parameters( unsigned int, float, float, float ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_cbha_set_pressure_signal( const QVector<float>& ) )
		,_client, SLOT( set_cbha_set_pressure( const QVector<float>& ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_cbha_set_compressors_enabled_signal( bool ) )
		,_client, SLOT( set_cbha_set_compressors_enabled( bool ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_cbha_set_water_drain_valve_signal( bool ) )
		,_client, SLOT( set_cbha_set_water_drain_valve( bool ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_cbha_set_gripper_valve1_signal( bool ) )
		,_client, SLOT( set_cbha_set_gripper_valve1( bool ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_cbha_set_gripper_valve2_signal( bool ) )
		,_client, SLOT( set_cbha_set_gripper_valve2( bool ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_grappler_set_power_signal( int, bool ) )
		,_client, SLOT( set_grappler_set_power( int, bool ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_grappler_set_positions_signal( const QVector< rec::robotino::rpc::GrapplerServoInfo>& ) )
		,_client, SLOT( set_grappler_set_positions( const QVector< rec::robotino::rpc::GrapplerServoInfo>& ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_grappler_toggle_torque_signal() )
		,_client, SLOT( set_grappler_toggle_torque() ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_set_camera_settings_signal( unsigned int, unsigned int, unsigned int, const QString& ) )
		,_client, SLOT( set_set_camera_settings( unsigned int, unsigned int, unsigned int, const QString& ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_set_camera_control_signal( unsigned int, const QString&, int ) )
		,_client, SLOT( set_set_camera_control( unsigned int, const QString&, int ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_image_signal( unsigned int,
									const QByteArray&,
									unsigned int,
									unsigned int,
									unsigned int,
									const QString& ) )
		,_client, SLOT( set_image( unsigned int,
									const QByteArray&,
									unsigned int,
									unsigned int,
									unsigned int,
									const QString& ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_set_digital_output_signal( unsigned int, bool ) )
		,_client, SLOT( set_set_digital_output( unsigned int, bool ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_set_digital_output_array_signal( const QVector< bool >& ) )
		,_client, SLOT( set_set_digital_output_array( const QVector< bool >& ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_set_relay_signal( unsigned int, bool ) )
		,_client, SLOT( set_set_relay( unsigned int, bool ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_set_relay_array_signal( const QVector< bool >& ) )
		,_client, SLOT( set_set_relay_array( const QVector< bool >& ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_set_shutdown_signal( bool ) )
		,_client, SLOT( set_set_shutdown( bool ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_set_northstar_parameters_signal( unsigned int, float ) )
		,_client, SLOT( set_set_northstar_parameters( unsigned int, float ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_set_odometry_signal( double, double, double ) )
		,_client, SLOT( set_set_odometry( double, double, double ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_kinect_set_tilt_signal( unsigned int, double ) )
		,_client, SLOT( set_kinect_set_tilt( unsigned int, double ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_kinect_set_led_signal( unsigned int, unsigned int ) )
		,_client, SLOT( set_kinect_set_led( unsigned int, unsigned int ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_kinect_set_video_format_signal( unsigned int, unsigned int ) )
		,_client, SLOT( set_kinect_set_video_format( unsigned int, unsigned int ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_kinect_set_depth_format_signal( unsigned int, unsigned int ) )
		,_client, SLOT( set_kinect_set_depth_format( unsigned int, unsigned int ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( charger0_clear_error_signal() )
		,_client, SLOT( set_charger0_clear_error() ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( charger1_clear_error_signal() )
		,_client, SLOT( set_charger1_clear_error() ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( charger2_clear_error_signal() )
		,_client, SLOT( set_charger2_clear_error() ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( set_shutdown_signal( bool ) )
		,_client, SLOT( set_set_shutdown( bool ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	STRING_SIGNAL_CONNECTION(fleetcom_request)
	JSON_WITH_DATA_SIGNAL_CONNECTION(mapDir_request)
	JSON_SIGNAL_CONNECTION(pathnetwork_edited)
	JSON_SIGNAL_CONNECTION(smartlocations_edited)
	JSON_SIGNAL_CONNECTION(smartstations_edited)

	connected &= (bool)connect(this, SIGNAL(set_mapPlannerEdited_signal(const QByteArray&, int /*width*/, int /*height*/, float /*resolution*/, float /*offsetx*/, float /*offsety*/))
		, _client, SLOT(set_mapPlannerEdited(const QByteArray&, int /*width*/, int /*height*/, float /*resolution*/, float /*offsetx*/, float /*offsety*/)), Qt::QueuedConnection);
	Q_ASSERT(connected);

	connected &= (bool)connect(this, SIGNAL(set_json_message_with_data_signal(int /*topic*/, const QByteArray& /*jsonData*/, const QByteArray& /*data*/))
		, _client, SLOT(set_json_message_with_data(int /*topic*/, const QByteArray& /*jsonData*/, const QByteArray& /*data*/)), Qt::QueuedConnection);
	Q_ASSERT(connected);

	connected &= (bool)connect( this, SIGNAL( set_custom_message_signal( unsigned int, const QByteArray& ) )
		,_client, SLOT ( set_custom_message( unsigned int, const QByteArray& ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect(this, SIGNAL(start_hotswap_signal())
		, _client, SLOT(start_hotswap()), Qt::QueuedConnection);
	Q_ASSERT(connected);

	connected &= (bool)connect(this, SIGNAL(start_motor_debug_signal(int))
		, _client, SLOT(start_motor_debug(int)), Qt::QueuedConnection);
	Q_ASSERT(connected);

	connected &= (bool)connect(this, SIGNAL(set_upload_program_signal(const QByteArray&, const QByteArray&))
		, _client, SLOT(set_upload_program(const QByteArray&, const QByteArray&)), Qt::QueuedConnection);
	Q_ASSERT(connected);

	_startSemaphore.release();

	exec();

	delete _client;
	_client = NULL;
}
