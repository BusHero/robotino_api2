#ifndef _REC_NSTAR2_TAG_ProtocolVersion_H_
#define _REC_NSTAR2_TAG_ProtocolVersion_H_

#include "rec/nstar2/tag/Tag.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class ProtocolVersion;
			typedef QSharedPointer<ProtocolVersion> ProtocolVersionPointer;

			class ProtocolVersion : public Tag
			{
			public:
				static ProtocolVersionPointer create()
				{
					return ProtocolVersionPointer( new ProtocolVersion );
				}

				QByteArray encode() const
				{
					QByteArray ba( 2, 0 );
					ba[0] = static_cast<quint8>( value() );
					return ba;
				}

			private:
				ProtocolVersion()
					: Tag( TAG_PROTOCOL_VERSION )
				{
				}
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_ProtocolVersion_H_
