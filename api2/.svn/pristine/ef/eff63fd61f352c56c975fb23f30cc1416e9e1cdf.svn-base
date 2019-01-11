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

#ifndef _REC_ROBOTINO_API2_MotorDebugIMPL_HPP_
#define _REC_ROBOTINO_API2_MotorDebugIMPL_HPP_

#include "rec/robotino/api2/MotorDebug.h"
#include "rec/robotino/api2/MotorDebugReader.hpp"

#include <QtCore>

namespace rec
{
	namespace robotino
	{
		namespace api2
		{
			class MotorDebugImpl : public MotorDebugReader
			{
			public:
				MotorDebugImpl( MotorDebug* MotorDebug )
					: _MotorDebug( MotorDebug )
					, _mutex( QMutex::Recursive )
				{
				}

				~MotorDebugImpl()
				{
					QMutexLocker lk( &_mutex );
					_MotorDebug->_impl = NULL;
				}

				void processMotorDebugEvent()
				{
					QMutexLocker lk( &_mutex );
					if (_times.size() > 0
						&& _times.size() == _speed_setpoints.size()
						&& _times.size() == _speeds.size()
						&& _times.size() == _currents.size()
						&& _times.size() == _control_points.size())
					{
						_MotorDebug->motorDebugEvent(_motor, _times.constData(), _speed_setpoints.constData(), _speeds.constData(), _currents.constData(), _control_points.constData(), _times.size());

						_times.clear();
						_speed_setpoints.clear();
						_speeds.clear();
						_currents.clear();
						_control_points.clear();
					}
				}

				void setMotorDebug(int motor, const QVector<unsigned int>& times, const QVector<float>& speed_setpoints, const QVector<float>& speeds, const QVector<float>& currents, const QVector<float>& control_points)
				{
					QMutexLocker lk( &_mutex );

					if (motor != _motor || _times.size() > 200)
					{
						_times.clear();
						_speed_setpoints.clear();
						_speeds.clear();
						_currents.clear();
						_control_points.clear();
					}

					_motor = motor;
					_times += times;
					_speed_setpoints += speed_setpoints;
					_speeds += speeds;
					_currents += currents;
					_control_points += control_points;
				}

				MotorDebug* _MotorDebug;
				QMutex _mutex;

				int _motor;
				QVector<unsigned int> _times;
				QVector<float> _speed_setpoints;
				QVector<float> _speeds;
				QVector<float> _currents;
				QVector<float> _control_points;
			};
		}
	}
}

#endif //_REC_ROBOTINO_API2_MotorDebugIMPL_HPP_
