#include "Server.h"

Server::Server()
: _imageInfoProcess(NULL)
{
	REGISTER_CUSTOM_REQUEST_HANDLER( handleCustomRequest );

	bool connected = true;

	connected = QObject::connect( this, SIGNAL( closed() ), this, SLOT( exit() ) );
	Q_ASSERT( connected );

	connected = QObject::connect( this, SIGNAL( clientConnected( const rec::rpc::ClientInfo& ) ), this, SLOT( on_clientConnected( const rec::rpc::ClientInfo& ) ) );
	Q_ASSERT( connected );

	connected = QObject::connect( this, SIGNAL( clientDisconnected( const rec::rpc::ClientInfo& ) ), this, SLOT( on_clientDisconnected( const rec::rpc::ClientInfo& ) ) );
	Q_ASSERT( connected );
	
	connected = QObject::connect( this, SIGNAL( listening() ), SLOT( on_listening() ) );
	Q_ASSERT( connected );
	
	connected = QObject::connect( this, SIGNAL( listening() ), SLOT( on_listening() ) );
	Q_ASSERT( connected );
	
	connected = QObject::connect( this, &Server::upload_program_changed , this, &Server::on_upload_program_changed);
	Q_ASSERT( connected );

	set_rec_robotino_rpc_set_log_level_enabled( true );
	set_rec_robotino_rpc_upload_program_enabled( true );
}

BEGIN_CUSTOM_REQUEST_HANDLER_DEFINITION( Server, handleCustomRequest )
	response = QString("RPC").toLatin1();
	return;
END_CUSTOM_REQUEST_HANDLER_DEFINITION

void Server::disconnectAllNonLocalClients()
{
	Q_FOREACH( const rec::rpc::ClientInfo& info, _nonLocalClients )
	{
		Q_EMIT log( QString("Disconnecting %1:%2 %3").arg( info.address.toString() ).arg( info.port ).arg( info.name ), 0 );
		disconnectClient( info.address, info.port );
	}
}

void Server::on_clientConnected( const rec::rpc::ClientInfo& info )
{
	if( false == info.isLocal() )
	{
		_nonLocalClients.insert( info );
	}
}

void Server::on_clientDisconnected( const rec::rpc::ClientInfo& info )
{
	_nonLocalClients.remove( info );
}

void Server::on_upload_program_changed(const QByteArray& jsonData, const QByteArray& data)
{
	QJsonParseError  error;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &error);

	if (QJsonParseError::NoError == error.error)
	{
		QJsonObject root = jsonDoc.object();
		QString commandStr = root["COMMAND"].toString();
		
		if("UPLOAD_RVWX" == commandStr)
		{
			QString nameStr = root["NAME"].toString();
			
			QString fileName = "/home/robotino/programs/" + nameStr;
			
			QFile file( fileName );
			if(file.open(QIODevice::WriteOnly))
			{
				file.write(data);
			}
			int ret = system( QString("chown robotino.robotino \"%1\"").arg(fileName).toLatin1().constData() );
		}
	}
}

void Server::on_listening()
{
	delete _imageInfoProcess;
	_imageInfoProcess = new QProcess(this);
	
	_imageInfoData.clear();
	
	bool ok = connect(_imageInfoProcess,SIGNAL(finished(int, QProcess::ExitStatus)),SLOT(on_imageInfoProcess_finished(int, QProcess::ExitStatus)),Qt::DirectConnection);
	Q_ASSERT(ok);
	
	//ok = connect(_imageInfoProcess,SIGNAL(errorOccurred(QProcess::ProcessError)),SLOT(on_imageInfoProcess_errorOccurred(QProcess::ProcessError)),Qt::DirectConnection);
	//Q_ASSERT(ok);
	
	ok = connect(_imageInfoProcess,SIGNAL(readyReadStandardOutput()),SLOT(on_imageInfoProcess_readyReadStandardOutput()),Qt::DirectConnection);
	Q_ASSERT(ok);
	
	ok = connect(_imageInfoProcess,SIGNAL(readyReadStandardError()),SLOT(on_imageInfoProcess_readyReadStandardError()),Qt::DirectConnection);
	Q_ASSERT(ok);
	
	ok = connect(_imageInfoProcess,SIGNAL(started()),SLOT(on_imageInfoProcess_started()),Qt::DirectConnection);
	Q_ASSERT(ok);
	
	_imageInfoProcess->start("/usr/local/bin/create-image-info.sh");
}

void Server::on_imageInfoProcess_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
	if(QProcess::NormalExit == exitStatus)
	{
		set_json_message_with_data(rec::robotino::rpc::imageinfo_topic,_imageInfoData,QByteArray());
	}
}

void Server::on_imageInfoProcess_errorOccurred(QProcess::ProcessError error)
{
	
}

void Server::on_imageInfoProcess_readyReadStandardOutput()
{
	QByteArray ba = _imageInfoProcess->readAllStandardOutput();
	
	_imageInfoData.append(ba);
}

void Server::on_imageInfoProcess_readyReadStandardError()
{
	
}

void Server::on_imageInfoProcess_started()
{
	
}
