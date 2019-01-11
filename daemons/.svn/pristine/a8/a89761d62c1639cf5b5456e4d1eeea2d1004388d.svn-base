#ifndef _ACTION_Execute_H_
#define _ACTION_Execute_H_

#include "action/Action.h"

namespace action
{
	class Execute : public action::Action
	{
		Q_OBJECT
	public:
		Execute( Action* parent );
		virtual ~Execute();

		virtual void customActivate( Action* before );
		void customLeave( Action* next );

	private Q_SLOTS:
		void on_process_readyRead();
		void on_process_finished( int exitCode, QProcess::ExitStatus exitStatus );
		void on_process_error( QProcess::ProcessError );
		void on_timer_timeout();

	private:
		void back();
		void enter();
		void up();
		void down();

		void drawProcessOutput();

		void activateFinishedAction();

		QProcess* _process;
		QString _currentTitle;
		QTimer* _timer;
		unsigned int _step;
		bool _abortWithKeyAllowd;

		QStringList _processOutput;
		int _currentItem;

		int _displayWidth;
		int _maxProcessOutputSize;
	};
}

#endif //_ACTION_Execute_H_
