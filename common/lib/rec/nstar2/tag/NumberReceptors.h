#ifndef _REC_NSTAR2_TAG_NumberReceptors_H_
#define _REC_NSTAR2_TAG_NumberReceptors_H_

#include "rec/nstar2/tag/Tag.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class NumberReceptors;
			typedef QSharedPointer<NumberReceptors> NumberReceptorsPointer;

			class NumberReceptors : public Tag
			{
			public:
				static NumberReceptorsPointer create()
				{
					return NumberReceptorsPointer( new NumberReceptors );
				}

				int numReceptors() const { return _numReceptors; }

				QString print() const
				{
					return QString( "TAG_NUMBER_RECEPTORS: %1" ).arg( numReceptors() );
				}

				QByteArray encode() const
				{
					QByteArray ba( 2, 0 );
					ba[0] = static_cast<quint8>( value() );
					return ba;
				}

				static NumberReceptorsPointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 1 != ch )
						{
							throw TagException( QString( "NumberReceptors length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "NumberReceptors length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						NumberReceptorsPointer numberReceptors = create();
						numberReceptors->_numReceptors = static_cast<int>( ba[0] );
						return numberReceptors;
					}
					else
					{
						throw TagException( QString( "Error getting length of NumberReceptors tag" ) );
					}
				}

			private:
				NumberReceptors()
					: Tag( TAG_NUMBER_RECEPTORS )
					, _numReceptors( 0 )
				{
				}

				int _numReceptors;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_NumberReceptors_H_
