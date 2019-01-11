#ifndef _REC_NSTAR2_TAG_MeasSpotPosition_H_
#define _REC_NSTAR2_TAG_MeasSpotPosition_H_

#include "rec/nstar2/tag/Tag.h"
#include "rec/nstar2/utils.h"
#include "rec/nstar2/Report.h"
#include "rec/nstar2/tag/MeasSpotPositionFwd.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class MeasSpotPosition : public Tag
			{
			public:
				class Readings
				{
				public:
					Readings()
						: frequency( 0 )
						, x( 0.0f )
						, y( 0.0f )
						, magnitude( 0 )
					{
					}

					int frequency;
					float x;
					float y;
					int magnitude;
				};

				typedef QList< Readings > ReadingsContainer;

				static MeasSpotPositionPointer create()
				{
					return MeasSpotPositionPointer( new MeasSpotPosition );
				}

				ReadingsContainer readings() const { return _readings; }

				QString print() const
				{
					QString str( "TAG_MEAS_SPOT_POSITION:\n   id         x         y   mag" );
					Q_FOREACH( const Readings& r, _readings )
					{
						str += QString( "\n%1 %2 %3 %4" ).arg( r.frequency, 5, 10 ).arg( (double)r.x, 9, 'f' ).arg( (double)r.y, 9, 'f' ).arg( r.magnitude, 5, 10 );
					}

					str += "\nPOSES:\n           x            y        theta";
					ReadingsContainer::const_iterator iter = _readings.constBegin();
					while( _readings.constEnd() != iter )
					{
						const Readings& r0 = *iter;
						++iter;
						if( _readings.constEnd() == iter )
						{
							break;
						}
						const Readings& r1 = *iter;
						++iter;

						float x,y,theta;
						Report::calculate_pose( r0.x, r0.y, r1.x, r1.y, 2, &x, &y, &theta );
						str += QString("\n %1  %2  %3").arg( (double)x, 11, 'f' ).arg( (double)y, 11, 'f' ).arg( (double)theta, 11, 'f' );
					}

					return str;
				}

				static MeasSpotPositionPointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 0 != (ch%8) )
						{
							throw TagException( QString( "MeasSpotPosition length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "MeasSpotPosition length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						MeasSpotPositionPointer MeasSpotPosition = create();
						const char* p = ba.constData();
						for( int i=0; i<ba.size(); i+=8 )
						{
							Readings r;
							r.frequency = rec::nstar2::decodeUInt16( p );
							p+=2;
							r.x = rec::nstar2::decodeFloat_int16( p );
							p+=2;
							r.y = rec::nstar2::decodeFloat_int16( p );
							p+=2;
							r.magnitude = rec::nstar2::decodeUInt16( p );
							p+=2;

							MeasSpotPosition->_readings << r;
						}
						return MeasSpotPosition;
					}
					else
					{
						throw TagException( QString( "Error getting length of MeasSpotPosition tag" ) );
					}
				}

			private:
				MeasSpotPosition()
					: Tag( TAG_MEAS_SPOT_POSITION )
				{
				}

				ReadingsContainer _readings;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_MeasSpotPosition_H_
