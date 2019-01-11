#include "rec/robotino3/fleetcom/FleetStateResponse.h"

using namespace rec::robotino3::fleetcom;

FleetStateResponse::FleetStateResponse( const QString& message, unsigned int sequence )
	: Response( Response::FleetStateResponseId, sequence )
{
	QStringList list = message.split(" ", QString::SkipEmptyParts);

	RobotinoState rs;

	Q_FOREACH( const QString& listItem, list )
	{
		QStringList items = listItem.split( ":",  QString::SkipEmptyParts );
		if( 2 == items.count() )
		{
			QString key = items.first();
			if( "robotinoid" == key )
			{
				bool ok;
				int value = items.last().toInt( &ok );
				if( ok )
				{
					if( rs.id > -1 )
					{
						_fleetState.states << rs;
					}										
                    rs = RobotinoState();
                    rs.id = value;
				}
			}
			else if( "x" == key )
			{
				bool ok;
				double value = items.last().toDouble( &ok );

				if( ok )
				{
					rs.x = value;
				}
			}
			else if( "y" == key )
			{
				bool ok;
				double value = items.last().toDouble( &ok );

				if( ok )
				{
					rs.y = value;
				}
			}
			else if( "phi" == key )
			{
				bool ok;
				double value = items.last().toDouble( &ok );

				if( ok )
				{
					rs.phi = value;
				}
			}
			else if( "batteryvoltage" == key )
			{
				bool ok;
				double value = items.last().toDouble( &ok );

				if( ok )
				{
					rs.batteryVoltage = value;
				}
			}
			else if ("ipaddress" == key)
			{
				rs.ipAddress = items.last();
			}
		}
	}

	if( rs.id > -1 )
	{
		_fleetState.states << rs;
	}
}

QString FleetStateResponse::print() const
{
	QString str;

	str += "FleetState";

	Q_FOREACH( const RobotinoState& rs, _fleetState.states )
	{
		str += QString( "\n\rid:%1 x:%2m y:%3m phi:%4deg bat:%5V ipaddress:%6" ).arg( rs.id ).arg( rs.x ).arg( rs.y ).arg( rs.phi ).arg( rs.batteryVoltage ).arg( rs.ipAddress );
	}

	return str;
}
