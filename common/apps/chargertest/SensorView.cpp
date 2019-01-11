#include "SensorView.h"

#define COL_NAME 0
#define COL_VALUE 1

enum
{
	VOLTAGE_ROW,
	CURRENT_ROW,
	TEMP1_ROW,
	TEMP2_ROW,
	TIME_ROW,
	VERSION_ROW,
	ERROR_ROW,
	ROW_COUNT
};

SensorView::SensorView( QWidget* parent )
	: QDockWidget( parent )
	, _table( new QTableWidget( this ) )
{
	setWidget( _table );

	_table->setRowCount( ROW_COUNT );
	_table->setColumnCount( 2 );

	_table->setItem( VOLTAGE_ROW, COL_NAME, new QTableWidgetItem( "Voltage" ) );
	_table->setItem( VOLTAGE_ROW, COL_VALUE, new QTableWidgetItem );

	_table->setItem( CURRENT_ROW, COL_NAME, new QTableWidgetItem( "Current" ) );
	_table->setItem( CURRENT_ROW, COL_VALUE, new QTableWidgetItem );

	_table->setItem( TEMP1_ROW, COL_NAME, new QTableWidgetItem( "Temp1" ) );
	_table->setItem( TEMP1_ROW, COL_VALUE, new QTableWidgetItem );

	_table->setItem( TEMP2_ROW, COL_NAME, new QTableWidgetItem( "Temp2" ) );
	_table->setItem( TEMP2_ROW, COL_VALUE, new QTableWidgetItem );

	_table->setItem( TIME_ROW, COL_NAME, new QTableWidgetItem( "Time" ) );
	_table->setItem( TIME_ROW, COL_VALUE, new QTableWidgetItem );

	_table->setItem( VERSION_ROW, COL_NAME, new QTableWidgetItem( "Version" ) );
	_table->setItem( VERSION_ROW, COL_VALUE, new QTableWidgetItem );

	_table->setItem( ERROR_ROW, COL_NAME, new QTableWidgetItem( "Error" ) );
	_table->setItem( ERROR_ROW, COL_VALUE, new QTableWidgetItem );
}

void SensorView::setReadings( const QVariantMap& readings )
{
	_table->item( VOLTAGE_ROW, COL_VALUE )->setText( QString("%1").arg( readings.value( "voltage" ).toDouble() ) );
	_table->item( CURRENT_ROW, COL_VALUE )->setText( QString("%1").arg( readings.value( "current" ).toDouble() ) );
	_table->item( TEMP1_ROW, COL_VALUE )->setText( QString("%1").arg( readings.value( "temp1" ).toDouble() ) );
	_table->item( TEMP2_ROW, COL_VALUE )->setText( QString("%1").arg( readings.value( "temp2" ).toDouble() ) );

	int time = readings.value( "time" ).toInt();
	int hours = time / 3600;
	int minutes = ( time - hours * 3600 ) / 60;
	int seconds = ( time - hours * 3600 - minutes * 60 );

	_table->item( TIME_ROW, COL_VALUE )->setText( QString("%1:%2:%3").arg( hours, 3, 10, QChar('0') ).arg( minutes, 2, 10,  QChar('0') ).arg( seconds, 2, 10,  QChar('0') ) );;
	_table->item( VERSION_ROW, COL_VALUE )->setText( readings.value("version").toString() );
	_table->item( ERROR_ROW, COL_VALUE )->setText( QString("%1").arg( readings.value( "error" ).toDouble() ) );
}
