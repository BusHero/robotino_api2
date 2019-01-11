#include "rec/robotino3/fleetcom/JobInfoResponse.h"

using namespace rec::robotino3::fleetcom;

JobInfoResponse::JobInfoResponse(const QString& message, unsigned int sequence)
	: Response(Response::JobInfoResponseId, sequence)
{
	QStringList l = message.split(" ", QString::SkipEmptyParts);
	Q_FOREACH(const QString& s, l)
	{
		QStringList sl = s.split(":", QString::SkipEmptyParts);
		if (2==sl.count())
		{
			QString key = sl.first();
			QString value = sl.last();
			if (0==key.compare("robotinoid", Qt::CaseInsensitive))
			{
				robotinoId = value.toInt();
			}
			else if (0 == key.compare("jobid", Qt::CaseInsensitive))
			{
				jobId = value.toInt();
			}
			else if (0 == key.compare("priority", Qt::CaseInsensitive))
			{
				priority = value.toInt();
			}
			else if (0 == key.compare("fromStation", Qt::CaseInsensitive))
			{
				fromStation = value.toInt();
			}
			else if (0 == key.compare("fromBelt", Qt::CaseInsensitive))
			{
				fromBelt = value.toInt();
			}
			else if (0 == key.compare("toStation", Qt::CaseInsensitive))
			{
				toStation = value.toInt();
			}
			else if (0 == key.compare("toBelt", Qt::CaseInsensitive))
			{
				toBelt = value.toInt();
			}
			else if (0 == key.compare("state", Qt::CaseInsensitive))
			{
				state = value;
			}
		}
	}
}

QString JobInfoResponse::print() const
{
	QString str = QString("JobInfo robotinoid:%1 jobid:%2 priority:%3 fromStation:%4 fromBelt:%5 toStation:%6 toBelt:%7 state:%8").arg(robotinoId).arg(jobId).arg(priority).arg(fromStation).arg(fromBelt).arg(toStation).arg(toBelt).arg(state);

	return str;
}
