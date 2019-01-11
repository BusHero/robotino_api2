#ifndef _FCGI_H_
#define _FCGI_H_

#include <QtCore>
#include "Client.h"
#include <QTime>
#include "ImageHandler.h"
#include "ControlHandler.h"
#include "SensorDataHandler.h"
#include <QFile>
#include <QTextStream>
#include "ConfigPageBuilder.h"
#include "ConfigEditor.h"

/**
 * FCGID core.
 * Run the request-handling infinite loop.
 * @author Tianlan Shao
 */
class Fcgi : public QThread
{
	Q_OBJECT
private:
	void dumpFile(const char* filename);
public:
	enum ResponseType{
		SUCCESS = 0,
		NO_JSON_OBJECT_RECIEVED = -1,
		DATA_RESPONSE = 1,
		TYPE_FIELD_NOT_FOUND = 2,
		UNKNOWN_COMMAND = 3,
		MISSING_PARAMETER = 4,
		INVALID_PARAMETER = 5,
		NO_RUNNING_PROGRAM = 6,
		OTHER_ERROR = 7,
		INVALID_FILE_NAME = 8
	};

    struct node_coord
    {
        double x;
        double y;
    };

    Fcgi();

	void stop();


private Q_SLOTS:
	void on_clientConnected();
	void on_clientDisconnected();
	void on_uploadProgramsChanged(const QString& /*path*/);

private:
	void run();

	bool isValidFilename(QString name) const;

	ImageHandler* _imageHandler;
	SensorDataHandler* _sensorDataHandler;
	ControlHandler* _controlHandler;

	QImage _nocamImg;//image when camera image not available
	QByteArray _nocamImgData;

	bool _nocamImgLoaded;
	QImage _nomapImg;
	bool _nomapImgLoaded;

	QString _latestProgram;
	QString _robviewPath;

	ConfigPageBuilder configXMLBuilder;
	
	QFileSystemWatcher* _uploadProgramsWatcher;

    //QMap<QString, QString> settingMap;//TODO for test settings
};


#endif
