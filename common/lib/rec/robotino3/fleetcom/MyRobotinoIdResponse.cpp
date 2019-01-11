#include "rec/robotino3/fleetcom/MyRobotinoIdResponse.h"

using namespace rec::robotino3::fleetcom;

MyRobotinoIdResponse::MyRobotinoIdResponse( const QString& message, unsigned int sequence )
	: Response( Response::MyRobotinoIdResponseId, sequence )
{
	bool ok;
	_robotinoId = message.toInt( &ok );

	if( false == ok )
	{
		_robotinoId = -1;
	}
}

QString MyRobotinoIdResponse::print() const
{
	QString str = QString( "MyRobotinoId %1" ).arg( _robotinoId );

	return str;
}