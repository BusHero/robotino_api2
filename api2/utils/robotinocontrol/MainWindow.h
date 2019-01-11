#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QtGui>

namespace rec
{
	namespace robotino
	{
		namespace rpc
		{
			class Client;
		}
	}
}

class CameraView;
class AddressInput;
class CameraParameters;
class LCDDisplay;
class Parameters;
class ClientsConnected;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();
	~MainWindow();

private Q_SLOTS:
	void on_addressInput_connectToHost( const QString& );
	void on_addressInput_disconnectFromHost();

	void on_client_is_connected();
	void on_client_is_disconnected();
	void on_client_log( const QString& message, int );

private:
	rec::robotino::rpc::Client* _client;
	CameraView* _cameraView;
	AddressInput* _addressInput;
	CameraParameters* _cameraParameters;
	LCDDisplay* _display;
	Parameters* _parameters;
	ClientsConnected* _clientsConnected;
};

#endif
