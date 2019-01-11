#include "MainWindow.h"
#include "rec/core/LaserRangeFinderData.h"
#include <cmath>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	//QRegExp reg( "((\\-{0,1}[0-9]+(\\.{0,1}\\d*){0,1})\\s*){2,3}" );

	//QString str = "1234.67  3467.45 -3.2";

	//bool ret = reg.exactMatch( str );

	//qDebug() << (ret?"matched":"not matched");
	//qDebug() << reg.matchedLength();

	//return 0;

	rec::core::LaserRangeFinderData data0;
	data0.measuresVector.resize( 200 );
	for( int i=0; i<data0.measuresVector.size(); ++i )
	{
		data0.measuresVector[i] = static_cast< boost::uint16_t >( 100 * sin( (float)i ) );
	}

	const int bufferSize = data0.encodedDataSize();
	QByteArray buffer( bufferSize, 0 );

	data0.encode( buffer.data() );

	//rec::core::LaserRangeFinderData data1( -0.56f, 0.03974, 512, 1000 );

	rec::core::LaserRangeFinderData data1( buffer.constData(), bufferSize );

	if( data0 != data1 )
	{
		return 1;
	}

	MainWindow mw;
	mw.show();

	return app.exec();
}
