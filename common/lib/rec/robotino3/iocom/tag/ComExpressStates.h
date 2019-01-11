#ifndef _REC_ROBOTINO3_IOCOM_TAG_ComExpressStates_H_
#define _REC_ROBOTINO3_IOCOM_TAG_ComExpressStates_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/ComExpressStatesFwd.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class ComExpressStates : public rec::robotino3::serialio::Tag
				{
				public:
					static ComExpressStatesPointer create()
					{
						return ComExpressStatesPointer( new ComExpressStates );
					}

					QString print() const
					{
						return QString( "TAG_SUS_STATES"
							"\nSUS_S3#  :%1"
							"\nSUS_S4# : %2"
							"\nSUS_S5# : %3"
							"\nTHRM# : %4"
							"\nTHRMTRIP# : %5"
							)
							.arg( (_sus_s3?"High":"Low") )
							.arg( (_sus_s4?"High":"Low") )
							.arg( (_sus_s5?"High":"Low") )
							.arg( (_thrm?"High":"Low") )
							.arg( (_thrmtrip?"High":"Low") )
							;
					}

					static ComExpressStatesPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 5 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "ComExpressStates length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "ComExpressStates length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							ComExpressStatesPointer ComExpressStates = create();
							ComExpressStates->_sus_s3 = ( (unsigned char)ba[0] != 0 );
							ComExpressStates->_sus_s4 = ( (unsigned char)ba[1] != 0 );
							ComExpressStates->_sus_s5 = ( (unsigned char)ba[2] != 0 );
							ComExpressStates->_thrm = ( (unsigned char)ba[3] != 0 );
							ComExpressStates->_thrmtrip = ( (unsigned char)ba[4] != 0 );
							return ComExpressStates;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of ComExpressStates tag" ) );
						}
					}

					bool sus_s3() const
					{
						return _sus_s3;
					}

					bool sus_s4() const
					{
						return _sus_s4;
					}

					bool sus_s5() const
					{
						return _sus_s5;
					}

					bool thrm() const
					{
						return _thrm;
					}

					bool thrmtrip() const
					{
						return _thrmtrip;
					}

				private:
					ComExpressStates()
						: Tag( TAG_COM_EXPRESS_STATES )
						, _sus_s3( false )
						, _sus_s4( false )
						, _sus_s5( false )
						, _thrm( false )
						, _thrmtrip( false )
					{
					}

					bool _sus_s3;
					bool _sus_s4;
					bool _sus_s5;
					bool _thrm;
					bool _thrmtrip;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_ComExpressStates_H_
