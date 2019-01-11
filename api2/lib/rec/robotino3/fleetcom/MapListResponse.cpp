#include "rec/robotino3/fleetcom/MapListResponse.h"

using namespace rec::robotino3::fleetcom;

MapListResponse::MapListResponse(const QString& message, const QVariantMap& pairs, unsigned int sequence)
	: Response( Response::MapListResponseId, sequence )
{
	_mapList = pairs.value("maplist").toStringList();
}

MapListResponse::MapListResponse(const QStringList& mapList)
	: Response(Response::MapListResponseId, sequence)
	, _mapList(mapList)
{

}


void MapListResponse::encode(QString* message, QVariantMap* pairs) const
{
	*message = "MapList";
	(*pairs)["maplist"] = _mapList;
}

QString MapListResponse::print() const
{
	QString str;

	str += "MapList";

	Q_FOREACH( const QString& name, _mapList )
	{
		str += QString("\n\r%1").arg(name);
	}

	return str;
}