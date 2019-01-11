#include "MainWindow.h"

MainWindow::MainWindow()
: _table( new QTableWidget( this ) )
{
	QTimer* generator = new QTimer( this );
	generator->setSingleShot( false );
	generator->setInterval( 10 );
	connect( generator, SIGNAL( timeout() ), SLOT( on_generator_timeout() ) );
	generator->start();

	setCentralWidget( _table );

	connect( &_server,
		SIGNAL( listening() ),
		SLOT( on_server_listening() ) );

	connect( &_server,
		SIGNAL( closed() ),
		SLOT( on_server_closed() ) );

	connect( &_server,
		SIGNAL( error( QAbstractSocket::SocketError, const QString& ) ),
		SLOT( on_server_error( QAbstractSocket::SocketError, const QString& ) ) );

	connect( &_server,
		SIGNAL( clientConnected( quint32 ) ),
		SLOT( on_server_clientConnected( quint32 ) ) );

	connect( &_server,
		SIGNAL( clientDisconnected( quint32 ) ),
		SLOT( on_server_clientDisconnected( quint32 ) ) );

	connect( &_server,
		SIGNAL( dataReceived( const QString&, rec::dataexchange::DataType, const rec::core::variant::Variant& ) ),
		SLOT( on_server_dataReceived( const QString&, rec::dataexchange::DataType, const rec::core::variant::Variant& ) ) );

	connect( &_server,
		SIGNAL( error( QAbstractSocket::SocketError, const QString& ) ),
		SLOT( on_error( QAbstractSocket::SocketError, const QString& ) ) );

	connect( _server.configuration(),
		SIGNAL( changed() ),
		SLOT( on_server_configuration_changed() ) );

	_server.listen();

	_server.configuration()->addItem( "123 item1", rec::dataexchange::U32 );
	_server.configuration()->addItem( "@item2", rec::dataexchange::FLOAT32 );
	_server.configuration()->addItem( "#!<<>><!--item3", rec::dataexchange::STRING );
	_server.configuration()->addItem( "item4", rec::dataexchange::I32 );
	_server.configuration()->addItem( "item5", rec::dataexchange::LASERRANGEFINDERINFOLIST );
	_server.configuration()->addItem( "item6", rec::dataexchange::LASERRANGEFINDERDATA );
	_server.configuration()->addItem( "item7", rec::dataexchange::LOCALIZEDLASERRANGEFINDERDATA );
	_server.configuration()->addItem( "item8", rec::dataexchange::FLOATARRAY );

	connect( _table, SIGNAL( itemChanged( QTableWidgetItem* ) ), SLOT( on_table_itemChanged( QTableWidgetItem* ) ) );

	QPushButton* restartServer = new QPushButton;
	restartServer->setText( "Restart server" );
	statusBar()->addWidget( restartServer );
	connect( restartServer, SIGNAL( clicked() ), SLOT( on_restartServer_clicked() ) );

}

void MainWindow::on_server_listening()
{
	qDebug() << QString( "Server listening at port %1" ).arg( _server.serverPort() );
}

void MainWindow::on_server_closed()
{
	qDebug() << QString( "Server closed" );
}

void MainWindow::on_server_error( QAbstractSocket::SocketError error, const QString& errorString )
{
	qDebug() << QString( "Server error: %1" ).arg( errorString );
}

void MainWindow::on_server_clientConnected( quint32 address )
{
	qDebug() << "Client connected " << QHostAddress( address );
}

void MainWindow::on_server_clientDisconnected( quint32 address )
{
	qDebug() << "Client disconnected " << QHostAddress( address );
}

void MainWindow::on_server_dataReceived( const QString& name, rec::dataexchange::DataType type, const rec::core::variant::Variant& value )
{
	qDebug() << "Server MainWindow::on_server_dataReceived: " << name << "   value: " << value.toString().c_str();

	_table->blockSignals( true );
	for( int row=0; row < _table->rowCount(); ++row )
	{
		if( _table->item( row, 0 )->text() == name )
		{
			_table->item( row, 2 )->setText( QString::fromStdString( value.toString() ) );
			break;
		}
	}
	_table->blockSignals( false );
}

void MainWindow::on_error( QAbstractSocket::SocketError error, const QString& errorString )
{
	qDebug() << "Error: " << errorString;
}


void MainWindow::on_server_configuration_changed()
{
	_table->clear();
	_table->setColumnCount( 3 );
	_table->setHorizontalHeaderLabels( QStringList() << "name" << "type" << "value" );

	QMap< QString, rec::dataexchange::configuration::Item > container = _server.configuration()->itemContainer();

	_table->setRowCount( container.size() );

	QMap< QString, rec::dataexchange::configuration::Item >::const_iterator iter = container.constBegin();
	int row = 0;
	while( container.constEnd() != iter )
	{
		const rec::dataexchange::configuration::Item& cfgItem = iter.value();

		for( int col = 0; col < 3; ++col )
		{
			QString text;
			
			switch( col )
			{
			case 0:
				text = cfgItem.name;
				break;

			case 1:
				text = QString::fromLatin1( rec::dataexchange::dataTypeName( cfgItem.type ) );
				break;

			case 2:
				text = QString::fromStdString( cfgItem.data.toString() );
				break;
			}

			QTableWidgetItem* item = new QTableWidgetItem( text );
			_table->setItem( row, col, item );
		}

		++iter;
		++row;
	}
}

void MainWindow::on_table_itemChanged( QTableWidgetItem* item )
{
	switch( item->column() )
	{
	case 2:
		_server.sendData( _table->item( item->row(), 0 )->text(), item->text().toStdString() );
		break;
	}
}

void MainWindow::on_generator_timeout()
{
	static unsigned int value = 0;
	//_server.sendData( "item1", value++ );
}

void MainWindow::on_restartServer_clicked()
{
	_server.close();

	_server.listen();
}

