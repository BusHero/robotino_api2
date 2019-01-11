#ifndef _REC_ROBOTINO3_IOCOM_TAG_CanMsg_H_
#define _REC_ROBOTINO3_IOCOM_TAG_CanMsg_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/CanMsgFwd.h"
#include "rec/robotino3/iocom/utils.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class CanMsg : public rec::robotino3::serialio::Tag
				{
				public:
					static CanMsgPointer create()
					{
						return CanMsgPointer( new CanMsg );
					}

					static CanMsgPointer create( unsigned short id, unsigned char dlc, const unsigned char* data )
					{
						return CanMsgPointer( new CanMsg( id, dlc, data ) );
					}

					QString print() const
					{
						QString str = QString( "CAN_MSG: ID = 0x%1 DLC = %2 Data =" ).arg( _id, 3, 16, QChar( '0' ) ).arg( _dlc );
						for( int i = 0; i < _dlc; ++i )
						{
							str.append( QString( " 0x%1" ).arg( _data[i], 2, 16, QChar( '0' ) ) );
						}
						return str;
					}

					static CanMsgPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 11 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "CanMsg length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "CanMsg length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							CanMsgPointer canMsg = create();

							canMsg->_id = decodeUInt16( ba.constData() );
							canMsg->_dlc = ba[2];
							if ( canMsg->_dlc > 8 )
								canMsg->_dlc = 8;
							memcpy( canMsg->_data, ba.constData() + 3, 8 );
							return canMsg;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of CanMsg tag" ) );
						}
					}

					QByteArray encode() const
					{
						QByteArray ba( 13, 0 );
						ba[0] = static_cast< quint8 >( value() );
						ba[1] = 11;
						encodeUInt16( ba.data() + 2, _id );
						ba[4] = _dlc;
						memcpy( ba.data() + 5, _data, 8 );
						return ba;
					}

					unsigned short id() const
					{
						return _id;
					}

					unsigned char dlc() const
					{
						return _dlc;
					}

					const unsigned char* data() const
					{
						return _data;
					}

				private:
					CanMsg()
						: Tag( TAG_CAN_MSG )
						, _id( 0 )
						, _dlc( 0 )
					{
						memset( _data, 0, 8 );
					}

					CanMsg( unsigned short id, unsigned char dlc, const unsigned char* data )
						: Tag( TAG_CAN_MSG )
						, _id( id )
						, _dlc( dlc )
					{
						if ( _id > 0x7FF )
							_id = 0x7FF;
						if ( _dlc > 8 )
							_dlc = 8;
						memset( _data, 0, 8 );
						memcpy( _data, data, _dlc );
					}

					unsigned short _id;
					unsigned char _dlc;
					unsigned char _data[8];
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_CanMsg_H_
