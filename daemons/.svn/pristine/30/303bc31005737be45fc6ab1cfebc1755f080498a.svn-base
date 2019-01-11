#ifndef _ACTION_NETINPUT_H_
#define _ACTION_NETINPUT_H_

#include "action/Action.h"
#include <QHostAddress>

namespace action
{
	class NetInput : public Action
	{
		Q_OBJECT
	public:
		NetInput( Action* parent );
		virtual ~NetInput();

		void setIFace( const QString& name );

		QString iface() const { return _iface; }

		bool isInitialized() const { return _isInitialized; }

		virtual void customActivate( Action* before );

		virtual void customLeave( Action* next );

		QString result() const;

		void refreshTitle();

	protected:
		QString value() const;
		void setValue( const QString& value );
		void drawDisplay();

		QString _iface;
		QVector< int > _ranges;

	private:
		void back();
		void enter();
		void up();
		void down();

		// current edit position
		unsigned int _editPos;

		QVector<int> _values;

		bool _isInitialized;

	private Q_SLOTS:
		void on_ea09_ip4address_changed( const QHostAddress&, const QHostAddress& );
	};
}

#endif //_ACTION_NETINPUT_H_
