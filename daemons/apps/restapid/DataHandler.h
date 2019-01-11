#ifndef _DATAHANDLER_H_
#define _DATAHANDLER_H_

#include <QtCore>

class DataHandler
{
public:
	DataHandler();
	~DataHandler();

	QByteArray data(const QString& path);
	
	bool setData(const QString& path, const QByteArray& data, QByteArray& response);
private:
};

#endif //_DATAHANDLER_H_