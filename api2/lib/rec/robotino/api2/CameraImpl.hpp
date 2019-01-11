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

#ifndef _REC_ROBOTINO_API2_CAMERAIMPL_HPP_
#define _REC_ROBOTINO_API2_CAMERAIMPL_HPP_

#include "rec/robotino/api2/Camera.h"
#include "rec/robotino/api2/CameraReader.hpp"
#include "rec/robotino/api2/CameraCapabilities.h"
#include "rec/cv_lt/yuyv2rgb.h"
#include "rec/cv_lt/jpeg.h"
#include "rec/cv_lt/rgb2bgr.h"

#include <QtCore>

namespace rec
{
	namespace robotino
	{
		namespace api2
		{
			class CameraImpl : public CameraReader
			{
			public:
				CameraImpl( Camera* camera )
					: _camera( camera )
					, _mutex( QMutex::Recursive )
					, _number( 0 )
					, _isNewImageAvailable( false )
					, _width( 0 )
					, _height( 0 )
					, _step( 0 )
					, _settings_width( 0 )
					, _settings_height( 0 )
					, _bgrEnabled( false )
					, _jpgDecodingEnabled( true )
				{
				}

				~CameraImpl()
				{
					QMutexLocker lk( &_mutex );
					_camera->_impl = NULL;
				}

				void clone( CameraReader* destination ) const
				{
					CameraImpl* dest = dynamic_cast<CameraImpl*>( destination );
					Q_ASSERT( dest );

					QMutexLocker lk( &_mutex );
					QMutexLocker lk2( &dest->_mutex );

					dest->_number = _number;
					dest->_width = _width;
					dest->_height = _height;
					dest->_step = _step;
					dest->_data = _data;

					dest->_settings_width = _settings_width;
					dest->_settings_height = _settings_height;
					dest->_settings_format = _settings_format;

					dest->_capabilities = _capabilities;

					dest->_calibration = _calibration;
				}

				void processImageEvent()
				{
					QMutexLocker lk( &_mutex );
					if (0 == _width && 0 == _height && 0 == _step)
					{
						_camera->imageReceivedEvent((unsigned char*)_data.constData(), (unsigned int)_data.size(), _width, _height, _step);
					}
					else
					{
						_camera->imageReceivedEvent((unsigned char*)_data.constData(), _step*_height, _width, _height, _step);
					}
				}

				void processCapabilitiesEvent()
				{
					QMutexLocker lk( &_mutex );
					_camera->capabilitiesChangedEvent( _capabilities );
				}

				void processSettingsEvent()
				{
					QMutexLocker lk( &_mutex );
					_camera->settingsChangedEvent( _settings_width, _settings_height, _settings_format.c_str() );
				}

				void processCalibrationEvent()
				{
					QMutexLocker lk( &_mutex );
					_camera->calibrationChanged( _calibration.constData(), _calibration.size() );
				}

				void setImage( const QByteArray& data,
									unsigned int width,
									unsigned int height,
									unsigned int step,
									const QString& format )
				{
					QMutexLocker lk( &_mutex );

					if( "yuyv" == format )
					{
						_data.resize( height*width*3 );
						_step = width*3;
						_width = width;
						_height = height;

						if( _bgrEnabled )
						{
							rec::cv_lt::yuyv2bgr( data.constData(),
								width,
								height,
								step,
								_data.data(),
								_step
								);
						}
						else
						{
							rec::cv_lt::yuyv2rgb( data.constData(),
								width,
								height,
								step,
								_data.data(),
								_step
								);
						}
						
						_isNewImageAvailable = true;
					}
					if( "rgb24" == format )
					{
						_data = data;
						_width = width;
						_height = height;
						_step= step;

						if( _bgrEnabled )
						{
							rec::cv_lt::rgb2bgr( _data.data(), _width, _height, _step );
						}
						
						_isNewImageAvailable = true;
					}
					if( "rgb32" == format )
					{
					}
					else if( format.contains( "jpg" ) || format.contains( "jpeg" ) )
					{
						if (_jpgDecodingEnabled)
						{
							_data.resize(1024 * 768 * 3);
							unsigned int numChannels;
							if (rec::cv_lt::jpg_decompress(data.constData(), data.size(), _data.data(), _data.size(), &_width, &_height, &numChannels))
							{
								_step = _width * 3;
								_isNewImageAvailable = true;

								if (_bgrEnabled)
								{
									rec::cv_lt::rgb2bgr(_data.data(), _width, _height, _step);
								}
							}
						}
						else
						{
							_data = data;
							_width = 0;
							_height = 0;
							_step = _data.size();
							
							_isNewImageAvailable = true;
						}
					}
				}

				void setCapabilities( const QString& cameraName, const QMap<QString, QVector<QSize> >& capabilities, const QStringList& controls )
				{
					QMutexLocker lk( &_mutex );
					_capabilities = CameraCapabilities();

					_capabilities.setName( cameraName.toLatin1().constData() );

					QMap<QString, QVector<QSize> >::const_iterator iter = capabilities.constBegin();
					while( capabilities.constEnd() != iter )
					{
						std::string name = iter.key().toLatin1().constData();

						const QVector<QSize>& sizes = iter.value();
						for( int j=0; j<sizes.size(); ++j )
						{
							_capabilities.addFormat( CameraFormat( sizes[j].width(), sizes[j].height(), name.c_str() ) );
						}
						++iter;
					}

					Q_FOREACH( const QString& str, controls )
					{
						if( "brightness" == str )
						{
							_capabilities.brightness = true;
						}
						else if( "contrast" == str )
						{
							_capabilities.contrast = true;
						}
						else if( "saturation" == str )
						{
							_capabilities.saturation = true;
						}
						else if( "autoWhiteBalance" == str )
						{
							_capabilities.autoWhiteBalance = true;
						}
						else if( "gain" == str )
						{
							_capabilities.gain = true;
						}
						else if( "whiteBalanceTemperature" == str )
						{
							_capabilities.whiteBalanceTemperature = true;
						}
						else if( "backlightCompensation" == str )
						{
							_capabilities.backlightCompensation = true;
						}
						else if( "autoExposure" == str )
						{
							_capabilities.autoExposure = true;
						}
						else if( "exposure" == str )
						{
							_capabilities.exposure = true;
						}
						else if( "autoFocus" == str )
						{
							_capabilities.autoFocus = true;
						}
						else if( "focus" == str )
						{
							_capabilities.focus = true;
						}
						else if( "sharpness" == str )
						{
							_capabilities.sharpness = true;
						}
					}
				}

				void setSettings( unsigned int width, unsigned int height, const QString& format )
				{
					QMutexLocker lk( &_mutex );
					_settings_width = width;
					_settings_height = height;
					_settings_format = format.toLatin1().constData();
				}

				void setCalibration( const QVector<double>& data )
				{
					QMutexLocker lk( &_mutex );
					_calibration = data;
				}
				
				Camera* _camera;
				mutable QMutex _mutex;

				int _number;

				bool _isNewImageAvailable;
				CameraCapabilities _capabilities;

				QByteArray _data;
				unsigned int _width;
				unsigned int _height;
				unsigned int _step;

				unsigned int _settings_width;
				unsigned int _settings_height;
				std::string _settings_format;

				QVector<double> _calibration;

				bool _bgrEnabled;
				bool _jpgDecodingEnabled;
			};
		}
	}
}

#endif //_REC_ROBOTINO_API2_CAMERAIMPL_HPP_
