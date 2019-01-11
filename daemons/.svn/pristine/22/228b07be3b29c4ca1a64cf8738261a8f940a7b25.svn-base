#ifndef HANDLER_H
#define HANDLER_H

#include "common.h"
#include <iostream>

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

class DataHandler;

class handler
{
    public:
        handler();
        handler(http::uri url);
        virtual ~handler();

        pplx::task<void>open(){return m_listener.open();}
        pplx::task<void>close(){return m_listener.close();}

    protected:

    private:
        void handle_get(http_request message);
        void handle_put(http_request message);
        void handle_post(http_request message);
        void handle_delete(http_request message);
        void handle_error(pplx::task<void>& t);
        http_listener m_listener;
		
		DataHandler* _dataHandler;
};

#endif // HANDLER_H