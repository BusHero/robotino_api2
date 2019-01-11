#include "McHandler.h"

#include "Client.h"
#include <stdlib.h>
#include "rec/robotino/daemons/Log.h"
#include "jansson.h"

McHandler::McHandler(QObject* parent)
    : _layouts()
    , _status()
{

    bool connected = (bool)connect( Client::singleton()
           , SIGNAL( mcstatus_changed( const QByteArray& ) )
           , SLOT (on_mcstatus_changed( const QByteArray& ) ) );
    Q_ASSERT( connected );
     connected = (bool)connect( Client::singleton()
           , SIGNAL( mclayout_changed( const QByteArray& ) )
           , SLOT (on_mclayout_changed( const QByteArray& ) ) );
    Q_ASSERT( connected );
}

std::vector<McHandler::layoutData> McHandler::getLayout()
{
    QMutexLocker lk( &_mutex );
    return _layouts;
}

void McHandler::getStatus(std::vector<statusData>& sts, std::map< int, std::vector<int> >& path)
{
    QMutexLocker lk( &_mutex );
    sts=_status;
    path=_paths;
}

void McHandler::on_mclayout_changed( const QByteArray& data )
{
    QString layout = QString(data);
    QString errprefix = "McLayout Error: ";
    json_error_t error;
    json_t *root = json_loads(layout.toLatin1(),0,&error);
    if(!root)
    {
        rec::robotino::daemons::Log::singleton()->log(errprefix+layout, 1);
    }
    else
    {
        _layouts.clear();
        json_t* array = json_object_get(root,"DATA");
        int size = json_array_size(array);
        rec::robotino::daemons::Log::singleton()->log("MCLAYOUT IN: "+layout, 1);
        for(int i=0;i < size;++i)
        {
            json_t* item=json_array_get(array, i);
            if(!json_is_object(item))
            {
                break;
            }
            layoutData ld;
            json_t *temp;
            temp = json_object_get(item,"id");
            ld.id   = json_integer_value(temp);
//            json_decref(temp);
            temp = json_object_get(item,"x");
            ld.x    = json_real_value(temp);
//            json_decref(temp);
            temp = json_object_get(item,"y");
            ld.y    = json_real_value(temp);
//            json_decref(temp);
            temp = json_object_get(item,"path");
            ld.path = json_integer_value(temp);
//            json_decref(temp);
            temp = json_object_get(item,"pose");
            ld.pose = json_integer_value(temp);
//            json_decref(temp);
//            json_decref(item);
            _layouts.push_back(ld);
        }
//        json_decref(array);
    }
    json_decref(root);
}

void McHandler::on_mcstatus_changed( const QByteArray& data )
{
    QString status = QString(data);
    QString errprefix = "McStatus Error: ";
    json_error_t error;
    json_t *root = json_loads(status.toLatin1(),0,&error);
    if(!root)
    {
        rec::robotino::daemons::Log::singleton()->log(errprefix+status, 1);
    }
    else
    {
        json_t* array = json_object_get(root,"STATUS");
        if(array)
        {
            int size = json_array_size(array);
            if(_status.size() != size) _status.resize(size);
            for(int i=0;i < size;++i)
            {
                json_t* item=json_array_get(array, i);
                if(!json_is_object(item))
                {
                    break;
                }
                statusData sd;
                json_t *temp;
                temp = json_object_get(item,"id");
                sd.id     = json_integer_value(temp);
                json_decref(temp);
                temp = json_object_get(item,"sts");
                sd.status = json_integer_value(temp);
                json_decref(temp);
                temp = json_object_get(item,"rId");
                sd.rId    = atoi(json_string_value(temp));
                json_decref(temp);
                _status[i]=sd;
                json_decref(item);
            }
            json_decref(array);
        }
        else
        {
            json_t* path_json = json_object_get(root,"PATHS");
            if(path_json)
            {
                json_t* rId = json_object_get(root,"rid");
                const char* rid = json_string_value(rId);
                json_decref(rId);
                int rID = rid?atoi(rid):-1;
                if(rID == -1)
                {
                    QString errmsg = errprefix +"Robotino ID from Smartsoft is not String";
                    rec::robotino::daemons::Log::singleton()->log(errmsg, 1);
                    return;
                }
                int size = json_array_size(path_json);
                std::vector<int> paths;
                for(int i=0;i < size;++i)
                {
                    json_t* item = json_array_get(path_json, i);
                    json_t* id = json_object_get(item,"id");
                    paths.push_back(json_integer_value(id));
                    json_decref(id);
                    json_decref(item);
                }
                _paths[rID]=paths;
            }
            json_decref(path_json);
        }
    }
    json_decref(root);
}
