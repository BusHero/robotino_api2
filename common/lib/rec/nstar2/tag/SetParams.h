#ifndef _REC_NSTAR2_TAG_SetParams_H_
#define _REC_NSTAR2_TAG_SetParams_H_

#include "rec/nstar2/tag/Tag.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class SetParams;
			typedef QSharedPointer<SetParams> SetParamsPointer;

			class SetParams : public Tag
			{
			public:
				static SetParamsPointer create()
				{
					return SetParamsPointer( new SetParams );
				}

				QByteArray encode() const
				{
					QByteArray body;
					Q_FOREACH( TagPointer child, _children )
					{
						body.append( child->encode() );
					}

					QByteArray header( 2, 0 );
					header[0] = static_cast<quint8>( value() );
					header[1] = static_cast<quint8>( body.size() );

					return header + body;
				}

				Tag& operator<<( TagPointer child )
				{
					_children << child;
					return *this;
				}

				TagList children() const
				{
					return _children;
				}

			private:
				SetParams()
					: Tag( TAG_SET_PARAMS )
				{
				}

				TagList _children;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_SetParams_H_
