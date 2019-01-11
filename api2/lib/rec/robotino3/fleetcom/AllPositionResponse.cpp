#include "rec/robotino3/fleetcom/AllPositionResponse.h"

using namespace rec::robotino3::fleetcom;

AllPositionResponse::AllPositionResponse( const QString& message, unsigned int sequence )
	: Response( Response::AllPositionResponseId, sequence )
{
	QStringList posList = message.split( ",", QString::SkipEmptyParts );

	Q_FOREACH( const QString& str, posList )
	{
		QStringList paraList = str.split( QRegExp("\\s"), QString::SkipEmptyParts);

		if( 6 == paraList.length() )
		{
			Position p;

			bool success = true;
			bool ok;

			p.id = paraList[0].toInt(&ok);
			success &= ok;

			p.x = paraList[1].toDouble(&ok);
			success &= ok;

			p.y = paraList[2].toDouble(&ok);
			success &= ok;

			p.phi = paraList[3].toDouble(&ok);
			success &= ok;

			p.numBelts = paraList[4].toInt(&ok);
			success &= ok;

			p.distance = paraList[5].toDouble(&ok);
			success &= ok;

			if( success )
			{
				_positions << p;
			}
		}
	}

	qSort( _positions );
}

QString AllPositionResponse::print() const
{
	QString str;

	str += "AllPosition";

	Q_FOREACH( const Position& p, _positions )
	{
		str += QString( "\n\rid:%1 x:%2m y:%3m phi:%4deg numBelts:%5 distance:%6m" ).arg( p.id ).arg( p.x ).arg( p.y ).arg( p.phi ).arg( p.numBelts ).arg( p.distance );
	}

	return str;
}