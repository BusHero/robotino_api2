#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_

#include "rec/robotino/rpc/Server.h"

class Client;
class DistanceHandler;
class ControlHandler;
class ImageHandler;

class WebServer : public rec::robotino::rpc::Server
{
	Q_OBJECT
public:
	WebServer();
	~WebServer();

private Q_SLOTS:
	void on_listening();
	void on_clientConnected( const rec::rpc::ClientInfo& );
	void on_clientDisconnected( const rec::rpc::ClientInfo& );

	void on_unregisterTopicsTimer_timeout();

	void on_controlHandler_positionDriverFinished();

private:
	DECLARE_CUSTOM_REQUEST_HANDLER( handleCustomRequest );
	DECLARE_HTTP_GET_HANDLER( handleHttpGetRequest );

	int _numWebClients;

	QByteArray _liveControlPage;
	QByteArray _controlPage;
	QMap< QString, QByteArray > _webImages;

	DistanceHandler* _distanceHandler;
	ControlHandler* _controlHandler;
	ImageHandler* _imageHandler;

	QTimer* _unregisterTopicsTimer;
};

#endif //_WEBSERVER_H_
