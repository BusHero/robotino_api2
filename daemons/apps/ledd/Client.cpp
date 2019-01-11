#include "Client.h"

Client::Client()
{
	bool connected = true;
	connected &= (bool)QObject::connect( this, SIGNAL( json_message_with_data_changed(int, const QByteArray&, const QByteArray&) ), SLOT( on_json_message_with_data_changed(int, const QByteArray&, const QByteArray&) ) );
	
	Q_ASSERT( connected );

	setAutoReconnectEnabled( true );
	setTopicEnabled("ledd",true);
}

void Client::on_json_message_with_data_changed(int topic, const QByteArray& jsonData, const QByteArray& data)
{
	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(jsonData,&error);
	
	if(QJsonParseError::NoError != error.error)
	{
		qDebug() << "error:\n" << error.errorString();
		return;
	}
	
    QJsonObject root = doc.object();
	
	QString command = root["command"].toString();
	
	if("displaydata" == command)
	{
		QImage image = QImage::fromData(data);
		if(image.isNull())
		{
			qDebug() << "Invalid data";
		}
		else
		{
			Q_EMIT displayImage(image);
		}
	}
	else if("show" == command)
	{
		QString name = root["name"].toString();
		QString fullName = QString(":/images/%1.png").arg( name );
		
		QImage image(fullName);
		if(image.isNull())
		{
			qDebug() << "Image with name " << name << " not found";
		}
		else
		{
			Q_EMIT displayImage(image);
		}
	}
}