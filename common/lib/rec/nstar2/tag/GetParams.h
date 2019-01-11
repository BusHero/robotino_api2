#ifndef _REC_NSTAR2_TAG_GetParams_H_
#define _REC_NSTAR2_TAG_GetParams_H_

#include "rec/nstar2/tag/Tag.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class GetParams;
			typedef QSharedPointer<GetParams> GetParamsPointer;

			class GetParams : public Tag
			{
			public:
				static GetParamsPointer create()
				{
					return GetParamsPointer( new GetParams );
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
				GetParams()
					: Tag( TAG_GET_PARAMS )
				{
				}

				TagList _children;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_GetParams_H_
