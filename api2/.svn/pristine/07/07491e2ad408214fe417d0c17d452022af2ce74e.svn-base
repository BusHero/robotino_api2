//  Copyright (C) 2004-2015, Robotics Equipment Corporation GmbH

//Copyright (c) ...
//
//REC Robotics Equipment Corporation GmbH, Planegg, Germany. All rights reserved.
//Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//
//THIS SOFTWARE IS PROVIDED BY REC ROBOTICS EQUIPMENT CORPORATION GMBH ?AS IS? AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL REC ROBOTICS EQUIPMENT CORPORATION GMBH
//BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
//GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
//LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//Copyright (c) ...
//
//REC Robotics Equipment Corporation GmbH, Planegg, Germany. Alle Rechte vorbehalten.
//Weiterverbreitung und Verwendung in nichtkompilierter oder kompilierter Form, mit oder ohne Ver?nderung, sind unter den folgenden Bedingungen zul?ssig:
//1) Weiterverbreitete nichtkompilierte Exemplare m?ssen das obige Copyright, diese Liste der Bedingungen und den folgenden Haftungsausschluss im Quelltext enthalten.
//2) Weiterverbreitete kompilierte Exemplare m?ssen das obige Copyright, diese Liste der Bedingungen und den folgenden Haftungsausschluss in der Dokumentation und/oder anderen Materialien, die mit dem Exemplar verbreitet werden, enthalten.
//
//DIESE SOFTWARE WIRD VON REC ROBOTICS EQUIPMENT CORPORATION GMBH OHNE JEGLICHE SPEZIELLE ODER IMPLIZIERTE GARANTIEN ZUR VERF?GUNG GESTELLT, DIE UNTER
//ANDEREM EINSCHLIESSEN: DIE IMPLIZIERTE GARANTIE DER VERWENDBARKEIT DER SOFTWARE F?R EINEN BESTIMMTEN ZWECK. AUF KEINEN FALL IST REC ROBOTICS EQUIPMENT CORPORATION GMBH
//F?R IRGENDWELCHE DIREKTEN, INDIREKTEN, ZUF?LLIGEN, SPEZIELLEN, BEISPIELHAFTEN ODER FOLGESCH?DEN (UNTER ANDEREM VERSCHAFFEN VON ERSATZG?TERN ODER -DIENSTLEISTUNGEN;
//EINSCHR?NKUNG DER NUTZUNGSF?HIGKEIT; VERLUST VON NUTZUNGSF?HIGKEIT; DATEN; PROFIT ODER GESCH?FTSUNTERBRECHUNG), WIE AUCH IMMER VERURSACHT UND UNTER WELCHER VERPFLICHTUNG
//AUCH IMMER, OB IN VERTRAG, STRIKTER VERPFLICHTUNG ODER UNERLAUBTER HANDLUNG (INKLUSIVE FAHRL?SSIGKEIT) VERANTWORTLICH, AUF WELCHEM WEG SIE AUCH IMMER DURCH DIE BENUTZUNG
//DIESER SOFTWARE ENTSTANDEN SIND, SOGAR, WENN SIE AUF DIE M?GLICHKEIT EINES SOLCHEN SCHADENS HINGEWIESEN WORDEN SIND.

#ifndef _REC_ROBOTINO_API2_FACTORY4IMPL_HPP_
#define _REC_ROBOTINO_API2_FACTORY4IMPL_HPP_

#include "rec/robotino/api2/Factory4.h"
#include "rec/robotino/api2/Factory4Reader.hpp"
#include "rec/robotino3/fleetcom/Factory.h"

#include <QtCore>

#define JSON_TOPIC_FUNCTIONS(TOPICNAME) \
	void process_##TOPICNAME##_event() \
	{ \
		QMutexLocker lk(&_mutex); \
		_factory4->TOPICNAME##Event(_##TOPICNAME##_data.constData(), _##TOPICNAME##_data.size()); \
	} \
	\
	void set_##TOPICNAME(const QByteArray& data) \
	{ \
		QMutexLocker lk(&_mutex); \
		_##TOPICNAME##_data = data; \
	}

#define JSON_WITH_DATA_TOPIC_FUNCTIONS(TOPICNAME) \
	void process_##TOPICNAME##_event() \
		{ \
		QMutexLocker lk(&_mutex); \
		_factory4->TOPICNAME##Event(_##TOPICNAME##_info.constData(), _##TOPICNAME##_info.size(),_##TOPICNAME##_data.constData(), _##TOPICNAME##_data.size()); \
		} \
	\
	void set_##TOPICNAME(const QByteArray& info, const QByteArray& data) \
		{ \
		QMutexLocker lk(&_mutex); \
		_##TOPICNAME##_info = info; \
		_##TOPICNAME##_data = data; \
		}

#define ENQUEUED_JSON_TOPIC_FUNCTIONS(TOPICNAME) \
	void process_##TOPICNAME##_event() \
		{ \
		QMutexLocker lk(&_mutex); \
		Q_FOREACH(const QByteArray& data,_##TOPICNAME##_data)\
		{\
			_factory4->TOPICNAME##Event(data.constData(), data.size()); \
		}\
		_##TOPICNAME##_data.clear();\
		} \
	\
	void set_##TOPICNAME(const QByteArray& data) \
	{ \
		QMutexLocker lk(&_mutex); \
		_##TOPICNAME##_data.append(data); \
		if(_##TOPICNAME##_data.count()>20)\
		{\
			_##TOPICNAME##_data.removeFirst();\
		}\
	}

namespace rec
{
	namespace robotino
	{
		namespace api2
		{
			class Factory4Impl : public Factory4Reader
			{
			public:
				Factory4Impl(Factory4* factory4)
					: _factory4(factory4)
					, _mutex(QMutex::Recursive)
				{
				}

				~Factory4Impl()
				{
					QMutexLocker lk(&_mutex);
					_factory4->_impl = NULL;
				}

				JSON_TOPIC_FUNCTIONS(mclayout)
				JSON_TOPIC_FUNCTIONS(mcstatus)
				JSON_TOPIC_FUNCTIONS(pathnetwork)
				JSON_TOPIC_FUNCTIONS(localizationMode)
				ENQUEUED_JSON_TOPIC_FUNCTIONS(smartlog)
				JSON_TOPIC_FUNCTIONS(smartnavigationplan)
				JSON_TOPIC_FUNCTIONS(smartlocations)
				JSON_TOPIC_FUNCTIONS(smartstations)
				JSON_TOPIC_FUNCTIONS(smartrobotinfo)
				JSON_TOPIC_FUNCTIONS(smartmyrobotid)
				JSON_TOPIC_FUNCTIONS(smartjoblist)
				JSON_TOPIC_FUNCTIONS(mapDir)
				JSON_WITH_DATA_TOPIC_FUNCTIONS(mapDirWithData)
				JSON_TOPIC_FUNCTIONS(smartlicense)
				ENQUEUED_JSON_TOPIC_FUNCTIONS(fleetcom_response)
				JSON_WITH_DATA_TOPIC_FUNCTIONS(mapDir_response)

				void process_map_event()
				{
					QMutexLocker lk(&_mutex);
					_factory4->mapEvent(_mapData.constData(), (unsigned int)_mapData.size(), _mapInfo);
				}

				void process_mapPlanner_event()
				{
					QMutexLocker lk(&_mutex);
					_factory4->mapPlannerEvent(_mapPlannerData.constData(), (unsigned int)_mapPlannerData.size(), _mapPlannerInfo);
				}

				void set_map(const QByteArray& data, int width, int height, float resolution, float offsetx, float offsety)
				{
					QMutexLocker lk(&_mutex);
					_mapData = data;
					_mapInfo.width = width;
					_mapInfo.height = height;
					_mapInfo.resolution = resolution;
					_mapInfo.offsetx = offsetx;
					_mapInfo.offsety = offsety;
					_mapInfo.sequence++;
				}

				void set_mapPlanner(const QByteArray& data, int width, int height, float resolution, float offsetx, float offsety)
				{
					QMutexLocker lk(&_mutex);
					_mapPlannerData = data;
					_mapPlannerInfo.width = width;
					_mapPlannerInfo.height = height;
					_mapPlannerInfo.resolution = resolution;
					_mapPlannerInfo.offsetx = offsetx;
					_mapPlannerInfo.offsety = offsety;
					_mapPlannerInfo.sequence++;
				}

				Factory4* _factory4;

				QByteArray _mclayout_data;
				QByteArray _mcstatus_data;
				QByteArray _pathnetwork_data;
				QByteArray _localizationMode_data;
				QList<QByteArray> _smartlog_data;
				QByteArray _smartnavigationplan_data;
				QByteArray _smartlocations_data;
				QByteArray _smartstations_data;
				QByteArray _smartrobotinfo_data;
				QByteArray _smartmyrobotid_data;
				QByteArray _smartjoblist_data;
				QByteArray _mapDir_data;
				QByteArray _smartlicense_data;
				QList<QByteArray> _fleetcom_response_data;

				QByteArray _mapDir_response_info;
				QByteArray _mapDir_response_data;

				QByteArray _mapDirWithData_info;
				QByteArray _mapDirWithData_data;

				QByteArray _mapData;
				Factory4MapInfo _mapInfo;

				QByteArray _mapPlannerData;
				Factory4MapInfo _mapPlannerInfo;

				QMutex _mutex;
			};
		}
	}
}

#endif //_REC_ROBOTINO_API2_FACTORY4IMPL_HPP_
