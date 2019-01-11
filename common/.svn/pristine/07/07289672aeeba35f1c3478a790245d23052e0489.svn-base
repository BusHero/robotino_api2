#include "PositionTable.h"

#define PSETCOL 0
#define SSETCOL 1
#define PCURCOL 2
#define SCURCOL 3
#define CWCOL	4
#define CCWCOL	5
#define ERRCOL	6

PositionTable::PositionTable( QWidget* parent )
: QTableWidget( parent )
{
	setRowCount( 9 );
	setColumnCount( 7 );

	for( int row=0; row<rowCount(); ++row )
	{
		for( int col=0; col<columnCount(); ++col )
		{
			QTableWidgetItem* item = new QTableWidgetItem( "0" );
			if( SSETCOL == col )
			{
				item->setText( "40" );
			}

			setItem( row, col, item );
		}
	}

	setVerticalHeaderLabels( QStringList()
		<< "RX64 ID1"
		<< "RX64 ID2"
		<< "RX64 ID3"
		<< "RX28 ID1"
		<< "RX28 ID2"
		<< "RX28 ID3"
		<< "RX10 ID1"
		<< "RX10 ID2"
		<< "RX10 ID3"
		);

	setHorizontalHeaderLabels( QStringList()
		<< "Pset"
		<< "Sset"
		<< "Pcur"
		<< "Scur"
		<< "CW Lim"
		<< "CCW Lim"
		<< "err"
		);

	for( int row=0; row<rowCount(); ++row )
	{
		setRowHidden( row, true );
	}
}

void PositionTable::setServosDetected( const rec::grappler::HardwareInfo& info )
{
	for( int i=0; i<info.servos.size(); ++i )
	{
		setRowHidden( i, false == info.servos[i].found );
	}
}

void PositionTable::setAllServoPositions( const rec::grappler::HardwareInfo& info, bool useAsSetPoint )
{
	for( int i=0; i<info.servos.size(); ++i )
	{
		QTableWidgetItem* positm = item( i, PCURCOL );
		QString posText = QString( "%1" ).arg( info.servos[i].currentPosition, 4 );
		positm->setText( posText );

		QTableWidgetItem* speeditm = item( i, SCURCOL );
		QString speedtext = QString( "%1" ).arg( info.servos[i].currentSpeed, 4 );
		speeditm->setText( speedtext );

		if( useAsSetPoint )
		{
			QTableWidgetItem* setitm = item( i, PSETCOL );
			setitm->setText( posText );
		}
	
		QTableWidgetItem* erritm = item( i, ERRCOL );
		erritm->setText( QString( "%1" ).arg( info.servos[i].error, 2 ) );
	}
}

void PositionTable::setAxesLimits( const rec::grappler::HardwareInfo& info )
{
	for( int i=0; i<info.servos.size(); ++i )
	{
		QTableWidgetItem* cwitm = item( i, CWCOL );
		QString cwText = QString( "%1" ).arg( info.servos[i].cwAxisLimit, 4 );
		cwitm->setText( cwText );

		QTableWidgetItem* ccwitm = item( i, CCWCOL );
		QString ccwText = QString( "%1" ).arg( info.servos[i].ccwAxisLimit, 4 );
		ccwitm->setText( ccwText );
	}
}

QVector<quint16> PositionTable::positions() const
{
	QVector<quint16> vec( rowCount() );

	for( int row=0; row<rowCount(); ++row )
	{
		QTableWidgetItem* itm = item( row, PSETCOL );
		vec[row] = itm->text().toUShort();
	}

	return vec;
}

QVector<quint16> PositionTable::currentPositions() const
{
	QVector<quint16> vec( rowCount() );

	for( int row=0; row<rowCount(); ++row )
	{
		QTableWidgetItem* itm = item( row, PCURCOL );
		vec[row] = itm->text().toUShort();
	}

	return vec;
}

QVector<quint16> PositionTable::speeds() const
{
	QVector<quint16> vec( rowCount() );

	for( int row=0; row<rowCount(); ++row )
	{
		QTableWidgetItem* itm = item( row, SSETCOL );
		vec[row] = itm->text().toUShort();
	}

	return vec;
}