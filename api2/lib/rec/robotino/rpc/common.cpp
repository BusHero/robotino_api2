#include "rec/robotino/rpc/common.h"
#include "rec/robotino/rpc/GrapplerServoInfo.h"
#include "rec/robotino/rpc/ProcessStatus.h"
#include "rec/robotino/rpc/ProcessOutput.h"
#include "rec/rpc/Exception.h"
#include <QtCore>

typedef QMap<QString, QVector<QSize> > resolutions_t;

Q_DECLARE_METATYPE( QVector<float> )
Q_DECLARE_METATYPE( QVector<int> )
Q_DECLARE_METATYPE( QList< QVariant > )
Q_DECLARE_METATYPE( resolutions_t )
Q_DECLARE_METATYPE( rec::rpc::Exception )

void rec::robotino::rpc::once()
{
	static bool isInit = false;

	if( isInit )
	{
		return;
	}

	qRegisterMetaType< QVector<float> >();
	qRegisterMetaType< QVector<int> >();
	qRegisterMetaType< QList< QVariant > >();
	qRegisterMetaType< resolutions_t >();
	qRegisterMetaType< rec::rpc::Exception >();
	qRegisterMetaType< rec::robotino::rpc::GrapplerServoInfo >();
	qRegisterMetaType< rec::robotino::rpc::ProcessStatus >();
	qRegisterMetaType< rec::robotino::rpc::ProcessOutput >();

	isInit = true;
}
