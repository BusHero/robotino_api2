#ifndef _REC_NSTAR2_TAG_Root_H_
#define _REC_NSTAR2_TAG_Root_H_

#include "rec/nstar2/tag/Tag.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class Root;
			typedef QSharedPointer<Root> RootPointer;

			class Root : public Tag
			{
			public:
				static RootPointer create()
				{
					return RootPointer( new Root );
				}

				bool isRoot() const { return true; }

				QString print() const
				{
					QString str;
					Q_FOREACH( TagPointer p, _children )
					{
						str += '\n' + p->print();
					}
					return "ROOT:" + str;
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
				Root()
					: Tag( TAG_ROOT )
				{
				}

				TagList _children;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_Root_H_
