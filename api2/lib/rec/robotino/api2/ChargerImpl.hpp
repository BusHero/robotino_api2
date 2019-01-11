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

#ifndef _REC_ROBOTINO_API2_ChargerIMPL_HPP_
#define _REC_ROBOTINO_API2_ChargerIMPL_HPP_

#include "rec/robotino/api2/Charger.h"
#include "rec/robotino/api2/ChargerReader.hpp"

#include <QtCore>

namespace rec
{
	namespace robotino
	{
		namespace api2
		{
			class ChargerImpl : public ChargerReader
			{
			public:
				ChargerImpl( Charger* charger )
					: _charger( charger )
					, _number( -1 )
					, _time( 0 )
					, _batteryVoltage( 0 )
					, _chargingCurrent( 0 )
					, _bat1temp( 0 )
					, _bat2temp( 0 )
					, _state_number( 0 )
					, _major( 0 )
					, _minor( 0 )
					, _patch( 0 )
				{
				}

				~ChargerImpl()
				{
					QMutexLocker lk( &_mutex );
					_charger->_impl = NULL;
				}

				const char* state()
				{
					_stateStr = _state.toLatin1();
					return _stateStr.constData();
				}

				const char* errorMessage()
				{
					_errorMessageStr = _errorMessage.toLatin1();
					return _errorMessageStr.constData();
				}

				void processChargerInfoEvent()
				{
					QMutexLocker lk( &_mutex );
					_charger->chargerInfoChanged( _time, _batteryVoltage, _chargingCurrent, _bat1temp, _bat2temp, _state_number, state() );
				}

				void processChargerErrorEvent()
				{
					QMutexLocker lk( &_mutex );
					_charger->chargerErrorChanged( _time, errorMessage() );
				}

				void processChargerVersionEvent()
				{
					QMutexLocker lk( &_mutex );
					_charger->chargerVersionChanged( _major, _minor, _patch );
				}

				void setChargerInfo( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state )
				{
					QMutexLocker lk( &_mutex );
					_time = time;
					_batteryVoltage = batteryVoltage;
					_chargingCurrent = chargingCurrent;
					_bat1temp = bat1temp;
					_bat2temp = bat2temp;
					_state_number = state_number;
					_state = state;
				}

				void setChargerError( unsigned int time, const QString& message )
				{
					QMutexLocker lk( &_mutex );
					_time = time;
					_errorMessage = message;
				}

				void setChargerVersion( int major, int minor, int patch )
				{
					QMutexLocker lk( &_mutex );
					_major = major;
					_minor = minor;
					_patch = patch;
				}

				Charger* _charger;
				QMutex _mutex;

				int _number;
				unsigned int _time;
				float _batteryVoltage;
				float _chargingCurrent;
				float _bat1temp;
				float _bat2temp;
				int _state_number;
				QString _state;
				QByteArray _stateStr;
				QString _errorMessage;
				QByteArray _errorMessageStr;

				int _major;
				int _minor;
				int _patch;
			};
		}
	}
}

#endif //_REC_ROBOTINO_API2_ChargerIMPL_HPP_
