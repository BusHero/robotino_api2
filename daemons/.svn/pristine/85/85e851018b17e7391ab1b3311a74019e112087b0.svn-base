#ifndef _ACTION_EmergencyBumper_H_
#define _ACTION_EmergencyBumper_H_

#include "action/Action.h"

namespace action
{
	class EmergencyBumper : public Action
	{
		Q_OBJECT
	public:
		EmergencyBumper( Action* parent );

		void customActivate( Action* before );

		QString title() const;

	private Q_SLOTS:
		void on_emergency_bumper_changed( bool );
	};
}

#endif //_ACTION_EmergencyBumper_H_
