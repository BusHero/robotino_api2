#ifndef _SERVER_H_
#define _SERVER_H_

#include "rec/robotino/rpc/Server.h"

class Server : public rec::robotino::rpc::Server
{
	Q_OBJECT
public:
	Server();

public Q_SLOTS:
	void disconnectAllNonLocalClients();

private Q_SLOTS:
	void on_clientConnected( const rec::rpc::ClientInfo& info );
	void on_clientDisconnected( const rec::rpc::ClientInfo& info );
	void on_listening();
	
	void on_imageInfoProcess_finished(int exitCode, QProcess::ExitStatus exitStatus);
	void on_imageInfoProcess_errorOccurred(QProcess::ProcessError error);
	void on_imageInfoProcess_readyReadStandardOutput();
	void on_imageInfoProcess_readyReadStandardError();
	void on_imageInfoProcess_started();
	
	void on_upload_program_changed(const QByteArray& jsonData, const QByteArray& data);

private:
	DECLARE_CUSTOM_REQUEST_HANDLER( handleCustomRequest );

	QSet<rec::rpc::ClientInfo> _nonLocalClients;
	
	QProcess* _imageInfoProcess;
	QByteArray _imageInfoData;
};

#endif //_SERVER_H_
