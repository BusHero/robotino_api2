#include "rec/robotino3/fleetcom/RobotInfoResponse.h"

using namespace rec::robotino3::fleetcom;

RobotInfoResponse::RobotInfoResponse(const QString& message, unsigned int sequence)
	: Response(Response::RobotInfoResponseId, sequence)
{
	QStringList l = message.split(" ", QString::SkipEmptyParts);
	Q_FOREACH(const QString& s, l)
	{
		QStringList sl = s.split(":", QString::SkipEmptyParts);
		if (2 == sl.count())
		{
			QString key = sl.first();
			QString value = sl.last();
			if (0 == key.compare("robotinoid", Qt::CaseInsensitive))
			{
				robotinoId = value.toInt();
			}
			else if (0 == key.compare("x", Qt::CaseInsensitive))
			{
				x = value.toFloat();
			}
			else if (0 == key.compare("y", Qt::CaseInsensitive))
			{
				y = value.toFloat();
			}
			else if (0 == key.compare("phi", Qt::CaseInsensitive))
			{
				phi = value.toFloat();
			}
			else if (0 == key.compare("batteryVoltage", Qt::CaseInsensitive))
			{
				batteryVoltage = value.toFloat();
			}
			else if (0 == key.compare("laserWarning", Qt::CaseInsensitive))
			{
				laserWarning = ((0==value.compare("true",Qt::CaseInsensitive)||"1"==value)?true:false);
			}
			else if (0 == key.compare("laserSafety", Qt::CaseInsensitive))
			{
				laserSafety = ((0 == value.compare("true", Qt::CaseInsensitive) || "1" == value) ? true : false);
			}
			else if (0 == key.compare("boxPresent", Qt::CaseInsensitive))
			{
				boxPresent = ((0 == value.compare("true", Qt::CaseInsensitive) || "1" == value) ? true : false);
			}
			else if (0 == key.compare("state", Qt::CaseInsensitive))
			{
				state = value;
			}
		}
	}
}

QString RobotInfoResponse::print() const
{
	QString str = QString("RobotInfo robotinoid:%1 x:%2 y:%3 phi:%4 batteryVoltage:%5 laserWarning:%6 laserSafety:%7 boxPresent:%8 state:%9")
		.arg(robotinoId)
		.arg(x)
		.arg(y)
		.arg(phi)
		.arg(batteryVoltage)
		.arg(laserWarning?"true":"false")
		.arg(laserSafety ? "true" : "false")
		.arg(boxPresent ? "true" : "false")
		.arg(state);

	return str;
}