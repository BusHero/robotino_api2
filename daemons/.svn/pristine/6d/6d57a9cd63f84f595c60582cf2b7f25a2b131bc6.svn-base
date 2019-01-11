#ifndef _ACTION_FACTORY_H_
#define _ACTION_FACTORY_H_

#include <QtCore>
#include <QtXml>

class Client;

namespace action
{
	class Action;

	class Factory : public QObject
	{
		Q_OBJECT
	public:
		Factory();

		void load();

	Q_SIGNALS:
		void log( const QString&, int );

	private:
		action::Action* loadAction( QDomElement& e, action::Action* parent );
	};
}

#endif //_ACTION_FACTORY_H_
