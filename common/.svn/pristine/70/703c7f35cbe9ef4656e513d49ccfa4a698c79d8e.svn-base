#include "PositionStorage.h"

PositionStorage::PositionStorage( QWidget* parent )
: QListWidget( parent )
{
	QSettings settings;
	QStringList data = settings.value( "positions" ).toStringList();

	addItem( "(511;40) (511;40) (511;40) (511;40) (511;40) (511;40) (511;40) (511;40) (511;40)" );
	Q_FOREACH( const QString& str, data )
	{
		addItem( str );
	}
}

void PositionStorage::on_load()
{
	Q_EMIT load( item( currentRow() )->text() );
}

void PositionStorage::on_remove()
{
	QListWidgetItem* item = takeItem( currentRow() );
	delete item;
}

void PositionStorage::on_edit()
{
	//QListWidgetItem* item = item( currentRow() );

	//QDialog dlg;
	//dlg.add
	//dlg.exec();
}

void PositionStorage::on_saveToFile()
{
	QSettings settings;

	QString fileName = QFileDialog::getSaveFileName( this, "Save positions to file", settings.value( "positionsdir" ).toString() );

	QFile file( fileName );
	if( file.open( QIODevice::WriteOnly ) )
	{
		settings.setValue( "positionsdir", fileName );

		QTextStream out(&file);
		
		for( int i=0; i<count(); ++i )
		{
			out << item( i )->text() << '\n';
		}

		file.close();
	}
}

void PositionStorage::on_loadFromFile()
{
	QSettings settings;

	QString fileName = QFileDialog::getOpenFileName( this, "Load positions to file", settings.value( "positionsdir" ).toString() );

	QFile file( fileName );
	if( file.open( QIODevice::ReadOnly ) )
	{
		clear();

		settings.setValue( "positionsdir", fileName );

		QTextStream in(&file);
		
		while (!in.atEnd())
		{
			QString line = in.readLine();
			if( line.isEmpty() )
			{
				break;
			}

			addItem( line );
		}

		file.close();
	}

}

void PositionStorage::mousePressEvent( QMouseEvent* e )
{
	QListWidgetItem* item = itemAt( e->pos() );

	if( item && e->button() == Qt::RightButton )
	{
		setCurrentRow( row( item ) );
		QMenu menu;

		menu.addAction( "Load", this, SLOT( on_load() ) );
		menu.addAction( "Remove", this, SLOT( on_remove() ) );
		menu.addAction( "Edit", this, SLOT( on_edit() ) );
		menu.addAction( "Save to file", this, SLOT( on_saveToFile() ) );
		menu.addAction( "Load from file", this, SLOT( on_loadFromFile() ) );

		menu.exec( e->globalPos() );
	}
}

void PositionStorage::hideEvent( QHideEvent* e )
{
	QStringList data;

	for( int i=1; i<count(); ++i )
	{
		data.append( item( i )->text() );
	}

	QSettings settings;
	settings.setValue( "positions", data );
}
