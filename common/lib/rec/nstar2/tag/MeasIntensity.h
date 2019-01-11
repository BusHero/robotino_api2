#ifndef _REC_NSTAR2_TAG_MeasIntensity_H_
#define _REC_NSTAR2_TAG_MeasIntensity_H_

#include "rec/nstar2/tag/Tag.h"
#include "rec/nstar2/utils.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class MeasIntensity;
			typedef QSharedPointer<MeasIntensity> MeasIntensityPointer;

			class MeasIntensity : public Tag
			{
			public:
				static MeasIntensityPointer create()
				{
					return MeasIntensityPointer( new MeasIntensity );
				}

				QList<float> intensities() const { return _intensities; }

				QString print() const
				{
					QString str( "TAG_MEAS_INTENSITY: [ " );
					Q_FOREACH( float f, _intensities )
					{
						str += QString( "%1 " ).arg( f );
					}
					str += "]";
					return str;
				}

				static MeasIntensityPointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 0 != (ch%4) )
						{
							throw TagException( QString( "MeasIntensity length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "MeasIntensity length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						MeasIntensityPointer MeasIntensity = create();
						const char* p = ba.constData();
						for( int i=0; i<ba.size(); i+=4 )
						{
							MeasIntensity->_intensities << decodeFloat( p );
							p+=4;
						}
						return MeasIntensity;
					}
					else
					{
						throw TagException( QString( "Error getting length of MeasIntensity tag" ) );
					}
				}

			private:
				MeasIntensity()
					: Tag( TAG_MEAS_INTENSITY )
				{
				}

				QList<float> _intensities;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_MeasIntensity_H_
