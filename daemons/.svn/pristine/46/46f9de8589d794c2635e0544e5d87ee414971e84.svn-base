#include "handler.h"
#include "DataHandler.h"
#include "configuration.h"
#include "conv.h"

#include "MyCamera.h"

#include <QtCore>

handler::handler()
{
    //ctor
}

handler::handler(http::uri url)
	: m_listener(url)
{
    m_listener.support(methods::GET, std::bind(&handler::handle_get, this, std::placeholders::_1));
    m_listener.support(methods::PUT, std::bind(&handler::handle_put, this, std::placeholders::_1));
    m_listener.support(methods::POST, std::bind(&handler::handle_post, this, std::placeholders::_1));
    m_listener.support(methods::DEL, std::bind(&handler::handle_delete, this, std::placeholders::_1));
	
	_dataHandler = new DataHandler;

}
handler::~handler()
{
    delete _dataHandler;
}

void handler::handle_error(pplx::task<void>& t)
{
    try
    {
        t.get();
    }
    catch(...)
    {
        // Ignore the error, Log it if a logger is available
    }
}


//
// Get Request 
//
void handler::handle_get(http_request message)
{
    //ucout <<  message.to_string() << endl;

    auto paths = http::uri::split_path(http::uri::decode(message.relative_uri().path()));

    QString relativeUriPath = utilityToQString(message.relative_uri().path());
	QString pathExtension;

	int extensionIndex = relativeUriPath.lastIndexOf(".");
	if (extensionIndex > -1)
	{
		pathExtension = relativeUriPath.mid(extensionIndex + 1);
	}
	
	//ucout << U("Relative path: ") << message.relative_uri().path() << endl;
	//Dbms* d  = new Dbms();
    //d->connect();
	
	QString resourcePath = configValue("General/resourcepath");
	QString resourceType = "text/html";
	
	if( relativeUriPath.startsWith("/data") )
	{
		QByteArray jsonData = _dataHandler->data( relativeUriPath );
		
		message.reply(status_codes::OK, qByteArrayToUtilityString( jsonData ), toUtilityString("application/json")).then([](pplx::task<void> t)
		{
			try
			{
				t.get();
			}
			catch(...)
			{
			}
		});
	}
	else if( relativeUriPath.startsWith("/download/program/"))
	{
		QString filePath = relativeUriPath.remove("/download/program/");
		filePath = utilityToQString( web::uri::decode(qStringToUtilityString( filePath ) ) );
		
		QString programPath = configValue("Robview/programpath");
		
		QString downloadPath = programPath;
		downloadPath += "/";
		downloadPath +=  filePath;
		
		concurrency::streams::fstream::open_istream(qStringToUtilityString(downloadPath), std::ios::in).then([=](concurrency::streams::istream is)
		{	
			message.reply(status_codes::OK, is, toUtilityString("application/octet-stream")).then([](pplx::task<void> t)
			{
				try
				{
					t.get();
				}
				catch(...)
				{
				//
				}
			});
		}).then([=](pplx::task<void>t)
		{
			try
			{
				t.get();
			}
			catch(...)
			{
				message.reply(status_codes::InternalError, toUtilityString("INTERNAL ERROR "));
			}
		});
	}
	else if(relativeUriPath.startsWith("/cam0") )
	{
		unsigned int dataSize = 0;
		
		if( MyCamera::singleton()->isNewImageAvailable(&dataSize) )
		{
			std::vector<char> camData(0xFFFFF);

			unsigned int width = 0;
			unsigned int height = 0;
			unsigned int step = 0;
			
			unsigned char* p = (unsigned char*)camData.data();
			
			MyCamera::singleton()->getImage( &p,
					camData.size(),
					&width,
					&height,
					&step );
					
			if(0==width && step>0) //jpeg image
			{
				//boost::iostreams::array_source source{cam0Data};
				//boost::iostreams::stream<boost::iostreams::array_source> is{source};
				
				concurrency::streams::container_buffer< std::vector<char> > imageBuffer(camData);
				concurrency::streams::istream is(imageBuffer);
				
				message.reply(status_codes::OK, is, step, toUtilityString("image/jpeg")).then([](pplx::task<void> t)
				{
					try
					{
						t.get();
					}
					catch(...)
					{
					//
					}
				});
			}
			else
			{
				message.reply(status_codes::InternalError, toUtilityString("No jpeg image available"));
			}
					
			message.reply(status_codes::OK, toUtilityString("No image available"));
		}
		else
		{
			message.reply(status_codes::InternalError, toUtilityString("No camera connected"));
		}
	}
	else
	{
		if( "/" == relativeUriPath)
		{
			resourcePath.append("/index.html");
		}
		else
		{
			resourcePath.append(relativeUriPath);
			
			if( "ico" == pathExtension)
			{
				resourceType = "image/x-icon";
			}
			else if( "png" == pathExtension)
			{
				resourceType = "image/png";
			}
			else if( "jpeg" == pathExtension || "jpg" == pathExtension)
			{
				resourceType = "image/jpeg";
			}
			else if( "js" == pathExtension)
			{
				resourceType = "text/javascript";
			}
			else if( "css" == pathExtension)
			{
				resourceType = "text/css";
			}
		}
		
		concurrency::streams::fstream::open_istream(qStringToUtilityString(resourcePath), std::ios::in).then([=](concurrency::streams::istream is)
		{	
			message.reply(status_codes::OK, is, qStringToUtilityString(resourceType)).then([](pplx::task<void> t)
			{
				try
				{
					t.get();
				}
				catch(...)
				{
				//
				}
			});
		}).then([=](pplx::task<void>t)
		{
			try
			{
				t.get();
			}
			catch(...)
			{
				message.reply(status_codes::InternalError, toUtilityString("INTERNAL ERROR "));
			}
		});
	}

    return;

};

//
// A POST request
//
void handler::handle_post(http_request message)
{
    //ucout <<  message.to_string() << endl;
    message.reply(status_codes::OK,message.to_string());
    return ;
};

//
// A DELETE request
//
void handler::handle_delete(http_request message)
{
    ucout <<  message.to_string() << endl;
    message.reply(status_codes::OK,message.to_string());
	
    return;
};


//
// A PUT request 
//
void handler::handle_put(http_request message)
{
    //ucout <<  message.to_string() << endl << flush;
	
	QString relativeUriPath = utilityToQString(message.relative_uri().path());

    message.extract_string(true).then([=](utility::string_t jsonStr)
	{
		try
		{
			http::status_code status;
			QByteArray response;
			
			if( _dataHandler->setData( relativeUriPath, utilityToQByteArray(jsonStr), response) )
			{
				status = status_codes::OK;
			}
			else
			{
				status = status_codes::BadRequest;
			}			
			
			message.reply(status, qByteArrayToUtilityString(response), toUtilityString("application/json"));
		}
		catch(...)
		{
			message.reply(status_codes::BadRequest);
		}
	});
};