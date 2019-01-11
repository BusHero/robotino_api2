#include "rec/robotino3/fleetcom/LoadPathResponse.h"

using namespace rec::robotino3::fleetcom;

LoadPathResponse::LoadPathResponse(const QString& message, const QVariantMap& pairs, unsigned int sequence)
	: Response( Response::DownloadMapResponseId, sequence )
{
	_data = pairs.value("data").toByteArray();
}

LoadPathResponse::LoadPathResponse(const QByteArray& data)
    : Response(Response::LoadPathResponseId, sequence)
    , _data(data)
{
}

QString LoadPathResponse::print() const
{
    QString str = QString("LoadPath dataSize=%1").arg(_data.size());

    return str;
}
