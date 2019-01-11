#include "Fcgi.h"

#include "fcgi_stdio.h"
#include <jansson.h>
#include <algorithm>
#include <stdlib.h>
#include <QDir>
#include <QFileInfoList>

#include "rec/robotino/daemons/Log.h"

#include "ScriptRunner.h"
#include "Client.h"
#include "utils.h"
#include "UdpLogger.h"
#include "ShapeData.h"
#include "MapEditor.h"

#define PROGRAMS_DIR "/var/www/programs"

Fcgi::Fcgi()
    : _imageHandler( new ImageHandler( this ) )
    , _controlHandler( new ControlHandler( this ) )
    ,_sensorDataHandler(new SensorDataHandler( this ))
	, _uploadProgramsWatcher(new QFileSystemWatcher(this))
{
    Client::singleton()->setTopicsEnabled(true);

    bool success = true;
    int connectionCheckCode = 0;//This code is for checking which connections are failed

    success = connect( Client::singleton(), SIGNAL( is_connected() ), SLOT( on_clientConnected() ) );
    if(!success){connectionCheckCode += (1<<0);}

    success = connect( Client::singleton(), SIGNAL( is_disconnected() ), SLOT( on_clientDisconnected() ) );
    if(!success){connectionCheckCode += (1<<1);}

    success = connect( Client::singleton(), SIGNAL( distance_sensors_changed( const QVector< float >& ) )
                       , _sensorDataHandler, SLOT( on_distance_sensors_changed( const QVector< float >& ) ) );
    if(!success){connectionCheckCode += (1<<3);}
    success = connect( Client::singleton(), SIGNAL( bumper_changed( bool ) ), _sensorDataHandler, SLOT( on_bumper_changed( bool ) ) );
    if(!success){connectionCheckCode += (1<<4);}

    success = connect( Client::singleton(), SIGNAL( battery_changed( float, float, bool , int , const QString& , bool , int ) ),
                       _sensorDataHandler, SLOT( on_battery_changed( float, float, bool , int , const QString& , bool , int ) ) );
    if(!success){connectionCheckCode += (1<<5);}

    success = connect( Client::singleton(), SIGNAL( charger0_info_changed( unsigned int, float, float, float, float, int, const QString& )),
                       _sensorDataHandler, SLOT( on_charger0_info_changed( unsigned int, float, float, float, float, int, const QString& ) ) );
    if(!success){connectionCheckCode += (1<<6);}

    success = connect( Client::singleton(), SIGNAL( charger1_info_changed( unsigned int, float, float, float, float, int, const QString& ) ),
                       _sensorDataHandler, SLOT( on_charger1_info_changed( unsigned int, float, float, float, float, int, const QString& ) ) );
    if(!success){connectionCheckCode += (1<<7);}

    success = connect( Client::singleton(), SIGNAL( charger2_info_changed( unsigned int, float, float, float, float, int, const QString& ) ),
                       _sensorDataHandler, SLOT( on_charger2_info_changed( unsigned int, float, float, float, float, int, const QString& ) ) );
    if(!success){connectionCheckCode += (1<<8);}

    success = connect( Client::singleton(), SIGNAL( digital_input_changed(const QVector<bool>&)), _sensorDataHandler,
                       SLOT( on_digitalinput_changed(const QVector<bool>&)) );
    if(!success){connectionCheckCode += (1<<12);}

    success = connect( Client::singleton(), SIGNAL( analog_input_changed(const QVector<float>&)), _sensorDataHandler,
                       SLOT( on_analoginput_changed(const QVector<float>&)) );
    if(!success){connectionCheckCode += (1<<13);}

    success = connect( Client::singleton(), SIGNAL( set_digital_output_array_changed(const QVector<bool>&)), _sensorDataHandler,
                       SLOT( on_digitaloutput_changed(const QVector<bool>&)) );
    if(!success){connectionCheckCode += (1<<14);}

    success = connect( Client::singleton(), SIGNAL( set_relay_array_changed(const QVector<bool>&)), _sensorDataHandler,
                       SLOT( on_relay_changed(const QVector<bool>&)) );
    if(!success){connectionCheckCode += (1<<15);}

    success = connect( Client::singleton(), SIGNAL( charger0_version_changed( int, int, int )),
                       _sensorDataHandler, SLOT( on_charger0_version_info_changed( int, int, int ) ) );
    if(!success){connectionCheckCode += (1<<16);}

    success = connect( Client::singleton(), SIGNAL( charger1_version_changed( int, int, int )),
                       _sensorDataHandler, SLOT( on_charger1_version_info_changed( int, int, int ) ) );
    if(!success){connectionCheckCode += (1<<17);}

    success = connect( Client::singleton(), SIGNAL( charger2_version_changed( int, int, int )),
                       _sensorDataHandler, SLOT( on_charger2_version_info_changed( int, int, int ) ) );
    if(!success){connectionCheckCode += (1<<18);}

    if(0 != connectionCheckCode)
    {
        rec::robotino::daemons::Log::singleton()->log( QString("ERROR:SOME SIGNAL-SLOT CONNECT FAILED! connectionCheckCode = %1").arg(connectionCheckCode), 0 );
    }

    _nocamImgLoaded = _nocamImg.load("./resources/nocam.png");
    if(_nocamImgLoaded)
    {
        QBuffer bf(&_nocamImgData);
        _nocamImg.save( &bf, "png");
    }
    _nomapImgLoaded = _nomapImg.load("./resources/nomapimg.png");

    QString programDir = Client::singleton()->getParameter( "~/Robview/programpath", "/home/robotino/programs" ).toString();
    QFileInfo dirInfo(programDir);
    if(!dirInfo.exists())
    {
        QDir rootDir = QDir::root();
        if(rootDir.mkpath(programDir))
        {
            QFile::setPermissions(programDir,
                                  QFile::ExeGroup | QFile::ExeOther | QFile::ExeOwner |
                                  QFile::WriteGroup | QFile::WriteOther | QFile::WriteOwner |
                                  QFile::ReadGroup | QFile::ReadOther | QFile::ReadOwner);
        }
        else
        {
            rec::robotino::daemons::Log::singleton()->log( "ERROR: Cannot Create path "+ programDir + " to save roboview programs!", 0 );
        }
    }
    else if(!dirInfo.isDir())
    {
        rec::robotino::daemons::Log::singleton()->log( "ERROR: "+ programDir + " already exists and is not a folder!", 0 );
    }

    QFileInfo linkFileInfo(PROGRAMS_DIR);
    if(linkFileInfo.exists())
    {
        rec::robotino::daemons::Log::singleton()->log("Symbolic /var/www/programs already exists. Will delete it and create a new one.", 0);
        if(!QFile::remove(PROGRAMS_DIR))
        {
            rec::robotino::daemons::Log::singleton()->log( "ERROR: Cannot delete symbolic link \"/var/www/programs\"", 0 );
        }
    }
    if(!QFile::link(programDir,PROGRAMS_DIR))
    {
        rec::robotino::daemons::Log::singleton()->log( "ERROR: Cannot create symbolic link \"/var/www/programs\"", 0 );
    }
    QFile::setPermissions(PROGRAMS_DIR,
                          QFile::ExeGroup | QFile::ExeOther | QFile::ExeOwner |
                          QFile::WriteGroup | QFile::WriteOther | QFile::WriteOwner |
                          QFile::ReadGroup | QFile::ReadOther | QFile::ReadOwner);
    _robviewPath = Client::singleton()->getParameter( "~/Robview/path", "/usr/local/RobotinoView2/bin/robview2_interpreter" ).toString();


    //get paths for .ini and .conf files
    QString ini_DIR_str = Client::singleton()->getParameter( "~/ConfigPage/directories", "/opt/smartsoft/etc;/etc/robotino" ).toString();
    QStringList ini_DIRS = ini_DIR_str.split(";", QString::SkipEmptyParts);

    configXMLBuilder.setINI_DIR(ini_DIRS);
    configXMLBuilder.rebuild();
	
	QDir tempDir("/tmp");
	tempDir.mkdir("upload_programs");
	
	_uploadProgramsWatcher->addPath("/tmp/upload_programs");
	success = connect(_uploadProgramsWatcher, SIGNAL(directoryChanged(const QString& /*path*/)), SLOT(on_uploadProgramsChanged(const QString& /*path*/)));
	Q_ASSERT(success);
}

void Fcgi::on_clientConnected()
{
	Client::singleton()->setTopicsEnabled( true );

}

void Fcgi::on_clientDisconnected()
{
}

void Fcgi::on_uploadProgramsChanged(const QString& path)
{
	rec::robotino::daemons::Log::singleton()->log( path + " changed", 0 );
	
	QDir uploadDir(path);
	QFileInfoList fileList = uploadDir.entryInfoList(QDir::Files);
	Q_FOREACH(const QFileInfo& info, fileList)
	{
		QString logStr = "Moving " + info.absoluteFilePath() + " to " + PROGRAMS_DIR + " ... ";
		if( 0 == QProcess::execute("sudo", QStringList() << "mv" << info.absoluteFilePath() << PROGRAMS_DIR) )
		{
			logStr += "success";
		}
		else
		{
			logStr += "failed";
		}
		rec::robotino::daemons::Log::singleton()->log( logStr, 0 );
	}
}
/*
 * Output a binary file to stdout.
 */
void Fcgi::dumpFile(const char* filename)
{
	FILE * pFile;
	long lSize;
	char * buffer;
	size_t result;

	pFile = fopen ( filename, "rb");
	if (pFile==NULL) {return;}

	// obtain file size:
	fseek (pFile , 0 , SEEK_END);
	lSize = ftell (pFile);
	rewind (pFile);

	buffer = (char*) malloc (sizeof(char)*lSize);
	result = fread (buffer,1,lSize,pFile);

	/* the whole file is now loaded in the memory buffer. */

	// terminate

	fwrite (buffer, 1, lSize, stdout);
	fflush(stdout);
	fclose (pFile);
	free (buffer);
}

void Fcgi::stop()
{
	terminate();
}

bool Fcgi::isValidFilename(QString name) const
{
	return ( name.indexOf(QRegExp("^[_a-zA-Z0-9\\-\\.]+$")) == 0 );
}

void Fcgi::run()
{
    ResponseType responseType;

    while(FCGI_Accept() >= 0)
    {

        responseType = SUCCESS;

        const char *qstr = getenv("QUERY_STRING");
        if(qstr == NULL)
        {
            qstr = "NULL QUERY STRING";
        }


        json_t * responseJson = json_object();

        QString str(qstr);
        str.replace("%20"," ");
        str.replace("%22","\"");
        str.replace("%7B","{");
        str.replace("%7D","}");

        rec::robotino::daemons::Log::singleton()->log( "recieved query string:" + str, 4 );

        try
        {
            json_t *root;
            json_error_t error;
            root = json_loads(str.toLatin1(), 0, &error);
            bool isRoutine = false;
            if(!root)
            {
                responseType = NO_JSON_OBJECT_RECIEVED;
            }
            if(responseType == SUCCESS && json_is_object(root))
            {
                json_t *type;
                type = json_object_get(root, "TYPE");
                if(type == NULL)
                {
                    responseType = TYPE_FIELD_NOT_FOUND;
                }
                else
                {
                    QString requestType(json_string_value(type));
                    if(requestType == "getMenuItems" )
                    {
                        bool updateItem = Client::singleton()->getParameter( "~/Menu/updateItem", false ).toBool();
                        bool factory4Item = Client::singleton()->getParameter( "~/Menu/factory4Item", false ).toBool();
                        json_t* jMenus = json_array();
                        json_t* jMenu = json_object();
                        json_object_set_new(jMenu, "name", json_string("Update"));
                        json_object_set_new(jMenu, "sts", json_integer(updateItem));
                        json_array_append_new(jMenus, jMenu);
                        jMenu = json_object();
                        json_object_set_new(jMenu, "name", json_string("Map"));
                        json_object_set_new(jMenu, "sts", json_integer(factory4Item));
                        json_array_append_new(jMenus, jMenu);
                        json_object_set_new(responseJson,"DATA",jMenus);
                        responseType = DATA_RESPONSE;
                    }
                    else if(requestType =="getimage")
                    {
                        //QImage image = _imageHandler->getImage();
                        QByteArray imageData = _imageHandler->getJpeg();//format should be jpeg

                        QString imgFormat = "jpeg";

                        if(  (imageData.size() <= 0 || (_imageHandler->timeSinceLastImg() > 1000))
                             && _nocamImgLoaded )
                        {

                            imageData = _nocamImgData;
                            imgFormat = "png";
                        }

                        printf((QString("Content-Type: image/") + imgFormat + QString(" \r\n\r\n")).toLatin1());
                        fwrite (const_cast<char*>(imageData.constData()), 1, imageData.size(), stdout);

//                        json_decref(root);
//                        json_decref(responseJson);
                        continue;
                    }
                    else if(requestType =="changeRes")
                    {
                        QString res = QString(json_string_value(json_object_get(root,"res")));
                        if(res == NULL && res.length()==0)
                        {
                            responseType = MISSING_PARAMETER;
                        }
                        else if(!(res == "high" || res == "low"))
                        {
                            responseType = INVALID_PARAMETER;
                        }
                        else
                        {
                            if(res == "high" )
                            {
                                Client::singleton()->set_set_camera0_settings(640,480,"mjpg");
                                rec::robotino::daemons::Log::singleton()->log("change to high resolution",1);
                            }
                            else
                            {
                                Client::singleton()->set_set_camera0_settings(320,240,"mjpg");
                                rec::robotino::daemons::Log::singleton()->log("change to low resolution",1);
                            }
                        }
                    }
                    else if(requestType =="stop")
                    {
                        _controlHandler->stop();
                    }
                    else if(requestType =="velDrive")
                    {
                        json_t* vx_jr = json_object_get(root, "vx");
                        json_t* vy_jr = json_object_get(root, "vy");
                        json_t* omega_jr = json_object_get(root, "omega");
                        if(!vx_jr || !vy_jr || !omega_jr)
                        {
                            responseType = MISSING_PARAMETER;
                        }
                        else if(!json_is_number(vx_jr) || !json_is_number(vy_jr)
                                || !json_is_number(omega_jr))
                        {
                            responseType = INVALID_PARAMETER;
                        }
                        else
                        {
                            float vx = (float)json_number_value(vx_jr);
                            float vy = (float)json_number_value(vy_jr);
                            float omega = (float)json_number_value(omega_jr);

                            //TODO check parameter range
                            _controlHandler->addDeltaVel(vx,vy,omega);

                            char buf[48];
                            sprintf(buf,"velDrive(%1.3f,%1.3f,%1.3f)",vx,vy,omega);
                            json_object_set_new(responseJson, "MSG", json_string(buf));
                        }
                    }
                    else if(requestType =="positionDrive")
                    {
                        json_t* x_jr = json_object_get(root, "x");
                        json_t* y_jr = json_object_get(root, "y");
                        json_t* omega_jr = json_object_get(root, "omega");
                        json_t* vel_jr = json_object_get(root, "vel");
                        json_t* rotvel_jr = json_object_get(root, "rotvel");
                        json_t* mode_jr = json_object_get(root, "mode");

                        if(!x_jr || !y_jr || !omega_jr || !vel_jr
                                || !rotvel_jr || !mode_jr)
                        {
                            responseType = MISSING_PARAMETER;
                        }
                        else if(!json_is_number(x_jr) || !json_is_number(y_jr)
                                || !json_is_number(omega_jr)
                                || !json_is_number(vel_jr)
                                || !json_is_number(rotvel_jr)
                                || !json_is_integer(mode_jr))
                        {
                            responseType = INVALID_PARAMETER;
                        }
                        else
                        {
                            float x = (float)json_number_value(x_jr);
                            float y = (float)json_number_value(y_jr);
                            float omega = (float)json_number_value(omega_jr);
                            float vel = (float)json_number_value(vel_jr);
                            float rotvel = (float)json_number_value(rotvel_jr);
                            int mode = (int)json_number_value(mode_jr);

                            _controlHandler->startPositionMode(x,y,omega,vel,rotvel,mode);

                            char buf[128];
                            sprintf(buf,"positionDrive(%1.3f,%1.3f,%1.3f,%1.3f,%1.3f,%d)",x,y,omega,vel,rotvel,mode);
                            json_object_set_new(responseJson, "MSG", json_string(buf));
                        }
                    }
                    else if(requestType =="HEARTBEAT")
                    {
                        isRoutine = true;
                        _controlHandler->alive();
                    }

                    else if(requestType =="getProgramList")
                    {
                        QDir qdir(PROGRAMS_DIR);
                        QFileInfoList fileList = qdir.entryInfoList(QDir::Files);
                        json_t* fileNameArray = json_array();
                        for(int i = 0; i < fileList.length(); i++)
                        {
                            json_t * fileJObj = json_object();
                            QFileInfo fileInfo = fileList.at(i);
                            json_object_set_new(fileJObj, "fileName", json_string(fileInfo.fileName().toLatin1()));
                            json_object_set_new(fileJObj, "fileSize", json_integer(fileInfo.size()));
                            json_object_set_new(fileJObj, "filePath", json_string(fileInfo.filePath().toLatin1()));
                            json_array_append_new(fileNameArray,fileJObj);
                        }
                        json_object_set_new(responseJson, "DATA", fileNameArray);
                        responseType = DATA_RESPONSE;
                    }
                    else if(requestType =="deleteFile")
                    {
                        QString fileName = QString(json_string_value(json_object_get(root,"fileName")));
                        if(fileName == NULL || fileName.indexOf(QRegExp("^[_a-zA-Z0-9\\-\\.]+$"))!=0)
                        {
                            responseType = INVALID_FILE_NAME;
                        }
                        else
                        {
							QString filePath = "/var/www/programs/"+ fileName;
                            QFile file(filePath);
                            if(!file.exists())
                            {
                                responseType = OTHER_ERROR;
                                json_object_set_new(responseJson, "MSG", json_string(QString(fileName + "doesn't exist.").toLatin1()));
                            }
                            else
                            {
                                if(0 != QProcess::execute("sudo", QStringList() << "rm" << filePath ) )
                                {
                                    responseType = OTHER_ERROR;
                                    json_object_set_new(responseJson, "MSG", json_string(QString("Failed to delete " + fileName + ".").toLatin1()));
                                }
                            }

                        }
                    }
                    else if(requestType =="renameFile")
                    {
                        QString fileName = QString(json_string_value(json_object_get(root,"fileName")));
                        QString newName = QString(json_string_value(json_object_get(root,"newFileName")));
                        if(fileName == NULL || !isValidFilename(fileName) )
                        {
                            responseType = OTHER_ERROR;
                            json_object_set_new(responseJson, "MSG", json_string(QString("Invalid file name " + fileName).toLatin1()));
                        }
                        else if(newName == NULL || !isValidFilename(newName))
                        {
                            responseType = OTHER_ERROR;
                            json_object_set_new(responseJson, "MSG", json_string(QString("Invalid new file name " + newName).toLatin1()));
                        }
                        else
                        {
                            QString filePath = "/var/www/programs/"+ fileName;
                            QFile file(filePath);
                            if(!file.exists())
                            {
                                responseType = OTHER_ERROR;
                                json_object_set_new(responseJson, "MSG", json_string(QString(fileName + "doesn't exist.").toLatin1()));
                            }
                            else
                            {
								QString newFilePath = "/var/www/programs/"+ newName;
                                if(0 != QProcess::execute("sudo", QStringList() << "mv" << filePath << newFilePath ))
                                {
                                    responseType = OTHER_ERROR;
                                    json_object_set_new(responseJson, "MSG", json_string(QString("Failed to rename " + fileName + " to " + newName + ".").toLatin1()));
                                }
                            }

                        }
                    }
                    else if(requestType =="runProgram")
                    {
                        if(!_latestProgram.isEmpty())
                        {
                            rec::robotino::daemons::Log::singleton()->log("kill current process:" + _latestProgram,2);
                            ScriptRunner::singleton()->killTree(_robviewPath);
                        }
                        QString program = QString(json_string_value(json_object_get(root,"programName")));
                        if(program == NULL)
                        {
                            responseType = MISSING_PARAMETER;
                        }
                        else
                        {
                            QString fullPathToProgram = Client::singleton()->getParameter( "~/Robview/programpath", "/home/robotino/programs" ).toString();
                            fullPathToProgram += "/";
                            fullPathToProgram += program;
                            QStringList arguments = QStringList() << "-f" << fullPathToProgram;

                            int interval =  Client::singleton()->getParameter( "~/Robview/globalVariableOutputInterval", 0 ).toInt();
                            if( interval > 0 )
                            {
                                arguments << "--global-variable-output-interval";
                                arguments << QString("%1").arg(interval);
                            }

                            ScriptRunner::singleton()->runScript(_robviewPath , arguments );
                            _latestProgram = program;
                            //ScriptRunner::singleton()->runScript("bash", arguments );//FIXME

                        }
                    }

                    else if(requestType =="killProcess")
                    {
                        //if(!_latestProgram.isEmpty() && ScriptRunner::singleton()->hasProcess(_robviewPath))
                        {
                            ScriptRunner::singleton()->killTree(_robviewPath);
                            _latestProgram = "";
                        }
                        //else
                        //{
                        //    responseType = NO_RUNNING_PROGRAM;
                        //}
                    }
                    else if(requestType =="writeToStdin")
                    {
                        //const char* input = json_string_value(json_object_get(root,"input"));
                        //if(NULL == input)
                        //{
                        //	status = 4;//missing parameter
                        //}
                        //else
                        //{
                        //	QMutexLocker lk( &_processMutex );
                        //	if(_process != NULL && (_process->state() == 2))
                        //	{
                        //		_process->write((QString(input)+QString("\n")).toLatin1());
                        //	}
                        //	else
                        //	{
                        //		status = 6;//no running program
                        //	}
                        //}
                    }
                    else if(requestType =="getProcessInfo")
                    {
                        isRoutine = true;
                        if(!_latestProgram.isEmpty() && ScriptRunner::singleton()->hasProcess(_robviewPath))
                        {
                            json_object_set_new(responseJson, "hasProcess",json_integer(1));
                            json_object_set_new(responseJson, "currentProcess",json_string(_latestProgram.toLatin1()));
                            json_object_set_new(responseJson, "processState",json_integer(ScriptRunner::singleton()->getProcessState(_robviewPath)));
                        }
                        else
                        {
                            json_object_set_new(responseJson, "hasProcess",json_integer(-1));
                            responseType = DATA_RESPONSE;
                        }
                        json_object_set_new(responseJson, "output", json_string(ScriptRunner::singleton()->getProcessOutput().toLatin1()));
                    }
                    else if(requestType =="SetEthConf")
                    {
                        rec::robotino::daemons::Log::singleton()->log("start processing: " + str,1);
                        QString DHCP = QString(json_string_value(json_object_get(root,"DHCP")));
                        QString IP = QString(json_string_value(json_object_get(root,"IP")));
                        QString Netmask = QString(json_string_value(json_object_get(root,"Netmask")));
                        QStringList arguments;
                        responseType = DATA_RESPONSE;
                        if(DHCP == NULL || DHCP == "false")
                        {
                            arguments << "static";
                            arguments << IP;
                            arguments << Netmask;
                            json_object_set_new(responseJson, "MSG", json_string("try setting eth0 static"));
                        }
                        else if(DHCP == "true")
                        {
                            arguments << "dhcp";
                            json_object_set_new(responseJson, "MSG", json_string("try setting eth0 dhcp"));

                        }
                        ScriptRunner::singleton()->runScript(
                                    Client::singleton()->getParameter( "~/Scripts/reconfigure_eth0_script", "/usr/local/bin/reconfigure_eth0.sh" ).toString(),arguments);
                    }
                    else if(requestType =="SetWlanConf")
                    {
                        rec::robotino::daemons::Log::singleton()->log("start processing: " + str,1);
                        QString mode = QString(json_string_value(json_object_get(root,"mode")));
                        QString DHCP = QString(json_string_value(json_object_get(root,"DHCP")));
                        QString IP = QString(json_string_value(json_object_get(root,"IP")));
                        QString Netmask = QString(json_string_value(json_object_get(root,"Netmask")));
                        QString ssid = QString(json_string_value(json_object_get(root,"ssid")));
                        QString passphrase = QString(json_string_value(json_object_get(root,"passphrase")));
                        QString wpaMode = QString(json_string_value(json_object_get(root,"wpaMode")));
                        QString cipher = QString(json_string_value(json_object_get(root,"cipher")));
                        if(  mode == NULL || (mode != "master" && mode != "client") )
                        {
                            responseType = MISSING_PARAMETER;
                        }
                        else
                        {
                            QStringList arguments;
                            responseType = DATA_RESPONSE;
                            if(mode == "master")
                            {
                                if(cipher == NULL ||
                                        (cipher != "TKIP" && cipher != "CCMP" && cipher != "TKIP/CCMP" && cipher != "TKIP CCMP"))
                                {
                                    cipher = "TKIP CCMP";
                                }
                                arguments << "master";
                                arguments << ssid;
                                arguments << cipher;
                                arguments << passphrase;
                                json_object_set_new(responseJson, "MSG", json_string("try starting master mode"));
                                ScriptRunner::singleton()->runScript(
                                            Client::singleton()->getParameter( "~/Scripts/reconfigure_wlan0_script", "/usr/local/bin/reconfigure_wlan0.sh" ).toString(),arguments);

                            }
                            else if(mode == "client")
                            {
                                if(DHCP == NULL || DHCP == "false")
                                {
                                    arguments << "client";
                                    arguments << "static";
                                    arguments << IP;
                                    arguments << Netmask;
                                    arguments << ssid;
                                    arguments << wpaMode;
                                    arguments << passphrase;
                                    json_object_set_new(responseJson, "MSG", json_string("try starting client mode (static)"));
                                }
                                else// if(DHCP == "true")
                                {
                                    arguments << "client";
                                    arguments << "dhcp";
                                    arguments << ssid;
                                    arguments << wpaMode;
                                    arguments << passphrase;
                                    json_object_set_new(responseJson, "MSG", json_string("try starting client mode (DHCP)"));

                                }
                                ScriptRunner::singleton()->runScript(
                                            Client::singleton()->getParameter( "~/Scripts/reconfigure_wlan0_script", "/usr/local/bin/reconfigure_wlan0.sh" ).toString(),arguments);
                            }
                            else
                            {
                                json_object_set_new(responseJson, "MSG", json_string((QString("Unknown mode: ") + mode + QString(". Should be either master or client.")).toLatin1()));
                                responseType = DATA_RESPONSE;
                            }

                        }

                    }
                    else if( requestType =="GetIFConf")
                    {
                        QString iface = QString(json_string_value(json_object_get(root,"IF")));

                        QString address = "0.0.0.0";
                        QString netmask = "0.0.0.0";

                        QNetworkInterface n = QNetworkInterface::interfaceFromName( iface );
                        if( n.isValid() )
                        {
                            QList<QNetworkAddressEntry> entries = n.addressEntries();
                            if( false == entries.isEmpty() )
                            {
                                address = entries.first().ip().toString();
                                netmask = entries.first().netmask().toString();
                            }
                        }
                        json_object_set_new(responseJson, "address", json_string(address.toLatin1()));
                        json_object_set_new(responseJson, "netmask", json_string(netmask.toLatin1()));
                        responseType = DATA_RESPONSE;
                        //TODO pass address and netmask to client
                    }
                    else if(requestType =="SetHostapd")
                    {
                        rec::robotino::daemons::Log::singleton()->log("start processing:" +str,1);
                        QString ssid = QString(json_string_value(json_object_get(root,"ssid")));
                        QString rsnPairwise = QString(json_string_value(json_object_get(root,"rsnPairwise")));
                        QString passphrase = QString(json_string_value(json_object_get(root,"passphrase")));
                        if( (ssid==NULL || (ssid.size() == 0)) || (passphrase==NULL || (passphrase.size() == 0) ) )
                        {
                            responseType = MISSING_PARAMETER;
                        }
                        else
                        {
                            json_object_set_new(responseJson, "MSG", json_string(QString("[Server] start changing hostapd conf file").toLatin1()));
                            rec::robotino::daemons::Log::singleton()->log("[Server] start changing hostapd conf file",1);
                            QStringList arguments;
                            arguments << ssid << rsnPairwise << passphrase;
                            ScriptRunner::singleton()->runScript(
                                        Client::singleton()->getParameter( "~/Scripts/mod_hostap_script", "/usr/local/bin/mod_hostap.pl" ).toString()
                                        , arguments );
                        }
                    }
                    else if(requestType =="getWLANConf")
                    {
                        QString master_ssid;
                        QString master_cipher;
                        QString master_passphrase;
                        getHostapdConf( &master_ssid, &master_cipher, &master_passphrase );

                        bool isDHCP;
                        QString address;
                        QString netmask;
                        QString mode;

                        getInterfacesWLAN0Conf( &isDHCP, &address, &netmask, &mode );

                        QString client_ssid;
                        QString client_proto;
                        QString client_psk;

                        getWpaSupplicantConf( &client_ssid, &client_proto, &client_psk );

                        json_object_set_new(responseJson, "mode", json_string(mode.toLatin1()));
                        json_object_set_new(responseJson, "isDHCP", json_string((isDHCP?QString("True"):QString("False")).toLatin1()));
                        json_object_set_new(responseJson, "address", json_string(address.toLatin1()));
                        json_object_set_new(responseJson, "netmask", json_string(netmask.toLatin1()));
                        json_object_set_new(responseJson, "master_ssid", json_string(master_ssid.toLatin1()));
                        json_object_set_new(responseJson, "master_cipher", json_string(master_cipher.toLatin1()));
                        json_object_set_new(responseJson, "master_passphrase", json_string(master_passphrase.toLatin1()));
                        json_object_set_new(responseJson, "client_ssid", json_string(client_ssid.toLatin1()));
                        json_object_set_new(responseJson, "client_proto", json_string(client_proto.toLatin1()));
                        json_object_set_new(responseJson, "client_psk", json_string(client_psk.toLatin1()));
                        responseType = DATA_RESPONSE;
                    }
                    else if(requestType =="getDistance")
                    {
                        isRoutine = true;
                        json_t* distArray = json_array();
                        float t = 0;
                        const QVector<float>& distance = _sensorDataHandler->getDistance();
                        for(int i = 0; i < distance.size(); i++)
                        {
                            t = 40.0*exp(-1.5*(distance[i]-0.3));
                            json_t * d = json_real(t);
                            json_array_append_new(distArray,d);
                        }
                        json_object_set_new(responseJson, "dist",distArray);
                        int bt = _sensorDataHandler->timeSinceLastBump();
                        if(bt >= 100000)
                        {
                            bt = 100000;
                        }
                        json_t * b = json_integer(bt);
                        json_object_set_new(responseJson, "bumpTime",b);

                        responseType = DATA_RESPONSE;
                    }
                    else if(requestType =="getBatteryInfo")
                    {
                        isRoutine = true;
                        float current =  _sensorDataHandler->getSystemCurrent();
                        float voltage = _sensorDataHandler->getBatteryBoltage();
                        QString batteryType = _sensorDataHandler->getBatteryType();
                        bool ext_power = _sensorDataHandler->isExtPower();
                        bool isBatteryLow = _sensorDataHandler->isBatteryLow();
                        int numChargers = _sensorDataHandler->getNumChargers();
                        int shutdownCounter = _sensorDataHandler->getBatteryLowShutdownCounter();

                        unsigned int _charger0_time = _sensorDataHandler->getCharger0Time();
                        float _charger0_batteryVoltage = _sensorDataHandler->getCharger0BatteryVoltage();
                        float _charger0_chargingCurrent = _sensorDataHandler->getCharger0ChargingCurrent();
                        float _charger0_bat1temp = _sensorDataHandler->getCharger0Bat1temp();
                        float _charger0_bat2temp = _sensorDataHandler->getCharger0Bat2temp();
                        int _charger0_state_number = _sensorDataHandler->getCharger0StateNumber();
                        QString _charger0_state = _sensorDataHandler->getCharger0State();

                        unsigned int _charger1_time = _sensorDataHandler->getCharger1Time();
                        float _charger1_batteryVoltage = _sensorDataHandler->getCharger1BatteryVoltage();
                        float _charger1_chargingCurrent = _sensorDataHandler->getCharger1ChargingCurrent();
                        float _charger1_bat1temp = _sensorDataHandler->getCharger1Bat1temp();
                        float _charger1_bat2temp = _sensorDataHandler->getCharger1Bat2temp();
                        int _charger1_state_number = _sensorDataHandler->getCharger1StateNumber();
                        QString _charger1_state =  _sensorDataHandler->getCharger1State();

                        unsigned int _charger2_time = _sensorDataHandler->getCharger2Time();
                        float _charger2_batteryVoltage = _sensorDataHandler->getCharger2BatteryVoltage();
                        float _charger2_chargingCurrent = _sensorDataHandler->getCharger2ChargingCurrent();
                        float _charger2_bat1temp = _sensorDataHandler->getCharger2Bat1temp();
                        float _charger2_bat2temp = _sensorDataHandler->getCharger2Bat2temp();
                        int _charger2_state_number = _sensorDataHandler->getCharger2StateNumber();
                        QString _charger2_state =  _sensorDataHandler->getCharger2State();

                        int _charger0_major = _sensorDataHandler->getCharger0MajorVer();
                        int _charger0_minor = _sensorDataHandler->getCharger0MinorVer();
                        int _charger0_patch = _sensorDataHandler->getCharger0PatchVer();
                        int _charger1_major = _sensorDataHandler->getCharger1MajorVer();
                        int _charger1_minor = _sensorDataHandler->getCharger1MinorVer();
                        int _charger1_patch = _sensorDataHandler->getCharger1PatchVer();
                        int _charger2_major = _sensorDataHandler->getCharger2MajorVer();
                        int _charger2_minor = _sensorDataHandler->getCharger2MinorVer();
                        int _charger2_patch = _sensorDataHandler->getCharger2PatchVer();

                        json_t* c = json_real(current);
                        json_object_set_new(responseJson, "current",c);

                        json_t* v = json_real(voltage);
                        json_object_set_new(responseJson, "voltage",v);

                        json_object_set_new(responseJson, "batteryType", json_string(batteryType.toLatin1()));

                        json_t* ext = json_real(ext_power?1:0);
                        json_object_set_new(responseJson, "ext_power",ext);

                        json_t* low = json_real(isBatteryLow?1:0);
                        json_object_set_new(responseJson, "isBatteryLow",low);

                        json_t* nc = json_integer(numChargers);
                        json_object_set_new(responseJson, "numChargers",nc);

                        json_t* sdc = json_integer(shutdownCounter);
                        json_object_set_new(responseJson, "shutdownCounter",sdc);

                        json_t* c0_time = json_real(_charger0_time);
                        json_object_set_new(responseJson, "c0_time",c0_time);
                        json_t* c0_v = json_real(_charger0_batteryVoltage);
                        json_object_set_new(responseJson, "c0_vol",c0_v);
                        json_t* c0_c = json_real(_charger0_chargingCurrent);
                        json_object_set_new(responseJson, "c0_current",c0_c);
                        json_t* c0_temp1 = json_real(_charger0_bat1temp);
                        json_object_set_new(responseJson, "c0_temp1",c0_temp1);
                        json_t* c0_temp2 = json_real(_charger0_bat2temp);
                        json_object_set_new(responseJson, "c0_temp2",c0_temp2);
                        json_object_set_new(responseJson, "c0_state", json_string(_charger0_state.toLatin1()));

                        json_t* c1_time = json_real(_charger1_time);
                        json_object_set_new(responseJson, "c1_time",c1_time);
                        json_t* c1_v = json_real(_charger1_batteryVoltage);
                        json_object_set_new(responseJson, "c1_vol",c1_v);
                        json_t* c1_c = json_real(_charger1_chargingCurrent);
                        json_object_set_new(responseJson, "c1_current",c1_c);
                        json_t* c1_temp1 = json_real(_charger1_bat1temp);
                        json_object_set_new(responseJson, "c1_temp1",c1_temp1);
                        json_t* c1_temp2 = json_real(_charger1_bat2temp);
                        json_object_set_new(responseJson, "c1_temp2",c1_temp2);
                        json_object_set_new(responseJson, "c1_state", json_string(_charger1_state.toLatin1()));

                        json_t* c2_time = json_real(_charger2_time);
                        json_object_set_new(responseJson, "c2_time",c2_time);
                        json_t* c2_v = json_real(_charger2_batteryVoltage);
                        json_object_set_new(responseJson, "c2_vol",c2_v);
                        json_t* c2_c = json_real(_charger2_chargingCurrent);
                        json_object_set_new(responseJson, "c2_current",c2_c);
                        json_t* c2_temp1 = json_real(_charger2_bat1temp);
                        json_object_set_new(responseJson, "c2_temp1",c2_temp1);
                        json_t* c2_temp2 = json_real(_charger2_bat2temp);
                        json_object_set_new(responseJson, "c2_temp2",c2_temp2);
                        json_object_set_new(responseJson, "c2_state", json_string(_charger2_state.toLatin1()));

                        json_t* c0_version = json_integer(_charger0_major);
                        json_object_set_new(responseJson, "c0_major",c0_version);
                        c0_version = json_integer(_charger0_minor);
                        json_object_set_new(responseJson, "c0_minor",c0_version);
                        c0_version = json_integer(_charger0_patch);
                        json_object_set_new(responseJson, "c0_patch",c0_version);
                        json_t* c1_version = json_integer(_charger1_major);
                        json_object_set_new(responseJson, "c1_major",c1_version);
                        c1_version = json_integer(_charger1_minor);
                        json_object_set_new(responseJson, "c1_minor",c1_version);
                        c1_version = json_integer(_charger1_patch);
                        json_object_set_new(responseJson, "c1_patch",c1_version);
                        json_t* c2_version = json_integer(_charger2_major);
                        json_object_set_new(responseJson, "c2_major",c2_version);
                        c2_version = json_integer(_charger2_minor);
                        json_object_set_new(responseJson, "c2_minor",c2_version);
                        c2_version = json_integer(_charger2_patch);
                        json_object_set_new(responseJson, "c2_patch",c2_version);

                        responseType = DATA_RESPONSE;
                    }//end "getBatteryInfo"
                    else if(requestType == "getIOBoardInfo")
                    {
                        QVector< bool > digout,relout;
                        if(json_is_array(json_object_get(root,"output")))
                        {
                            json_t* array = json_object_get(root,"output");
                            int size = json_array_size(array);
                            for(int i=0;i < size;i++)
                            {
                                json_t* item=json_array_get(array, i);
                                if(json_is_integer(item))
                                {
                                    if(i<8)
                                    {   if(json_integer_value(item)==1)digout.append(true);
                                        else digout.append(false);
                                    }
                                    else
                                    {
                                        if(json_integer_value(item)==1)relout.append(true);
                                        else relout.append(false);
                                    }
                                }
                            }
                            Client::singleton()->set_set_digital_output_array(digout);

                            Client::singleton()->set_set_relay_array(relout);
                        }

                        QVector< bool > digitalin = _sensorDataHandler->getDigitalInput();
                        QVector< float > analog = _sensorDataHandler->getAnalogInput();
                        QVector< bool > digitalout = _sensorDataHandler->getDigitalOutput();
                        QVector< bool > relay = _sensorDataHandler->getRelay();

                        json_t* array = json_array();
                        Q_FOREACH( const bool& port, digitalin )
                        {
                            json_array_append_new(array, json_integer(port));
                        }
                        json_object_set_new(responseJson, "digital", array);

                        array = json_array();
                        Q_FOREACH( const float& port, analog )
                        {
                            json_array_append_new(array, json_real(port));
                        }
                        json_object_set_new(responseJson, "analog", array);

                        array = json_array();
                        Q_FOREACH( const bool& port, digitalout )
                        {
                            json_array_append_new(array, json_integer(port));
                        }
                        json_object_set_new(responseJson, "digitalout", array);

                        array = json_array();
                        Q_FOREACH( const bool& port, relay )
                        {
                            json_array_append_new(array, json_integer(port));
                        }
                        json_object_set_new(responseJson, "relay", array);

                        responseType = DATA_RESPONSE;
                    }
                    else if(requestType == "getSettingsCategoryXML")
                    {
                        configXMLBuilder.rebuild();//parse all ini file
                        printf("Content-type: text/xml\r\n\r\n");
                        printf(configXMLBuilder.getCategoryXML().toUtf8());
                        //dumpFile("./resources/settingCategoryDS.data.xml");
//                        json_decref(root);
//                        json_decref(responseJson);
                        continue;
                    }
                    else if(requestType == "getSettingsItemsXML")
                    {
                        QTime time;
                        time.start();
                        configXMLBuilder.rebuild();//parse all ini file
                        int ms =  time.elapsed();
                        rec::robotino::daemons::Log::singleton()->log(QString("configXMLBuilder.rebuild() took %1 ms").arg(ms),0);

                        printf("Content-type: text/xml\r\n\r\n");
                        printf(configXMLBuilder.getSettingItemXML().toUtf8());
                        //dumpFile("./resources/settingItem.data.xml");
//                        json_decref(root);
//                        json_decref(responseJson);
                        continue;
                    }
                    else if(requestType == "saveSettings")
                    {
                        if(!json_is_array(json_object_get(root,"settings")))
                        {
                            responseType = MISSING_PARAMETER;
                        }
                        else
                        {
                            QMap<QString,ConfigEditor*> editors;

                            json_t* array = json_object_get(root,"settings");
                            int size = json_array_size(array);
                            UdpLogger::Singleton()->broadcastLog( QString("saveSettings array size %1").arg( size ), 1);
                            for(int idx = 0;idx < size; idx++)
                            {
                                json_t* item = json_array_get(array, idx);
                                if(!json_is_object(item) ||
                                        !json_is_string(json_object_get(item,"category")) ||
                                        !json_is_string(json_object_get(item,"itemName")) ||
                                        !json_is_string(json_object_get(item,"value"))
                                        )
                                {
                                    responseType = MISSING_PARAMETER;
                                    break;
                                }
                                QString category = QString(json_string_value(json_object_get(item,"category")));
                                QString itemName = QString(json_string_value(json_object_get(item,"itemName")));
                                QString valueStr = QString(json_string_value(json_object_get(item,"value")));

                                ConfigEditor* editor = NULL;
                                if( editors.contains( category ) )
                                {
                                    editor = editors.value( category );
                                }
                                else
                                {
                                    editor = new ConfigEditor;
                                    editors[ category ] = editor;
                                }


                                editor->setValue(itemName, valueStr);
                            }

                            Q_FOREACH( const QString& category, editors.keys() )
                            {
                                ConfigEditor* editor = editors.value( category );

                                bool ok = editor->write( category );
                                if(!ok)
                                {
                                    UdpLogger::Singleton()->broadcastLog( QString("saveSettings error writing %1").arg( category ), 1);
                                    responseType = OTHER_ERROR;
                                }

                                delete editor;
                            }
                        }

                    }
                    else if(requestType ==  "restartComponent")
                    {
                        if(!json_is_string(json_object_get(root,"filepath")))
                        {
                            responseType = MISSING_PARAMETER;
                        }
                        QString filepath = QString(json_string_value(json_object_get(root,"filepath")));

                        const ConfigRestartItem restartItem = configXMLBuilder.getConfigRestartItem( filepath );

                        if( false == restartItem.isEmpty() )
                        {
                            QString script = restartItem.script;
                            //TODO execute commands
                            QFile tmpScriptFile("/tmp/restart_" + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss_zzz")+".sh");
                            bool ok = tmpScriptFile.open(QFile::WriteOnly);
                            if(!ok)
                            {
                                QString errmsg = QString("Failed to open tmpScriptFile: %1").arg(tmpScriptFile.fileName());
                                rec::robotino::daemons::Log::singleton()->log(errmsg,0);
                                json_object_set_new(responseJson, "MSG", json_string(errmsg.toLatin1()));
                                responseType = OTHER_ERROR;
                            }
                            else
                            {
                                tmpScriptFile.write(script.toUtf8());
                                tmpScriptFile.close();
                                tmpScriptFile.setPermissions(
                                            QFile::ExeOwner | QFile::ReadOwner | QFile::WriteOwner
                                            |QFile::ExeGroup | QFile::ReadGroup | QFile::WriteGroup
                                            |QFile::ExeOther | QFile::ReadOther | QFile::WriteOther);
                                ScriptRunner::singleton()->runScript(tmpScriptFile.fileName());
                            }
                        }
                        else
                        {
                            json_object_set_new(responseJson, "MSG", json_string("NO RULE FOUND TO RESTART THE COMPONENT"));
                            responseType = OTHER_ERROR;
                        }
                    }
                    else if(requestType == "getVersion")
                    {
                        QString filename="/version.txt";
                        QFile file(filename);
                        if(!file.open(QIODevice::ReadOnly))
                        {
                            responseType = OTHER_ERROR;
                        } else {
                            QTextStream in(&file);
                            if(!in.atEnd())
                            {
                                responseType = DATA_RESPONSE;
                                json_object_set_new(responseJson, "DATA", json_string(in.readLine().toLatin1()));
                            }
                            //file.close();
                        }
                    }
                    else if(requestType == "updateCheck")
                    {
                        QString filename="/var/www/updlist.rob";
                        int sts = system("COLUMNS=200 dpkg -l > "+filename.toLatin1());
                        QFile file(filename);
                        if(sts == -1 || !file.open(QIODevice::ReadOnly))
                        {
                            responseType = OTHER_ERROR;
                        } else {
                            QTextStream in(&file);
                            QString line;
                            QStringList fields;
                            while(!in.atEnd())
                            {
                                line = in.readLine();
                                fields = line.split("-");
                                if(fields[0] == "+++") break;
                            }
                            json_t* jpkg = json_array();
                            while(!in.atEnd())
                            {
                                line = in.readLine();
                                fields = line.split(' ', QString::SkipEmptyParts);
                                json_t * jobj = json_object();
                                json_object_set_new(jobj, "sts", json_string(fields[0].toLatin1()));
                                json_object_set_new(jobj, "nme", json_string(fields[1].toLatin1()));
                                json_object_set_new(jobj, "ver", json_string(fields[2].toLatin1()));
                                json_array_append_new(jpkg, jobj);
                            }
                            //file.close();

                            json_dump_file(jpkg,filename.toLatin1(), 0);
                            json_object_set_new(responseJson,"DATA",jpkg);
                        }
                    }
                    else if(requestType == "sendDebugMessage")
                    {
                        json_t* message = json_object_get(root, "message");
                        if(!message)
                        {
                            responseType =  MISSING_PARAMETER;
                        }
                        else if(!json_is_string(message))
                        {
                            responseType = INVALID_PARAMETER;
                        }
                        else{
                            QString command = "echo -n \""+QString(json_string_value(message))+
                                    "\" | nc localhost 2000";
                            int s=system(command.toLatin1());
                        }
                    }
                    else
                    {
                        responseType = UNKNOWN_COMMAND;
                    }
                }
            }
            switch(responseType)
            {
            case SUCCESS:
                json_object_set_new(responseJson, "TYPE", json_string("ACK"));
                break;
            case DATA_RESPONSE:
                json_object_set_new(responseJson, "TYPE", json_string("DATA"));
                break;
            case NO_JSON_OBJECT_RECIEVED:
                json_object_set_new(responseJson, "TYPE", json_string("ERR"));
                json_object_set_new(responseJson, "MSG", json_string("NO JSON OBJ RECIEVED"));
                rec::robotino::daemons::Log::singleton()->log("NO JSON OBJ RECIEVED: " +str,2);
                break;
            case TYPE_FIELD_NOT_FOUND:
                json_object_set_new(responseJson, "TYPE", json_string("ERR"));
                json_object_set_new(responseJson, "MSG", json_string("NO TYPE FIELD FOUND"));
                rec::robotino::daemons::Log::singleton()->log("NO TYPE FIELD FOUND: " +str,2);
                break;
            case UNKNOWN_COMMAND:
                json_object_set_new(responseJson, "TYPE", json_string("ERR"));
                json_object_set_new(responseJson, "MSG", json_string("UNKNOWN COMMAND"));
                rec::robotino::daemons::Log::singleton()->log("UNKNOWN COMMAND: " +str,2);
                break;
            case MISSING_PARAMETER:
                json_object_set_new(responseJson, "TYPE", json_string("ERR"));
                json_object_set_new(responseJson, "MSG", json_string("MISSING PARAMETER(S)"));
                rec::robotino::daemons::Log::singleton()->log("MISSING PARAMETER(S): " +str,2);
                break;
            case INVALID_PARAMETER:
                json_object_set_new(responseJson, "TYPE", json_string("ERR"));
                json_object_set_new(responseJson, "MSG", json_string("INVALID PARAMETER(S)"));
                rec::robotino::daemons::Log::singleton()->log("INVALID PARAMETER(S): " +str,2);
                break;
            case NO_RUNNING_PROGRAM:
                json_object_set_new(responseJson, "TYPE", json_string("ERR"));
                json_object_set_new(responseJson, "MSG", json_string("NO RUNNING PROGRAM"));
                rec::robotino::daemons::Log::singleton()->log("NO RUNNING PROGRAM: " +str,2);
                break;
            case OTHER_ERROR:
                json_object_set_new(responseJson, "TYPE", json_string("ERR") );
                rec::robotino::daemons::Log::singleton()->log("ERROR: " +str,2);
                break;
            case INVALID_FILE_NAME:
                json_object_set_new(responseJson, "TYPE", json_string("ERR"));
                json_object_set_new(responseJson, "MSG", json_string("File name is invalid."));
                break;
            default:
                json_object_set_new(responseJson, "TYPE", json_string("ERR"));
                json_object_set_new(responseJson, "MSG", json_string("UNKNOWN ERROR"));
                rec::robotino::daemons::Log::singleton()->log("UNKNOWN ERROR: " +str,2);
                break;
            }
            //json_object_set_new(responseJson, "JSON STRING", json_string(str.c_str()));+
            if(isRoutine)
            {
                rec::robotino::daemons::Log::singleton()->log((QString("Done:")+str),4);
            }
            else
            {
                rec::robotino::daemons::Log::singleton()->log((QString("Done:")+str),3);
            }
            printf("Content-type: text/html\r\n\r\n");
            char *buf = json_dumps(responseJson, JSON_ESCAPE_SLASH|JSON_COMPACT);
            printf(buf);
            free(buf);
            json_decref(root);
            json_decref(responseJson);
        }
        catch(...)
        {
            rec::robotino::daemons::Log::singleton()->log("EXCEPTION WHILE PROCESSING REQUEST !!!, request string: " + str, 0 );
        }
    }//end while(FCGI_Accept() >= 0)

}//end Fcgi::run()
