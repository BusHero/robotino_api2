#ifndef _ACTION_RequestShutdown_H_
#define _ACTION_RequestShutdown_H_

#include "action/Action.h"

#include <QHostAddress>

namespace action
{
	class RequestShutdown : public action::Action
	{
		Q_OBJECT
	public:
		RequestShutdown( Action* parent );

		void customActivate( Action* before );
		void customLeave( Action* next );

	private Q_SLOTS:
		void on_timer_timeout();
		void on_power_button_changed( bool );
		void on_request_shutdown_changed( bool );

	private:
		QTimer* _timer;
		int _countdown;
	};
}

#endif //_ACTION_RequestShutdown_H_
