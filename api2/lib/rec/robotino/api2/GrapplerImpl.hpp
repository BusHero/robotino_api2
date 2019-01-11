//  Copyright (C) 2004-2013, Robotics Equipment Corporation GmbH

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

#ifndef _REC_ROBOTINO_API2_GrapplerIMPL_HPP_
#define _REC_ROBOTINO_API2_GrapplerIMPL_HPP_

#include "rec/robotino/api2/Grappler.h"
#include "rec/robotino/api2/GrapplerReader.hpp"
#include "rec/robotino/api2/GrapplerReadings.h"

#include <QtCore>

namespace rec
{
	namespace robotino
	{
		namespace api2
		{
			class GrapplerImpl : public GrapplerReader
			{
			public:
				GrapplerImpl( Grappler* grappler )
					: _grappler( grappler )
					, _mutex( QMutex::Recursive )
					, _toggleTorque( false )
				{
				}

				~GrapplerImpl()
				{
					QMutexLocker lk( &_mutex );
					_grappler->_impl = NULL;
				}

				void processGrapplerStorePositionsEvent()
				{
					QMutexLocker lk( &_mutex );
					_grappler->storePositionsEvent( _storePositions );
				}

				void processGrapplerReadingsEvent()
				{
					QMutexLocker lk( &_mutex );
					_grappler->readingsEvent( _readings );				
				}

				void processGrapplerServosEvent()
				{
					QMutexLocker lk( &_mutex );
					_grappler->servoInfoEvent( _info );	
				}

				void processGrapplerToggleTorqueEvent()
				{
					QMutexLocker lk( &_mutex );
					if( _toggleTorque )
					{
						_toggleTorque = false;
						_grappler->toggleTorqueEvent();
					}
				}

				void setGrapplerStorePositions( const QVector< rec::robotino::rpc::GrapplerServoInfo >& data )
				{
					QMutexLocker lk( &_mutex );

					_storePositions.numServos = data.size();
					for( int i=0; i<data.size() && i<GrapplerReadings::maxNumServos; ++i )
					{
						const rec::robotino::rpc::GrapplerServoInfo& info = data[i];
						_storePositions.angles[i] = info.angle;
						_storePositions.channels[i] = info.channel;
						_storePositions.ids[i] = info.id;
					}

					++_readings.sequenceNumber;
					_storePositions.sequenceNumber = _readings.sequenceNumber;
				}

				void setGrapplerReadings( const QVector< rec::robotino::rpc::GrapplerServoInfo >& data )
				{
					QMutexLocker lk( &_mutex );

					_readings.numServos = data.size();
					for( int i=0; i<data.size() && i<GrapplerReadings::maxNumServos; ++i )
					{
						const rec::robotino::rpc::GrapplerServoInfo& info = data[i];
						_readings.angles[i] = info.angle;
						_readings.speeds[i] = info.speed;
						_readings.channels[i] = info.channel;
						_readings.ids[i] = info.id;
						_readings.cwAxesLimits[i] = _info.cwAxesLimits[i];
						_readings.ccwAxesLimits[i] = _info.ccwAxesLimits[i];
					}
					++_readings.sequenceNumber;
				}

				void setGrapplerServos( const QVector< rec::robotino::rpc::GrapplerServoInfo >& data )
				{
					QMutexLocker lk( &_mutex );

					_info.numServos = data.size();
					for( int i=0; i<data.size() && i<GrapplerReadings::maxNumServos; ++i )
					{
						const rec::robotino::rpc::GrapplerServoInfo& info = data[i];
						_info.channels[i] = info.channel;
						_info.ids[i] = info.id;
						_info.cwAxesLimits[i] = info.cwAxisLimit;
						_info.ccwAxesLimits[i] = info.ccwAxisLimit;
					}
					++_info.sequenceNumber;

					_setPointValues = data;
				}

				void setGrapplerToggleTorque()
				{
					QMutexLocker lk( &_mutex );
					_toggleTorque = true;
				}

				Grappler* _grappler;
				QMutex _mutex;

				GrapplerReadings _info;
				GrapplerReadings _readings;
				GrapplerReadings _storePositions;
				bool _toggleTorque;

				QVector< rec::robotino::rpc::GrapplerServoInfo> _setPointValues;
			};
		}
	}
}

#endif //_REC_ROBOTINO_API2_GrapplerIMPL_HPP_
