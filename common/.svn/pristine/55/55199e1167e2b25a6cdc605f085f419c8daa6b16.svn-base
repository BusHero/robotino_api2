#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QtGui>
#include "rec/dataexchange/server/Server.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();

private slots:
	void on_server_listening();
	void on_server_closed();
	void on_server_error( QAbstractSocket::SocketError error, const QString& errorString );

	void on_server_clientConnected( quint32 );
	void on_server_clientDisconnected( quint32 );
	void on_server_dataReceived( const QString& name, rec::dataexchange::DataType, const rec::core::variant::Variant& value );
	void on_error( QAbstractSocket::SocketError error, const QString& errorString );
	void on_server_configuration_changed();

	void on_table_itemChanged( QTableWidgetItem* );

	void on_generator_timeout();

	void on_restartServer_clicked();

private:
	rec::dataexchange::server::Server _server;

	QTableWidget* _table;
};

#endif
