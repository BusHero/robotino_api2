#include "CameraParameters.h"

CameraParameters::CameraParameters()
{
	bool connected = true;
	connected &= connect( this, SIGNAL( itemClicked ( QListWidgetItem* ) ), SLOT( on_itemClicked ( QListWidgetItem* ) ) );
	Q_ASSERT( connected );
}

void CameraParameters::setSettings( unsigned int width, unsigned int height, const QString& format )
{
	QString label = QString( "%1x%2 %3" ).arg( width ).arg( height ).arg( format );

	for( int i=0; i<count(); ++i )
	{
		QListWidgetItem* it = item( i );
		it->setIcon( QIcon() );
	}

	QList<QListWidgetItem*> l =	findItems( label, Qt::MatchExactly );

	if( 1 == l.size() )
	{
		l.at(0)->setIcon( QPixmap( ":/icons/green_dot.png" ) );
	}
}

void CameraParameters::setCapablitities( const QMap<QString, QVector<QSize> >& capabilities )
{
	clear();

	QMap<QString, QVector<QSize> >::const_iterator iter = capabilities.constBegin();
	while( capabilities.constEnd() != iter )
	{
		QString format = iter.key();
		const QVector<QSize>& sizes = iter.value();
		Q_FOREACH( const QSize& s, sizes )
		{
			addItem( QString( "%1x%2 %3" ).arg( s.width() ).arg( s.height() ).arg( format ) );
		}

		++iter;
	}
}

void CameraParameters::on_itemClicked ( QListWidgetItem* item )
{
	QStringList l = item->text().split( " " );
	QStringList sizeList = l.at(0).split( "x" );

	Q_EMIT settingsChanged( sizeList.at(0).toUInt(), sizeList.at(1).toUInt(), l.at( 1 ) );
}
