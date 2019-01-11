#include "NetworkConfig.h"
#include "utils.h"

QByteArray createNetworkConfigData()
{
	QJsonArray arrayObj;
	
	int id=1;
	Q_FOREACH(const QString& iname, QStringList() << "eth0" << "eth0:10" << "wlan0" )
	{
		QJsonObject ifaceObj;
		ifaceObj["name"] = iname;
		
		if("eth0"==iname)
		{
			
		}
	}
	
	QJsonDocument doc(arrayObj);
	QByteArray data = doc.toJson();
	
	return data;
}