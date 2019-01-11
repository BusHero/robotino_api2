#ifndef _REC_NSTAR2_TAG_ReportAuto_H_
#define _REC_NSTAR2_TAG_ReportAuto_H_

#include "rec/nstar2/tag/Tag.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class ReportAuto;
			typedef QSharedPointer<ReportAuto> ReportAutoPointer;

			class ReportAuto : public Tag
			{
			public:
				static ReportAutoPointer create( bool enable )
				{
					return ReportAutoPointer( new ReportAuto( enable ) );
				}

				QByteArray encode() const
				{
					if( _enable )
					{
						QByteArray ba( 3, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 1;
						ba[2] = 1;
						return ba;
					}
					else
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = ( _enable ? 1 : 0 );
						return ba;
					}
				}

			private:
				ReportAuto( bool enable )
					: Tag( TAG_REPORT_AUTO )
					, _enable( enable )
				{
				}

				bool _enable;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_ReportAuto_H_
