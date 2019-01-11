#include "Parameters.h"

Parameters::Parameters()
{
}

void Parameters::set_parameters( const QMap< QString, QVariant >& values )
{
	clear();

	setColumnCount( 2 );
	setRowCount( values.size() );

	int row = 0;
	QMap< QString, QVariant >::const_iterator iter = values.constBegin();
	while( values.constEnd() != iter )
	{
		QTableWidgetItem* key = new QTableWidgetItem( iter.key() );
		QTableWidgetItem* value = new QTableWidgetItem( (*iter).toString() );

		setItem( row, 0, key );
		setItem( row, 1, value );

		++iter;
		++row;
	}
}
