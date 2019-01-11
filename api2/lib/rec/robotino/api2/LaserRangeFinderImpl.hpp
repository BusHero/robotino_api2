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

#ifndef _REC_ROBOTINO_API2_rangefinderIMPL_HPP_
#define _REC_ROBOTINO_API2_rangefinderIMPL_HPP_

#include "rec/robotino/api2/LaserRangeFinder.h"
#include "rec/robotino/api2/LaserRangeFinderReader.hpp"
#include "rec/robotino/api2/LaserRangeFinderReadings.h"

#include <QtCore>

namespace rec
{
	namespace robotino
	{
		namespace api2
		{
			class LaserRangeFinderImpl : public LaserRangeFinderReader
			{
			public:
				LaserRangeFinderImpl( LaserRangeFinder* rangefinder )
					: _rangefinder( rangefinder )
					, _mutex( QMutex::Recursive )
					, _number( 0 )
				{
				}

				~LaserRangeFinderImpl()
				{
					QMutexLocker lk( &_mutex );
					_rangefinder->_impl = NULL;
				}

				void clone( LaserRangeFinderReader* destination ) const
				{
					LaserRangeFinderImpl* dest = dynamic_cast<LaserRangeFinderImpl*>( destination );
					Q_ASSERT( dest );

					QMutexLocker lk( &_mutex );
					QMutexLocker lk2( &dest->_mutex );

					dest->_readings = _readings;
				}

				void processScanEvent()
				{
					QMutexLocker lk( &_mutex );
					_rangefinder->scanEvent( _readings );
				}

				void setScan( const QVector< float > ranges, const QVector< float >& intensities, const QVariantMap& parameters )
				{
					QMutexLocker lk( &_mutex );
					_readings.seq = parameters["seq"].toUInt();
					_readings.stamp = parameters["stamp"].toUInt();
					
					QString frame_id = parameters["frame_id"].toString();
					if( false == frame_id.isEmpty() )
					{
						_readings.set_frame_id( frame_id.toLatin1().constData() );
					}

					_readings.angle_min = parameters["angle_min"].toFloat();
					_readings.angle_max = parameters["angle_max"].toFloat();
					_readings.angle_increment = parameters["angle_increment"].toFloat();
					_readings.time_increment = parameters["time_increment"].toFloat();
					_readings.scan_time = parameters["scan_time"].toFloat();
					_readings.range_min = parameters["range_min"].toFloat();
					_readings.range_max = parameters["range_max"].toFloat();

					if( ranges.isEmpty() )
					{
						_readings.clearRanges();
					}
					else
					{
						_readings.setRanges( ranges.constData(), ranges.size() );
					}

					if( intensities.isEmpty() )
					{
						_readings.clearIntensities();
					}
					else
					{
						_readings.setIntensities( intensities.constData(), intensities.size() );
					}
				}

				LaserRangeFinder* _rangefinder;
				int _number;

				mutable QMutex _mutex;

				LaserRangeFinderReadings _readings;
			};
		}
	}
}

#endif //_REC_ROBOTINO_API2_rangefinderIMPL_HPP_
