#ifndef _ACTION_STATUSSCREEN_H_
#define _ACTION_STATUSSCREEN_H_

#include "action/Action.h"

namespace action
{
	class StatusScreen : public action::Action
	{
		Q_OBJECT
	public:
		StatusScreen( Action* parent );

		void customActivate( Action* before );
		void customLeave( Action* next );

		void enter();

	private Q_SLOTS:
		void on_addressTimer_timeout();
		void on_battery_changed( float battery_voltage, float system_current );

	private:
		QStringList _ip;
		QTimer* _addressTimer;
		unsigned int _addressStep;
		int _allnetApSearchCount;
	};
}

#endif //_ACTION_STATUSSCREEN_H_
