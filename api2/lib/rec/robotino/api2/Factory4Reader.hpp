//  Copyright (C) 2004-2015, Robotics Equipment Corporation GmbH

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

#ifndef _REC_ROBOTINO_API2_FACTORY4READER_HPP_
#define _REC_ROBOTINO_API2_FACTORY4READER_HPP_

#include <QtCore>

namespace rec
{
	namespace robotino
	{
		namespace api2
		{
			class Factory4Reader
			{
			public:
				Factory4Reader()
				{
				}

				virtual ~Factory4Reader()
				{
				}

				
				virtual void process_mclayout_event() = 0;
				virtual void process_mcstatus_event() = 0;
				virtual void process_pathnetwork_event() = 0;
				virtual void process_localizationMode_event() = 0;
				virtual void process_smartlog_event() = 0;
				virtual void process_smartnavigationplan_event() = 0;
				virtual void process_smartlocations_event() = 0;
				virtual void process_smartstations_event() = 0;
				virtual void process_smartrobotinfo_event() = 0;
				virtual void process_smartmyrobotid_event() = 0;
				virtual void process_smartjoblist_event() = 0;
				virtual void process_mapDir_event() = 0;
				virtual void process_mapDirWithData_event() = 0;
				virtual void process_smartlicense_event() = 0;
				virtual void process_fleetcom_response_event() = 0;
				virtual void process_mapDir_response_event() = 0;
				virtual void process_map_event() = 0;
				virtual void process_mapPlanner_event() = 0;

				virtual void set_mclayout(const QByteArray& data) = 0;
				virtual void set_mcstatus(const QByteArray& data) = 0;
				virtual void set_pathnetwork(const QByteArray& data) = 0;
				virtual void set_localizationMode(const QByteArray& data) = 0;
				virtual void set_smartlog(const QByteArray& data) = 0;
				virtual void set_smartnavigationplan(const QByteArray& data) = 0;
				virtual void set_smartlocations(const QByteArray& data) = 0;
				virtual void set_smartstations(const QByteArray& data) = 0;
				virtual void set_smartrobotinfo(const QByteArray& data) = 0;
				virtual void set_smartmyrobotid(const QByteArray& data) = 0;
				virtual void set_smartjoblist(const QByteArray& data) = 0;
				virtual void set_mapDir(const QByteArray& data) = 0;
				virtual void set_mapDirWithData(const QByteArray& info, const QByteArray& data) = 0;
				virtual void set_smartlicense(const QByteArray& data) = 0;
				virtual void set_fleetcom_response(const QByteArray& data) = 0;
				virtual void set_mapDir_response(const QByteArray& info, const QByteArray& data) = 0;
				virtual void set_map(const QByteArray& data, int width, int height, float resolution, float offsetx, float offsety) = 0;
				virtual void set_mapPlanner(const QByteArray& data, int width, int height, float resolution, float offsetx, float offsety) = 0;
			};
		}
	}
}

#endif //_REC_ROBOTINO_API2_FACTORY4READER_HPP_

