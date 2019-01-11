#include "rec/robotino3/fleetcom/JobErrorResponse.h"

using namespace rec::robotino3::fleetcom;

JobErrorResponse::JobErrorResponse(const QString& message, unsigned int sequence)
	: Response(Response::JobErrorResponseId, sequence)
{
}

QString JobErrorResponse::print() const
{
	QString str;

	return str;
}
