#ifndef _ACTION_ComboBox_H_
#define _ACTION_ComboBox_H_

#include "action/Action.h"

namespace action
{
	class ComboBox : public Action
	{
		Q_OBJECT
	public:
		ComboBox( Action* parent );

		void customActivate( Action* before );

	private:
		void drawDisplay();

		void back();
		void enter();
		void up();
		void down();

		/* first item of menu shown */
		unsigned int _firstItem;
		/* number of item currently selected */
		unsigned int _selectedItem;

		QStringList _items;
	};
}

#endif //_ACTION_ComboBox_H_
