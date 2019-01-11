#ifndef _ACTION_ACTION_H_
#define _ACTION_ACTION_H_

#include <QtCore>

namespace action
{
	class SignalHelper;
	class Factory;

	class Action : public QObject
	{
		Q_OBJECT
		friend class SignalHelper;
		friend class Factory;
	public:
		Action( Action* parent );

		void addCondition( const QString& condition );

		bool conditionsOk() const;

		virtual QString title() const { return _attributes.value( "title" ); }

		Action* parent() const { return _parent; }

		QString fullPath() const;

		static void activate( Action* next, bool force = false );

		QString attribute( const QString& key, const QString& defaultValue = QString() ) const { return _attributes.value( key, defaultValue ); }

		void setAttribute( const QString& key, const QString& value ) { _attributes[ key ] = value; }

		void setAttributes( const QMap< QString, QString >& attr ) { _attributes = attr; }

		QStringList parameters() const;

		void setParameters( const QStringList& params );

		void clearParameters();

		Action* findChildByTitle( const QString& title );

		QList< Action* > children() const { return _children; }

		virtual void refreshTitle() {};

		virtual QString result() const { return QString(); }

		virtual void execute( Action* caller ) {};

		static Action* rootAction() { return _rootAction; }

		static Action* currentAction() { return _currentAction; }

		static Action* controlActivatedAction() { return _controlActivatedAction; }

		static void setCondition( const QString& key, bool );

		static bool containsCondition( const QString& key );

		static Action* find( const QString& title );

	protected:
		virtual void back() {};
		virtual void up() {};
		virtual void down() {};
		virtual void enter() {};

		virtual void customActivate( Action* before ) {};
		virtual void customLeave( Action* next ) {};

		void addChild( Action* child );

		void log( const QString&, int );
		void setDisplayText( const QString& text, unsigned int row, unsigned int col, bool clear_before, bool clear_after );
		void setDisplayBacklight( bool on );
		void setDisplayVBar( float value, unsigned int col, unsigned int start_row, unsigned int end_row );
		void setDisplayHBar( float value, unsigned int row, unsigned int start_col, unsigned int end_col );
		void setDisplayProgress( unsigned int step, unsigned int row );
		void displayClear();

		QMap< QString, QString > _attributes;
		QList< Action* > _children;
		Action* _parent;

	private Q_SLOTS:

	private:
		enum { ActivateEventId = QEvent::User };

		void customEvent( QEvent* e );
		void activate_i( Action* before );
		void leave( Action* next );

		SignalHelper* _signalHelper;

		static Action* _rootAction;
		static Action* _currentAction;
		static Action* _controlActivatedAction;
		static QMap<QString, bool> g_conditions;

		class ActivateEvent : public QEvent
		{
		public:
			ActivateEvent( Action* before_ )
				: QEvent( (QEvent::Type)ActivateEventId )
				, before( before_ )
			{
			}

			Action* before;
		};
	};
}

#endif //_ACTION_ACTION_H_
