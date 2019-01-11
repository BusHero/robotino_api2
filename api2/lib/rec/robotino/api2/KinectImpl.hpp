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

#ifndef _REC_ROBOTINO_API2_KINECTIMPL_HPP_
#define _REC_ROBOTINO_API2_KINECTIMPL_HPP_

#include "rec/robotino/api2/Kinect.h"
#include "rec/robotino/api2/KinectReader.hpp"
#include "rec/cv_lt/yuyv2rgb.h"
#include "rec/cv_lt/rgb2bgr.h"

#include <QtCore>

namespace rec
{
	namespace robotino
	{
		namespace api2
		{
			class KinectImpl : public KinectReader
			{
			public:
				KinectImpl( Kinect* Kinect )
					: _kinect( Kinect )
					, _mutex( QMutex::Recursive )
					, _number( 0 )
					, _isNewDepthAvailable( false )
					, _depthWidth( 0 )
					, _depthHeight( 0 )
					, _depthFormat( 0 )
					, _depthStamp( 0 )
					, _isNewVideoAvailable( false )
					, _videoWidth( 0 )
					, _videoHeight( 0 )
					, _videoStep( 0 )
					, _videoFormat( 0 )
					, _videoStamp( 0 )
					, _tilt( 0.0 )
					, _accelX( 0.0 )
					, _accelY( 0.0 )
					, _accelZ( 0.0 )
					, _bgrEnabled( false )
				{
				}

				~KinectImpl()
				{
					QMutexLocker lk( &_mutex );
					_kinect->_impl = NULL;
				}

				void processVideoEvent()
				{
					QMutexLocker lk( &_mutex );
					_kinect->videoEvent( (unsigned char*)_videoData.constData(), _videoData.size(), _videoWidth, _videoHeight, _videoStep, _videoFormat, _videoStamp );
				}

				void processDepthEvent()
				{
					QMutexLocker lk( &_mutex );
					_kinect->depthEvent( (const unsigned short*)_depthData.constData(), _depthData.size() / 2, (const unsigned short*)_object_data.constData(), _object_data.size() / 2, _depthWidth, _depthHeight, _depthFormat, _depthStamp );
				}

				void processTiltEvent()
				{
					QMutexLocker lk( &_mutex );
					_kinect->tiltEvent( _tilt );
				}

				void processAccelEvent()
				{
					QMutexLocker lk( &_mutex );
					_kinect->accelEvent( _accelX, _accelY, _accelZ );
				}


				void setVideo( const QByteArray& data,
									unsigned int width,
									unsigned int height,
									unsigned int format,
									unsigned int stamp )
				{
					QMutexLocker lk( &_mutex );
					_isNewVideoAvailable = true;

					_videoData = data;
					_videoStep = width*3;
					_videoWidth = width;
					_videoHeight = height;

					if( _bgrEnabled )
					{
						rec::cv_lt::rgb2bgr( _videoData.data(), _videoWidth, _videoHeight, _videoStep );
					}
				}

				void setDepth( const QByteArray& data,
								const QByteArray& object_data,
									unsigned int width,
									unsigned int height,
									unsigned int format,
									unsigned int stamp )
				{
					QMutexLocker lk( &_mutex );
					_isNewDepthAvailable = true;

					_depthData = data;
					_object_data = object_data;
					_depthWidth = width;
					_depthHeight = height;
					_depthFormat = format;
					_depthStamp = stamp;
				}

				void setTilt( double angleDeg )
				{
					QMutexLocker lk( &_mutex );
					_tilt = angleDeg;
				}

				void setAccel( double x, double y, double z )
				{
					QMutexLocker lk( &_mutex );
					_accelX = x;
					_accelY = y;
					_accelZ = z;
				}
				
				Kinect* _kinect;
				mutable QMutex _mutex;

				int _number;

				bool _isNewDepthAvailable;
				bool _isNewVideoAvailable;

				QByteArray _depthData;
				QByteArray _object_data;
				unsigned int _depthWidth;
				unsigned int _depthHeight;
				unsigned int _depthFormat;
				unsigned int _depthStamp;

				QByteArray _videoData;
				unsigned int _videoWidth;
				unsigned int _videoHeight;
				unsigned int _videoStep;
				unsigned int _videoFormat;
				unsigned int _videoStamp;

				double _tilt;
				double _accelX;
				double _accelY;
				double _accelZ;

				bool _bgrEnabled;
			};
		}
	}
}

#endif //_REC_ROBOTINO_API2_kinectIMPL_HPP_
