#ifndef _REC_ROBOTINO3_IOCOM_TAG_GyroData_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GyroData_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/GyroDataFwd.h"
#include "rec/robotino3/iocom/tag/tagenum.h"
#include "rec/robotino3/iocom/utils.h"


namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class GyroData : public rec::robotino3::serialio::Tag
				{
				public:
					static GyroDataPointer create()
					{
						return GyroDataPointer( new GyroData );
					}

					QString print() const
					{
						QString str =  "TAG_GYRODATA\n";
						for( int i=0; i<_omegas.size() && i<_stamps.size(); ++i )
						{
							str += QString("%1 | %2\n").arg( _stamps[i], 10, 10 ).arg( _omegas[i] );
						}

						return str;
					}

					static GyroDataPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( ch%8 != 0 )
							{
								throw rec::robotino3::serialio::TagException( QString( "GYRODATA length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "GYRODATA length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							GyroDataPointer GyroData = create();

							const char* p = ba.constData();
							for( int i=0; i<ba.size(); i+=8 )
							{
								unsigned int stamp = decodeUInt32( p );
								p += 4;
								float omega = decodeFloat( p );
								p += 4;

								GyroData->_stamps << stamp;
								GyroData->_omegas << omega;
							}
							
							return GyroData;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of GyroData tag" ) );
						}
					}

					const QVector<float>& omegas() const
					{
						return _omegas;
					}

					const QVector<unsigned int>& stamps() const
					{
						return _stamps;
					}

				private:
					GyroData()
						: Tag( TAG_GYRODATA )
					{
					}

					QVector<float> _omegas;
					QVector<unsigned int> _stamps;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GyroData_H_
