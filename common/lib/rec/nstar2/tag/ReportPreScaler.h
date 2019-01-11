#ifndef _REC_NSTAR2_TAG_ReportPreScaler_H_
#define _REC_NSTAR2_TAG_ReportPreScaler_H_

#include "rec/nstar2/tag/Tag.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class ReportPreScaler;
			typedef QSharedPointer<ReportPreScaler> ReportPreScalerPointer;

			class ReportPreScaler : public Tag
			{
			public:
				static ReportPreScalerPointer create()
				{
					return ReportPreScalerPointer( new ReportPreScaler );
				}

				int preScaler() const { return _preScaler; }

				QString print() const
				{
					return QString( "TAG_REPORT_PRE_SCALER: %1" ).arg( preScaler() );
				}

				QByteArray encode() const
				{
					QByteArray ba( 2, 0 );
					ba[0] = static_cast<quint8>( value() );
					return ba;
				}

				static ReportPreScalerPointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 1 != ch )
						{
							throw TagException( QString( "ReportPreScaler length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "ReportPreScaler length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						ReportPreScalerPointer ReportPreScaler = create();
						ReportPreScaler->_preScaler = static_cast<int>( ba[0] );
						return ReportPreScaler;
					}
					else
					{
						throw TagException( QString( "Error getting length of ReportPreScaler tag" ) );
					}
				}

			private:
				ReportPreScaler()
					: Tag( TAG_REPORT_PRE_SCALER )
					, _preScaler( 0 )
				{
				}

				int _preScaler;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_ReportPreScaler_H_
