#include "MainWindow.h"

#include "rec/dataexchange/messages/Data.h"
#include "rec/core/variant/Math.h"

#include <QtDebug>
#include <QBuffer>

bool basicTest()
{
	rec::core::variant::Variant a = rec::core::variant::Variant::fromStdString( "12345" );
	unsigned int b = a.toUInt32();

	const unsigned int samples = 5;

	rec::dataexchange::configuration::Item items[samples];

	items[0] = rec::dataexchange::configuration::Item( "test", rec::dataexchange::U32, 12345 );
	items[1] = rec::dataexchange::configuration::Item( "abscd", rec::dataexchange::I32, -12345 );
	items[2] = rec::dataexchange::configuration::Item( "3498fhkls", rec::dataexchange::FLOAT32, 12345.2938 );
	items[3] = rec::dataexchange::configuration::Item( "ckjbv##1243", rec::dataexchange::STRING, rec::core::variant::Variant::fromStdString( "dies ist ein langer string" ) );
	items[4] = rec::dataexchange::configuration::Item( "test1", rec::dataexchange::U32, 123458899 );

	QByteArray data;

	for( unsigned int i=0; i<samples; ++i )
	{
		data += rec::dataexchange::messages::Data::encode( items[i].name, items[i].type, items[i].data );
	}

	//qDebug() << data.size();

	QBuffer buffer;
	buffer.setData( data );
	buffer.open( QIODevice::ReadOnly );

	for( unsigned int i=0; i<samples; ++i )
	{
		QByteArray headerData = buffer.read( 5 );

		quint8 messageId = headerData.at( 0 );

		quint32 messageSize = headerData.at( 1 );
		messageSize |= ( headerData.at( 2 ) << 8 );
		messageSize |= ( headerData.at( 3 ) << 16 );
		messageSize |= ( headerData.at( 4 ) << 24 );

		QByteArray messageData = buffer.read( messageSize );

		QString name;
		rec::dataexchange::DataType type;
		rec::core::variant::Variant value;

		if( false == rec::dataexchange::messages::Data::decode( messageData, &name, &type, &value ) )
		{
			qDebug() << "Error decoding item " << i;
			return false;
		}

		if( name != items[i].name)
		{
			qDebug() << "Error compare names on item " << i << ": " << name << " != " << items[i].name;
			return false;
		}
		if( type != items[i].type )
		{
			qDebug() << "Error compare types on item " << i << ": " << rec::dataexchange::dataTypeName( type ) << " != " << rec::dataexchange::dataTypeName( items[i].type );
			return false;
		}
		if( rec::core::variant::Math::singleton().isNotEqual( value ,items[i].data ) )
		{
			qDebug() << "Error compare values on item " << i << ": " << QString::fromStdString( value.toString() ) << " != " << QString::fromStdString( items[i].data.toString() );
			return false;
		}
	}

	return true;
}

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	if( basicTest() )
	{
		MainWindow mw;
		mw.show();

		return app.exec();
	}
	else
	{
		return 1;
	}
}
