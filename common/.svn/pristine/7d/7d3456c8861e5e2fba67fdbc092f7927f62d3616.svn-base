#include "MainWindow.h"
#include "rec/core/LocalizedLaserRangeFinderData.h"
#include "rec/core/LaserRangeFinderInfo.h"
#include "rec/dataexchange/ports.h"

MainWindow::MainWindow()
: _table( new QTableWidget( this ) )
{
	setCentralWidget( _table );

	QTimer* generator = new QTimer( this );
	generator->setSingleShot( false );
	generator->setInterval( 10 );
	connect( generator, SIGNAL( timeout() ), SLOT( on_generator_timeout() ) );
	generator->start();

	connect( &_client, SIGNAL( error( QAbstractSocket::SocketError, const QString& ) ), SLOT( on_client_error( QAbstractSocket::SocketError, const QString& ) ) );
	connect( &_client, SIGNAL( dataReceived( const QString&, rec::dataexchange::DataType, const rec::core::variant::Variant& ) ), SLOT( on_client_dataReceived( const QString&, rec::dataexchange::DataType, const rec::core::variant::Variant& ) ) );
	connect( &_client, SIGNAL( connected() ), SLOT( on_client_connected() ) );
	connect( &_client, SIGNAL( disconnected() ), SLOT( on_client_disconnected() ) );

	connect( _client.configuration(), SIGNAL( changed() ), SLOT( on_client_configuration_changed() ) );

	_client.registerChannel( "devices" );

	//_client.setAddress( "127.0.0.1" );
	_client.setAddress( "192.168.1.173:9000" );
	_client.connectToServer();

	connect( _table, SIGNAL( itemChanged( QTableWidgetItem* ) ), SLOT( on_table_itemChanged( QTableWidgetItem* ) ) );
}

void MainWindow::on_client_error( QAbstractSocket::SocketError socketError, const QString& errorString )
{
	qDebug() << "Error: " << errorString;
	_client.connectToServer();
}

void MainWindow::on_client_dataReceived( const QString& name, rec::dataexchange::DataType type, const rec::core::variant::Variant& value )
{
	qDebug() << "Client MainWindow::on_client_dataReceived: " << name << "   value: " << value.toString().c_str();

	_table->blockSignals( true );
	for( int row=0; row < _table->rowCount(); ++row )
	{
		if( _table->item( row, 0 )->text() == name )
		{
			QString text;
			if( rec::dataexchange::LOCALIZEDLASERRANGEFINDERDATA == type )
			{
				rec::core::LocalizedLaserRangeFinderData data = value.get< rec::core::LocalizedLaserRangeFinderData >();

				text = QString( "numValues=%1 x=%2 y=%3 phi=%4" ).arg( data.measuresVector.size() ).arg( data.poseX ).arg( data.poseY ).arg( data.poseOrientation );
			}
			if( rec::dataexchange::LASERRANGEFINDERINFOLIST == type )
			{
				rec::core::LaserRangeFinderInfoList list = value.get< rec::core::LaserRangeFinderInfoList >();

				rec::core::LaserRangeFinderInfoList::const_iterator iter = list.constBegin();
				while( list.constEnd() != iter )
				{
					text.append( iter->name.c_str() );
					++iter;

					if( list.constEnd() != iter )
					{
						text.append( "\n" );
					}
				}
			}
			else
			{
				text = QString::fromStdString( value.toString() );
			}
			
			_table->item( row, 2 )->setText( text );
			break;
		}
	}
	_table->blockSignals( false );
}

void MainWindow::on_client_connected()
{
	qDebug() << "Connected";
}

void MainWindow::on_client_disconnected()
{
	qDebug() << "Disconnected";
	_client.connectToServer();
}

void MainWindow::on_client_configuration_changed()
{
	_table->blockSignals( true );

	_table->clear();
	_table->setColumnCount( 4 );
	_table->setHorizontalHeaderLabels( QStringList() << "name" << "type" << "value" << "register" );

	QMap< QString, rec::dataexchange::configuration::Item > container = _client.configuration()->itemContainer();

	_table->setRowCount( container.size() );

	QMap< QString, rec::dataexchange::configuration::Item >::const_iterator iter = container.constBegin();
	int row = 0;
	while( container.constEnd() != iter )
	{
		const rec::dataexchange::configuration::Item& cfgItem = iter.value();

		for( int col = 0; col < 4; ++col )
		{
			QTableWidgetItem* item = NULL;
			
			switch( col )
			{
			case 0:
				item = new QTableWidgetItem( cfgItem.name );
				item->setFlags( item->flags() & ~Qt::ItemIsEditable );
				break;

			case 1:
				item = new QTableWidgetItem( QString::fromLatin1( rec::dataexchange::dataTypeName( cfgItem.type ) ) );
				item->setFlags( item->flags() & ~Qt::ItemIsEditable );
				break;

			case 2:
				item = new QTableWidgetItem( QString::fromStdString( cfgItem.data.toString() ) );
				break;

			case 3:
				if( _client.isChannelRegistered( cfgItem.name ) )
				{
					item = new QTableWidgetItem( "1" );
				}
				else
				{
					item = new QTableWidgetItem( "0" );
				}
				break;
			}

			_table->setItem( row, col, item );
		}

		++iter;
		++row;
	}
	
	_table->blockSignals( false );
}

void MainWindow::on_table_itemChanged( QTableWidgetItem* item )
{
	switch( item->column() )
	{
	case 2:
		_client.sendData( _table->item( item->row(), 0 )->text(), item->text().toStdString() );
		break;

	case 3:
		if( "1" == item->text() )
		{
			_client.registerChannel( _table->item( item->row(), 0 )->text() );
		}
		else
		{
			_client.unregisterChannel( _table->item( item->row(), 0 )->text() );
		}
	}
}

void MainWindow::on_generator_timeout()
{
	static unsigned int value = 0;
	//_client.sendData( "item2", value++ );
}

