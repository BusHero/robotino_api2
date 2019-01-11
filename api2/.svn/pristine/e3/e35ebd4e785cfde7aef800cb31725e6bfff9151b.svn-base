//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

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

#include "rec/robotino/api2/Factory4.h"
#include "rec/robotino/api2/Factory4Impl.hpp"
#include "rec/robotino/api2/RobotinoException.h"
#include "rec/robotino/api2/WorkerThread.hpp"

#define JSON_TOPIC_VIRTUAL_FUNCTION(TOPICNAME) \
	void Factory4::TOPICNAME##Event(const char* data, const unsigned int dataSize){}

#define JSON_WITH_DATA_TOPIC_VIRTUAL_FUNCTION(TOPICNAME) \
	void Factory4::TOPICNAME##Event(const char* infoData, const unsigned int infoDataSize, const char* data, const unsigned int dataSize){}

using namespace rec::robotino::api2;

Factory4::Factory4()
: _impl( new Factory4Impl( this ) )
{
	try
	{
		setComId( _comID );
	}
	catch( const rec::robotino::api2::RobotinoException& )
	{
	}
}

Factory4::~Factory4()
{
	try
	{
		QMutexLocker lk( &_impl->_mutex );
		WorkerThread* thread = WorkerThread::instance( comId() );
		thread->removeFactory4Reader( _impl );
	}
	catch( const rec::robotino::api2::RobotinoException& )
	{
	}

	delete _impl;
}

void Factory4::setComId( const ComId& id )
{
	QMutexLocker lk( &_impl->_mutex );

	try
	{
		WorkerThread* thread = WorkerThread::instance( comId() );
		thread->removeFactory4Reader( _impl );
	}
	catch( const rec::robotino::api2::RobotinoException& )
	{
	}

	_comID = id;

	if( ComId::null() == comId() )
	{
		return;
	}
	
	{
		WorkerThread* thread = WorkerThread::instance( comId() );
		thread->addFactory4Reader( _impl );
	}
}

void Factory4::processEvents()
{
	try
	{
		QMutexLocker lk( &_impl->_mutex );
		WorkerThread* thread = WorkerThread::instance( comId() );
		thread->processFactory4Events();
	}
	catch( const rec::robotino::api2::RobotinoException& )
	{
	}
}

void Factory4::sendMessage(const char* message)
{
	try
	{
		QMutexLocker lk(&_impl->_mutex);
		WorkerThread* thread = WorkerThread::instance(comId());
		thread->set_fleetcom_request(message);
	}
	catch (const rec::robotino::api2::RobotinoException&)
	{
	}
}

void Factory4::setMapPlanner(const char* data, unsigned int dataSize)
{
	try
	{
		QMutexLocker lk(&_impl->_mutex);
		WorkerThread* thread = WorkerThread::instance(comId());
		thread->set_mapPlannerEdited(QByteArray(data, dataSize));
	}
	catch (const rec::robotino::api2::RobotinoException&)
	{
	}
}

void Factory4::mapDirRequest(const char* infoData, const unsigned int infoDataSize)
{
	mapDirRequest(infoData, infoDataSize, NULL, 0);
}

void Factory4::mapDirRequest(const char* infoData, const unsigned int infoDataSize, const char* data, const unsigned int dataSize)
{
	try
	{
		QMutexLocker lk(&_impl->_mutex);
		WorkerThread* thread = WorkerThread::instance(comId());
		QByteArray infoba(infoData, infoDataSize);
		QByteArray databa;
		if (data && dataSize > 0)
		{
			databa = QByteArray(data, dataSize);
		}
		thread->set_mapDir_request(infoba, databa);
	}
	catch (const rec::robotino::api2::RobotinoException&)
	{
	}
}

void Factory4::savePathNetwork(const char* data, const unsigned int dataSize)
{
    try
    {
        QMutexLocker lk(&_impl->_mutex);
        WorkerThread* thread = WorkerThread::instance(comId());
		thread->set_pathnetwork_edited(QByteArray(data, dataSize));
    }
    catch (const rec::robotino::api2::RobotinoException&)
    {
    }
}

void Factory4::setSmartLocations(const char* data, const unsigned int dataSize)
{
	try
	{
		QMutexLocker lk(&_impl->_mutex);
		WorkerThread* thread = WorkerThread::instance(comId());
		thread->set_smartlocations_edited(QByteArray(data, dataSize));
	}
	catch (const rec::robotino::api2::RobotinoException&)
	{
	}
}

void Factory4::setSmartStations(const char* data, const unsigned int dataSize)
{
	try
	{
		QMutexLocker lk(&_impl->_mutex);
		WorkerThread* thread = WorkerThread::instance(comId());
		thread->set_smartstations_edited(QByteArray(data, dataSize));
	}
	catch (const rec::robotino::api2::RobotinoException&)
	{
	}
}

JSON_TOPIC_VIRTUAL_FUNCTION(mclayout)
JSON_TOPIC_VIRTUAL_FUNCTION(mcstatus)
JSON_TOPIC_VIRTUAL_FUNCTION(pathnetwork)
JSON_TOPIC_VIRTUAL_FUNCTION(localizationMode)
JSON_TOPIC_VIRTUAL_FUNCTION(smartlog)
JSON_TOPIC_VIRTUAL_FUNCTION(smartnavigationplan)
JSON_TOPIC_VIRTUAL_FUNCTION(smartlocations)
JSON_TOPIC_VIRTUAL_FUNCTION(smartstations)
JSON_TOPIC_VIRTUAL_FUNCTION(smartrobotinfo)
JSON_TOPIC_VIRTUAL_FUNCTION(smartmyrobotid)
JSON_TOPIC_VIRTUAL_FUNCTION(smartjoblist)
JSON_TOPIC_VIRTUAL_FUNCTION(mapDir)
JSON_WITH_DATA_TOPIC_VIRTUAL_FUNCTION(mapDirWithData)
JSON_TOPIC_VIRTUAL_FUNCTION(fleetcom_response)
JSON_TOPIC_VIRTUAL_FUNCTION(smartlicense)
JSON_WITH_DATA_TOPIC_VIRTUAL_FUNCTION(mapDir_response)

void Factory4::mapEvent(const char* data, unsigned int dataSize, const rec::robotino::api2::Factory4MapInfo& info)
{

}

void Factory4::mapPlannerEvent(const char* data, unsigned int dataSize, const rec::robotino::api2::Factory4MapInfo& info)
{

}

