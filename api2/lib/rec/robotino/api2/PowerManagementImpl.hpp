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

#ifndef _REC_ROBOTINO_API2_PowerManagementIMPL_HPP_
#define _REC_ROBOTINO_API2_PowerManagementIMPL_HPP_

#include "rec/robotino/api2/PowerManagement.h"
#include "rec/robotino/api2/PowerManagementReader.hpp"

#include <QtCore>

namespace rec
{
	namespace robotino
	{
		namespace api2
		{
			class PowerManagementImpl : public PowerManagementReader
			{
			public:
				PowerManagementImpl( PowerManagement* powerManagement )
					: _powerManagement( powerManagement )
					, _mutex( QMutex::Recursive )
					, _current( 0.0f )
					, _voltage( 0.0f )
					, _ext_power( false )
					, _num_chargers( 0 )
					, _batteryType( "Pb" )
					, _batteryLow( false )
					, _batteryLowShutdownCounter( 0 )
				{
				}

				~PowerManagementImpl()
				{
					QMutexLocker lk( &_mutex );
					_powerManagement->_impl = NULL;
				}

				const char* batteryType()
				{
					_batteryTypeStr = _batteryType.toLatin1();
					return _batteryTypeStr.constData();
				}

				void processPowerManagementEvent()
				{
					QMutexLocker lk( &_mutex );
					_powerManagement->readingsEvent( _voltage, _current, _ext_power, _num_chargers, batteryType(), _batteryLow, _batteryLowShutdownCounter );
				}

				void setBattery( float battery_voltage, float system_current, bool ext_power, int num_chargers, const QString& batteryType, bool batteryLow, int batteryLowShutdownCounter )
				{
					QMutexLocker lk( &_mutex );
					_voltage = battery_voltage;
					_current = system_current;
					_ext_power = ext_power;
					_num_chargers = num_chargers;
					_batteryType = batteryType;
					_batteryLow = batteryLow;
					_batteryLowShutdownCounter = batteryLowShutdownCounter;
				}

				PowerManagement* _powerManagement;
				QMutex _mutex;

				float _current;
				float _voltage;
				bool _ext_power;
				int _num_chargers;
				QString _batteryType;
				bool _batteryLow;
				int _batteryLowShutdownCounter;
				QByteArray _batteryTypeStr;
			};
		}
	}
}

#endif //_REC_ROBOTINO_API2_PowerManagementIMPL_HPP_
