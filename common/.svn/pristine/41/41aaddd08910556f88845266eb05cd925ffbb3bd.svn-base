#ifndef _REC_NSTAR2_TAG_WAITNEXTMEAS_H_
#define _REC_NSTAR2_TAG_WAITNEXTMEAS_H_

#include "rec/nstar2/tag/Tag.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class WaitNextMeas;
			typedef QSharedPointer<WaitNextMeas> WaitNextMeasPointer;

			class WaitNextMeas : public Tag
			{
			public:
				static WaitNextMeasPointer create()
				{
					return WaitNextMeasPointer( new WaitNextMeas );
				}

				QByteArray encode() const
				{
					QByteArray ba( 2, 0 );
					ba[0] = static_cast<quint8>( value() );
					return ba;
				}

			private:
				WaitNextMeas()
					: Tag( TAG_WAIT_NEXT_MEAS )
				{
				}
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_WAITNEXTMEAS_H_
