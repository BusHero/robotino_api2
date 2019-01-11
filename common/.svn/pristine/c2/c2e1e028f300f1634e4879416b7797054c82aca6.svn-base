#include "rec/robotino3/fleetcom/AllRobotinoIdResponse.h"

using namespace rec::robotino3::fleetcom;

AllRobotinoIdResponse::AllRobotinoIdResponse( const QString& message, unsigned int sequence )
	: Response( Response::AllRobotinoIdResponseId, sequence )
{
	QStringList l = message.split( QRegExp( "\\s" ), QString::SkipEmptyParts );
	Q_FOREACH( const QString& str, l )
	{
		bool ok;
		int rid = str.toInt( &ok );

		if( ok )
		{
			_allRobotinoIds << rid;
		}
	}
}

QString AllRobotinoIdResponse::print() const
{
	QString str = "AllRobotinoId";

	Q_FOREACH( int i, _allRobotinoIds )
	{
		str += QString(" %1").arg( i );
	}

	return str;
}