#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QtGui>
#include "rec/dataexchange/client/Client.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();

private slots:
	void on_client_error( QAbstractSocket::SocketError socketError, const QString& );
	void on_client_dataReceived( const QString& name, rec::dataexchange::DataType, const rec::core::variant::Variant& value );
	void on_client_connected();
	void on_client_disconnected();

	void on_client_configuration_changed();

	void on_table_itemChanged( QTableWidgetItem* item );

	void on_generator_timeout();

private:
	rec::dataexchange::client::Client _client;
	QTableWidget* _table;
};

#endif
