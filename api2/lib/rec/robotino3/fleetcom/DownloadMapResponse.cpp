#include "rec/robotino3/fleetcom/DownloadMapResponse.h"

using namespace rec::robotino3::fleetcom;

DownloadMapResponse::DownloadMapResponse(const QString& message, const QVariantMap& pairs, unsigned int sequence)
	: Response( Response::DownloadMapResponseId, sequence )
{
	_data = pairs.value("data").toByteArray();
	_mapName = pairs.value("mapName").toString();
}

DownloadMapResponse::DownloadMapResponse(const QByteArray& data, const QString& mapName)
	: Response(Response::DownloadMapResponseId, sequence)
	, _data(data)
	, _mapName(mapName)
{
}

QString DownloadMapResponse::print() const
{
	QString str = QString("DownloadMap dataSize=%1 mapeName=%2").arg(_data.size()).arg(_mapName);

	return str;
}