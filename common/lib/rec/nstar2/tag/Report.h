#ifndef _REC_NSTAR2_TAG_Report_H_
#define _REC_NSTAR2_TAG_Report_H_

#include "rec/nstar2/tag/Tag.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class Report;
			typedef QSharedPointer<Report> ReportPointer;

			class Report : public Tag
			{
			public:
				static ReportPointer create()
				{
					return ReportPointer( new Report );
				}

				QString print() const
				{
					QString str;
					Q_FOREACH( TagPointer p, _children )
					{
						str += '\n' + p->print();
					}
					str.replace( "\n", "\n    " );

					return "TAG_REPORT:" + str;
				}

				QByteArray encode() const
				{
					QByteArray ba( 2, 0 );
					ba[0] = static_cast<quint8>( value() );
					return ba;
				}

				static ReportPointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 0 == ch )
						{
							throw TagException( QString( "Report length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "Report length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						ReportPointer report = create();
						report->_children << Tag::decode( ba );
						return report;
					}
					else
					{
						throw TagException( QString( "Error getting length of Report tag" ) );
					}
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
				Report()
					: Tag( TAG_REPORT )
				{
				}

				TagList _children;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_Report_H_
