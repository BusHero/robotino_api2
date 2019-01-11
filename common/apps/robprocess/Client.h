#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "rec/robotino/rpc/Client.h"

typedef enum
{
	NoCommand,
	LaunchCommand,
	TerminateCommand,
	KillCommand,
	GetIdsCommand,
	GetStatusCommand
} Command;

class Client : public rec::robotino::rpc::Client
{
	Q_OBJECT
public:
	Client( Command command, const QStringList& parameters, const QString& workingDirectory );

	int errorCode;

Q_SIGNALS:
	void delayed_connected_signal();

private Q_SLOTS:
	void on_connected();
	void on_disconnected( rec::rpc::ErrorCode );
	void on_delayed_connected();

private:
	Command _command;
	const QStringList _parameters;
	const QString _workingDirectory;
};

#endif //_CLIENT_H_