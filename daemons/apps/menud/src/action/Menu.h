#ifndef _ACTION_Menu_H_
#define _ACTION_Menu_H_

#include "action/Action.h"

namespace action
{
	class Menu : public action::Action
	{
		Q_OBJECT
	public:
		Menu( Action* parent );
		virtual ~Menu();

		void drawDisplay();

	protected:
		virtual void customActivate( Action* before );
		virtual void customLeave( Action* next );

		QString _customLine;

		/* first item of menu shown */
		unsigned int _firstItem;
		/* number of item currently selected */
		unsigned int _selectedItem;

	private:
		void back();
		void enter();
		void up();
		void down();

		QList<Action*> _visibleChildren;
	};
}

#endif //_ACTION_Menu_H_
