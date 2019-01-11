#ifndef _REC_NSTAR2_TAG_MeasAmbientLightLevel_H_
#define _REC_NSTAR2_TAG_MeasAmbientLightLevel_H_

#include "rec/nstar2/tag/Tag.h"
#include "rec/nstar2/utils.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class MeasAmbientLightLevel;
			typedef QSharedPointer<MeasAmbientLightLevel> MeasAmbientLightLevelPointer;

			class MeasAmbientLightLevel : public Tag
			{
			public:
				static MeasAmbientLightLevelPointer create()
				{
					return MeasAmbientLightLevelPointer( new MeasAmbientLightLevel );
				}

				int ambientLight() const { return _ambientLight; }

				QString print() const
				{
					return QString( "TAG_MEAS_AMBIENT_LIGHT_LEVEL: %1" ).arg( ambientLight() );
				}

				static MeasAmbientLightLevelPointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 2 != ch )
						{
							throw TagException( QString( "MeasAmbientLightLevel length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "MeasAmbientLightLevel length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						MeasAmbientLightLevelPointer MeasAmbientLightLevel = create();
						MeasAmbientLightLevel->_ambientLight = decodeUInt16( ba );
						return MeasAmbientLightLevel;
					}
					else
					{
						throw TagException( QString( "Error getting length of MeasAmbientLightLevel tag" ) );
					}
				}

			private:
				MeasAmbientLightLevel()
					: Tag( TAG_MEAS_AMBIENT_LIGHT_LEVEL )
					, _ambientLight( 0 )
				{
				}

				int _ambientLight;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_MeasAmbientLightLevel_H_
