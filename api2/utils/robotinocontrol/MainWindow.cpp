//  Copyright (C) 2004-2011, Robotics Equipment Corporation GmbH

#include "MainWindow.h"

#include "CameraView.h"
#include "ControlPanel.h"
#include "AddressInput.h"
#include "CameraParameters.h"
#include "LCDDisplay.h"
#include "Parameters.h"
#include "ClientsConnected.h"

#include "rec/robotino/rpc/Client.h"

MainWindow::MainWindow()
: _client( new rec::robotino::rpc::Client )
, _cameraView( new CameraView )
, _addressInput( new AddressInput( _client ) )
, _cameraParameters( new CameraParameters )
, _display( new LCDDisplay )
, _parameters( new Parameters )
, _clientsConnected( new ClientsConnected )
{
	_client->set_rec_robotino_rpc_image_enabled( true );
	_client->set_rec_robotino_rpc_camera_settings_enabled( true );
	_client->set_rec_robotino_rpc_camera_capabilities_enabled( true );
	_client->set_rec_robotino_rpc_display_buttons_enabled( true );
	_client->set_rec_robotino_rpc_parameters_enabled( true );
	_client->set_rec_robotino_rpc_omnidrive_enabled( true );
	_client->set_rec_robotino_rpc_clients_connected_enabled( true );

	ControlPanel* controlPanel = new ControlPanel;

	QDockWidget* addressInputDock = new QDockWidget( "Adress input", this );
	addressInputDock->setWidget( _addressInput );
	addressInputDock->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
	addDockWidget(Qt::TopDockWidgetArea, addressInputDock );
	addressInputDock->setFloating( false );
	addressInputDock->setVisible( true );

	QDockWidget* controlPanelDock = new QDockWidget( "Control panel", this );
	controlPanelDock->setWidget( controlPanel );
	controlPanelDock->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
	addDockWidget(Qt::BottomDockWidgetArea, controlPanelDock );
	controlPanelDock->setFloating( false );
	controlPanelDock->setVisible( true );

	QDockWidget* cameraParametersDock = new QDockWidget( "Camera parameters", this );
	cameraParametersDock->setWidget( _cameraParameters );
	cameraParametersDock->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
	addDockWidget(Qt::BottomDockWidgetArea, cameraParametersDock );
	cameraParametersDock->setFloating( false );
	cameraParametersDock->setVisible( true );

	QDockWidget* displayDock = new QDockWidget( "LCDDisplay", this );
	displayDock->setWidget( _display );
	displayDock->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
	addDockWidget(Qt::BottomDockWidgetArea, displayDock );
	displayDock->setFloating( false );
	displayDock->setVisible( true );

	QDockWidget* parametersDock = new QDockWidget( "Parameters", this );
	parametersDock->setWidget( _parameters );
	parametersDock->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
	addDockWidget(Qt::BottomDockWidgetArea, parametersDock );
	parametersDock->setFloating( false );
	parametersDock->setVisible( true );

	QDockWidget* clientsConnectedDock = new QDockWidget( "Clients connected", this );
	clientsConnectedDock->setWidget( _clientsConnected );
	clientsConnectedDock->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
	addDockWidget(Qt::BottomDockWidgetArea, clientsConnectedDock );
	clientsConnectedDock->setFloating( false );
	clientsConnectedDock->setVisible( true );

	bool connected = true;
	connected &= connect( _addressInput, SIGNAL( connectToHost( const QString& ) ),
			this, SLOT( on_addressInput_connectToHost( const QString& ) ) );

	connected &= connect( _addressInput, SIGNAL( disconnectFromHost() ),
			this, SLOT( on_addressInput_disconnectFromHost() ) );

	connected &= connect( _cameraParameters,
		SIGNAL( settingsChanged( unsigned int, unsigned int, const QString& ) ),
		_client,	SLOT( set_set_camera_settings( unsigned int, unsigned int, const QString& ) ) );

	connected &= connect( _client, SIGNAL( is_connected() ),
		this, SLOT( on_client_is_connected() ) );

	connected &= connect( _client, SIGNAL( is_disconnected() ),
			this, SLOT( on_client_is_disconnected() ) );

	connected &= connect( _client, SIGNAL( image_changed( const QByteArray&,
									unsigned int,
									unsigned int,
									unsigned int,
									const QString& ) ),
			_cameraView, SLOT( setImage( const QByteArray&,
									unsigned int,
									unsigned int,
									unsigned int,
									const QString& ) ) );

	connected &= connect( _client, SIGNAL( camera_settings_changed( unsigned int, unsigned int, const QString& ) ),
			_cameraParameters, SLOT( setSettings( unsigned int, unsigned int, const QString& ) ) );

	connected &= connect( _client, SIGNAL( camera_capabilities_changed( const QMap<QString, QVector<QSize> >& ) ),
			_cameraParameters, SLOT( setCapablitities( const QMap<QString, QVector<QSize> >& ) ) );

	connected &= connect( _client, SIGNAL( log( const QString&, int ) ),
			this, SLOT( on_client_log( const QString&, int ) ) );

	connected &= connect( _client, SIGNAL( display_buttons_changed( bool, bool, bool, bool ) ),
			_display, SLOT( display_buttons_changed( bool, bool, bool, bool ) ) );

	connected &= connect( _display, SIGNAL( display_text_changed( const QString&, unsigned int, unsigned int, bool, bool ) ),
			_client, SLOT( set_display_text( const QString&, unsigned int, unsigned int, bool, bool ) ) );

	connected &= connect( _display, SIGNAL( display_backlight_changed( bool ) ),
			_client, SLOT( set_display_backlight( bool ) ) );

	connected &= connect( _display, SIGNAL( display_vbar_changed( float, unsigned int, unsigned int, unsigned int ) ),
			_client, SLOT( set_display_vbar( float, unsigned int, unsigned int, unsigned int ) ) );

	connected &= connect( _display, SIGNAL( display_hbar_changed( float, unsigned int, unsigned int, unsigned int ) ),
			_client, SLOT( set_display_hbar( float, unsigned int, unsigned int, unsigned int ) ) );

	connected &= connect( _display, SIGNAL( display_progress_changed( unsigned int, unsigned int ) ),
			_client, SLOT( set_display_progress( unsigned int, unsigned int ) ) );

	connected &= connect( _display, SIGNAL( display_clear_changed() ),
			_client, SLOT( set_display_clear() ) );

	connected &= connect( controlPanel, SIGNAL( speedChanged( float, float, float ) ),
			_client, SLOT( set_omnidrive( float, float, float ) ) );

	connected &= connect( _client, SIGNAL( parameters_changed( const QMap< QString, QVariant >& ) ),
			_parameters, SLOT( set_parameters( const QMap< QString, QVariant >& ) ) );

	connected &= connect( _client, SIGNAL( clients_connected_changed( const QVector< rec::rpc::ClientInfo >& ) ),
			_clientsConnected, SLOT( setClients( const QVector< rec::rpc::ClientInfo >& ) ) );

	connected &= connect( _client, SIGNAL( current_controller_changed( const rec::rpc::ClientInfo& ) ),
			_clientsConnected, SLOT( setCurrentController( const rec::rpc::ClientInfo& ) ) );

	Q_ASSERT( connected );

	setCentralWidget( _cameraView );
}

MainWindow::~MainWindow()
{
	delete _client;
}

void MainWindow::on_addressInput_connectToHost( const QString& address )
{
	_client->setAddress( address );
	_client->connectToServer();
}

void MainWindow::on_addressInput_disconnectFromHost()
{
	_client->disconnectFromServer();
}

void MainWindow::on_client_is_connected()
{
	QSettings settings;
	settings.setValue( "ipaddress", _client->address() );

	_addressInput->setConnected( true );
}

void MainWindow::on_client_is_disconnected()
{
	_addressInput->setConnected( false );
}

void MainWindow::on_client_log( const QString& message, int )
{
	qDebug() << message;
}
