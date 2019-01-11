#include "rec/robotino3/fleetcom/Factory.h"
#include "rec/robotino3/fleetcom/InvalidResponse.h"
#include "rec/robotino3/fleetcom/FleetStateResponse.h"
#include "rec/robotino3/fleetcom/AllPositionResponse.h"
#include "rec/robotino3/fleetcom/AllRobotinoIdResponse.h"
#include "rec/robotino3/fleetcom/RobotFleetTypeResponse.h"
#include "rec/robotino3/fleetcom/MyRobotinoIdResponse.h"
#include "rec/robotino3/fleetcom/JobInfoResponse.h"
#include "rec/robotino3/fleetcom/RobotInfoResponse.h"
#include "rec/robotino3/fleetcom/MapListResponse.h"
#include "rec/robotino3/fleetcom/DownloadMapResponse.h"
#include "rec/robotino3/fleetcom/LoadPathResponse.h"
#include "rec/robotino3/fleetcom/ErrorResponse.h"

#include "rec/robotino3/fleetcom/InvalidRequest.h"
#include "rec/robotino3/fleetcom/GetMapListRequest.h"
#include "rec/robotino3/fleetcom/SetDefaultMapRequest.h"
#include "rec/robotino3/fleetcom/UploadMapRequest.h"
#include "rec/robotino3/fleetcom/DownloadMapRequest.h"
#include "rec/robotino3/fleetcom/RenameMapRequest.h"
#include "rec/robotino3/fleetcom/DeleteMapRequest.h"
#include "rec/robotino3/fleetcom/SavePathRequest.h"
#include "rec/robotino3/fleetcom/LoadPathRequest.h"

using namespace rec::robotino3::fleetcom;

ResponsePointer Factory::parse(const QString& message, unsigned int sequence)
{
	return Factory::parse(message, QVariantMap(), sequence);
}

ResponsePointer Factory::parse(const QString& message, const QVariantMap& pairs, unsigned int sequence)
{
	int pos = message.indexOf( QRegExp( "\\s" ) );
	QString messageWithoutCommand = message.mid( pos+1 );
	messageWithoutCommand = messageWithoutCommand.trimmed();

	if( message.startsWith( "FleetState", Qt::CaseInsensitive ) )
	{
		FleetStateResponsePointer r( new FleetStateResponse( messageWithoutCommand, sequence ) );
		return r;
	}
	else if(  message.startsWith( "AllPosition", Qt::CaseInsensitive ) )
	{
		AllPositionResponsePointer p( new AllPositionResponse( messageWithoutCommand, sequence ) );
		return p;
	}
	else if(  message.startsWith( "AllRobotinoID", Qt::CaseInsensitive ) )
	{
		AllRobotinoIdResponsePointer p( new AllRobotinoIdResponse( messageWithoutCommand, sequence ) );
		return p;
	}
	else if(  message.startsWith( "RobotFleetType", Qt::CaseInsensitive ) )
	{
		RobotFleetTypeResponsePointer p( new RobotFleetTypeResponse( messageWithoutCommand, sequence ) );
		return p;
	}
	else if (message.startsWith("MyRobotinoId", Qt::CaseInsensitive))
	{
		MyRobotinoIdResponsePointer p(new MyRobotinoIdResponse(messageWithoutCommand, sequence));
		return p;
	}
	else if (message.startsWith("JobInfo", Qt::CaseInsensitive))
	{
		JobInfoResponsePointer p(new JobInfoResponse(messageWithoutCommand, sequence));
		return p;
	}
	else if (message.startsWith("RobotInfo", Qt::CaseInsensitive))
	{
		RobotInfoResponsePointer p(new RobotInfoResponse(messageWithoutCommand, sequence));
		return p;
	}
	else if (message.startsWith("MapList", Qt::CaseInsensitive))
	{
		MapListResponsePointer p(new MapListResponse(messageWithoutCommand,pairs,sequence));
		return p;
	}
	else if (message.startsWith("DownloadMapResponse", Qt::CaseInsensitive))
	{
		DownloadMapResponsePointer p(new DownloadMapResponse(messageWithoutCommand, pairs, sequence));
		return p;
	}
    else if (message.startsWith("LoadPathResponse", Qt::CaseInsensitive))
    {
        LoadPathResponsePointer p(new LoadPathResponse(messageWithoutCommand, pairs, sequence));
        return p;
    }
	else if (message.startsWith("ErrorResponse", Qt::CaseInsensitive))
	{
		ErrorResponsePointer p(new ErrorResponse(messageWithoutCommand, pairs, sequence));
		return p;
	}
	else
	{
		InvalidResponsePointer p( new InvalidResponse( message, sequence ) );
		return p;
	}
}

RequestPointer Factory::parseRequest(const QString& message, const QVariantMap& pairs, unsigned int sequence)
{
	int pos = message.indexOf(QRegExp("\\s"));
	QString messageWithoutCommand = message.mid(pos + 1);
	messageWithoutCommand = messageWithoutCommand.trimmed();

	if (message.startsWith("GetMapList", Qt::CaseInsensitive))
	{
		GetMapListRequestPointer r(new GetMapListRequest);
		return r;
	}
	else if (message.startsWith("SetDefaultMap", Qt::CaseInsensitive))
	{
		SetDefaultMapRequestPointer r(new SetDefaultMapRequest(pairs));
		return r;
	}
	else if (message.startsWith("UploadMap", Qt::CaseInsensitive))
	{
		UploadMapRequestPointer r(new UploadMapRequest(pairs));
		return r;
	}
	else if (message.startsWith("DownloadMap", Qt::CaseInsensitive))
	{
		DownloadMapRequestPointer r(new DownloadMapRequest(pairs));
		return r;
	}
	else if (message.startsWith("RenameMap", Qt::CaseInsensitive))
	{
		RenameMapRequestPointer r(new RenameMapRequest(pairs));
		return r;
	}
	else if (message.startsWith("DeleteMap", Qt::CaseInsensitive))
	{
		DeleteMapRequestPointer r(new DeleteMapRequest(pairs));
		return r;
	}
    else if (message.startsWith("SavePath", Qt::CaseInsensitive))
    {
        SavePathRequestPointer r(new SavePathRequest(pairs));
        return r;
    }
    else if (message.startsWith("LoadPath", Qt::CaseInsensitive))
    {
        LoadPathRequestPointer r(new LoadPathRequest());
        return r;
    }
	else
	{
		InvalidRequestPointer p(new InvalidRequest(message,pairs, sequence));
		return p;
	}
}
