#include "rec/robotino3/fleetcom/RobotFleetTypeResponse.h"

using namespace rec::robotino3::fleetcom;

RobotFleetTypeResponse::RobotFleetTypeResponse( const QString& message, unsigned int sequence )
	: Response( Response::RobotFleetTypeResponseId, sequence )
	, _robotinoId( -1 )
	, _isMaster( false )
{
	QStringList l = message.split( QRegExp( "\\s" ), QString::SkipEmptyParts );
	if( 2 == l.count() )
	{
		bool ok;
		int rid = l.at(0).toInt( &ok );
		if( ok )
		{
			_robotinoId = rid;

			if( "master" == l.at(1).toLower() )
			{
				_isMaster = true;
			}
		}
	}
}

QString RobotFleetTypeResponse::print() const
{
	QString str = QString( "RobotFleetType %1 %2" ).arg( _robotinoId ).arg( (_isMaster?"master":"slave") );

	return str;
}