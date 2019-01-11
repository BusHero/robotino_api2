#ifndef _REC_ROBOTINO3_IOCOM_TAG_SetNrst_H_
#define _REC_ROBOTINO3_IOCOM_TAG_SetNrst_H_

#include "rec/robotino3/iocom/tag/SetNrstFwd.h"
#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class SetNrst : public rec::robotino3::serialio::Tag
				{
				public:
					static SetNrstPointer create( bool on )
					{
						return SetNrstPointer( new SetNrst( on ) );
					}

					QString print() const
					{
						return QString( "TAG_SET_NRST %1" ).arg( (_on?"on":"off") );
					}

					QByteArray encode() const
					{
						QByteArray ba( 3, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 1;
						ba[2] = ( _on?1:0 );
						return ba;
					}

					QString signature() const
					{
						return QString( "%1_%2" ).arg( value() ).arg( _on?1:0 );
					};


				private:
					SetNrst( bool on )
						: Tag( TAG_SET_NRST )
						, _on( on )
					{
					}

					bool _on;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_SetNrst_H_
