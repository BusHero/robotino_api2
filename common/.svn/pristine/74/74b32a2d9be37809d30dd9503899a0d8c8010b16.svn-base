#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "rec/robotino/rpc/Client.h"

typedef enum
{
	NoCommand,
	HelpCommand,
	SetCommand,
	GetCommand,
	ListCommand,
	FindCommand
} Command;

class Client : public rec::robotino::rpc::Client
{
	Q_OBJECT
public:
	Client( Command command, const QStringList& parameters );

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
};

#endif //_CLIENT_H_