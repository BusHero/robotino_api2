#ifndef _ACTION_LineEdit_H_
#define _ACTION_LineEdit_H_

#include "action/Action.h"

namespace action
{
	class LineEdit : public Action
	{
		Q_OBJECT
	public:
		LineEdit( Action* parent );

		void customActivate( Action* before );

	private:
		void drawDisplay();

		QChar inc( const QChar ch ) const;
		QChar dec( const QChar ch ) const;

		void back();
		void enter();
		void up();
		void down();

		// current edit position
		unsigned int _editPos;

		QString _newValue;
	};
}

#endif //_ACTION_LineEdit_H_
