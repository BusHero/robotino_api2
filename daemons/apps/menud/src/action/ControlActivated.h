#ifndef _ACTION_ControlActivated_H_
#define _ACTION_ControlActivated_H_

#include "action/Action.h"

#include <QHostAddress>

namespace action
{
	class ControlActivated : public action::Action
	{
		Q_OBJECT
	public:
		ControlActivated( Action* parent );

		void customActivate( Action* before );
		void customLeave( Action* next );

	private Q_SLOTS:
		void on_timer_timeout();
		void on_addressTimer_timeout();

	private:
		void back();
		void enter();
		void up();
		void down();

		QTimer* _timer;
		QTimer* _addressTimer;
		unsigned int _step;
		unsigned int _addressStep;
	};
}

#endif //_ACTION_ControlActivated_H_
