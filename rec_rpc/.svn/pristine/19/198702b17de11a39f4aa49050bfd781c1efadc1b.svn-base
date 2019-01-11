/*
Copyright (c) 2011, REC Robotics Equipment Corporation GmbH, Planegg, Germany
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
- Neither the name of the REC Robotics Equipment Corporation GmbH nor the names of
  its contributors may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Server.h"

#include "rec/rpc/Exception.h"

#include <QtXml>

Server::Server()
{
	// Register the RPC functions to allow clients to invoke them.
	REGISTER_FUNCTION( add );
	REGISTER_FUNCTION( sub );
	REGISTER_FUNCTION( mult );
	REGISTER_FUNCTION( div );

	// Add the topic "image" with initially 4kB shared memory.
	ADD_TOPIC( image, 0x100000 );

	ADD_ENQUEUEDTOPIC( text );
	ADD_TOPIC( text2, 0 );

	setMultiThreadedSerializationEnabled( true );
	setGreeting( "REC RPC test server" );
}

void Server::publishImage( const QImage& img )
{	// Publish the given QImage in the topic "image".
	PUBLISH_TOPIC_SIMPLE( image, img );
}

void Server::publishText( const QString& text )
{
	PUBLISH_TOPIC_SIMPLE( text, text );
}

void Server::setTopicsEnabled( bool enabled )
{	// Register or unregister topic listener and topic info listener for topic "image".
	if ( enabled )
	{
		REGISTER_TOPICINFOCHANGED( image );
		REGISTER_TOPICLISTENER( image );
		REGISTER_TOPICLISTENER( text );
		REGISTER_TOPICLISTENER( text2 );
	}
	else
	{
		UNREGISTER_TOPICINFOCHANGED( image );
		UNREGISTER_TOPICLISTENER( image );
		UNREGISTER_TOPICLISTENER( text );
		UNREGISTER_TOPICLISTENER( text2 );
	}
}

void Server::setHandleHttpGet( bool enabled )
{
	if ( enabled )
	{
		REGISTER_HTTP_GET_HANDLER( handleHttpGet );
	}
	else
	{
		REMOVE_HTTP_GET_HANDLER;
	}
}

void Server::setHandleCustomRequests( bool enabled )
{
	if ( enabled )
	{
		REGISTER_CUSTOM_REQUEST_HANDLER( handleCustomRequest );
	}
	else
	{
		REMOVE_CUSTOM_REQUEST_HANDLER;
	}
}

// Definition of the rpc functions. Parameters are accessible via "param", return values via "result".
// Serialization data types are defined in Serialization.h and Serialization.cpp.
BEGIN_FUNCTION_DEFINITION( Server, add )
	result = param.a() + param.b();
	Q_EMIT log( QString( "Client %1:%2: %3 + %4 = %5" ).arg( client.address.toString() ).arg( client.port ).arg( param.a() ).arg( param.b() ).arg( result ) );
END_FUNCTION_DEFINITION

BEGIN_FUNCTION_DEFINITION( Server, sub )
	result = param.a() - param.b();
	Q_EMIT log( QString( "Client %1:%2: %3 - %4 = %5" ).arg( client.address.toString() ).arg( client.port ).arg( param.a() ).arg( param.b() ).arg( result ) );
END_FUNCTION_DEFINITION

BEGIN_FUNCTION_DEFINITION( Server, mult )
	result = param.a() * param.b();
	Q_EMIT log( QString( "Client %1:%2: %3 * %4 = %5" ).arg( client.address.toString() ).arg( client.port ).arg( param.a() ).arg( param.b() ).arg( result ) );
END_FUNCTION_DEFINITION

BEGIN_FUNCTION_DEFINITION( Server, div )
	if ( param.b() == 0 )
	{
		Q_EMIT log( QString( "Client %1:%2: %3 / %4 = DIVISION BY ZERO" ).arg( client.address.toString() ).arg( client.port ).arg( param.a() ).arg( param.b() ) );
		// In case of an error, a rec::rpc::Exceptin can be thrown. The client will receive the error code.
		// If the function call is blocking, an exception will be thrown.
		throw rec::rpc::Exception( static_cast< rec::rpc::ErrorCode >( DivisionByZero ) );
	}
	result = param.a() / param.b();
	Q_EMIT log( QString( "Client %1:%2: %3 / %4 = %5" ).arg( client.address.toString() ).arg( client.port ).arg( param.a() ).arg( param.b() ).arg( result ) );
END_FUNCTION_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, image )
	// Topic listener definition for "image". Just notify the GUI that there is a new image.
	Q_EMIT log( QString( "Image received from %1:%2" ).arg( client.address.toString() ).arg( client.port ) );
	Q_EMIT imageReceived( data );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICINFOCHANGED_DEFINITION( Server, image )
	// Topic listener definition for "image". Just notify the GUI that the list of registered clients has changed.
	Q_EMIT imageInfoChanged( info );
END_TOPICINFOCHANGED_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, text )
	// Topic listener definition for "text".
	Q_EMIT log( QString( "Text received from %1:%2 - %3" ).arg( client.address.toString() ).arg( client.port ).arg( data.ref() ) );
END_TOPICLISTENER_DEFINITION

BEGIN_TOPICLISTENER_DEFINITION( Server, text2 )
	// Topic listener definition for "text2".
	Q_EMIT log( QString( "Text received from %1:%2 - %3" ).arg( client.address.toString() ).arg( client.port ).arg( data.ref() ) );
END_TOPICLISTENER_DEFINITION

BEGIN_HTTP_GET_HANDLER_DEFINITION( Server, handleHttpGet )
	QString path = url.path();
	while( path.endsWith( QChar( 13 ) ) || path.endsWith( QChar( 10 ) ) )
	{
		path.chop( 1 );
	}

	if ( path.startsWith( "/doc" ) )
	{
		QString realPath = path;
		realPath = realPath.mid( 4 );
		realPath.prepend( DOC_DIR );
		QDir dir( realPath );
		if ( dir.exists() )
		{
			if ( !path.endsWith( '/' ) )
				path.append( '/' );

			QString newLocation;
			if ( QFile::exists( path + "index.html" ) )
				newLocation = path + "index.html";
			else if ( QFile::exists( path + "index.htm" ) )
				newLocation = path + "index.htm";
			else
				newLocation = path + "index.html";

			throw rec::rpc::Exception( rec::rpc::HTTPMovedPermanently, newLocation );
		}
		if ( QFile::exists( realPath ) )
		{
			QFile f( realPath );
			if ( f.open( QFile::ReadOnly ) )
			{
				resultPage = f.readAll();
				f.close();

				if ( path.endsWith( "html" ) || path.endsWith( "htm" ) )
					contentType = "application/xhtml+xml; charset=UTF-8";
				else if ( path.endsWith( "png" ) )
					contentType = "image/png";
				else if ( path.endsWith( "css" ) )
					contentType = "text/css";
				else if ( path.endsWith( "js" ) )
					contentType = "application/x-javascript";
				return;
			}
		}
		throw rec::rpc::Exception( rec::rpc::HTTPNotFount );
	}
	else
	{
		contentType = "application/xhtml+xml; charset=UTF-8";

		QDomDocument doc( "html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\"" );
		doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"utf-8\"" ) );
		QDomElement html = doc.createElement( "html" );
		html.setAttribute( "xmlns", "http://www.w3.org/1999/xhtml" );
		doc.appendChild( html );

		QDomElement head = doc.createElement( "head" );
		html.appendChild( head );
		QDomElement title = doc.createElement( "title" );
		head.appendChild( title );
		title.appendChild( doc.createTextNode( "RPC server HTTP demo" ) );
		QDomElement meta = doc.createElement( "meta" );
		meta.setAttribute( "http-equiv", "Content-Type" );
		meta.setAttribute( "content", contentType );
		head.appendChild( meta );

		QDomElement body = doc.createElement( "body" );
		html.appendChild( body );
		QDomElement h1 = doc.createElement( "h1" );
		body.appendChild( h1 );
		h1.appendChild( doc.createTextNode( QString( "REC RPC Server at %1 says Hello!" ).arg( host ) ) );

		QDomElement p = doc.createElement( "p" );
		body.appendChild( p );
		p.appendChild( doc.createTextNode( QString( "URL: " ) + path ) );

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
		QUrlQuery q(url);
		QList< QPair< QString, QString > > queries = q.queryItems();
#else
		QList< QPair< QString, QString > > queries = url.queryItems();
#endif
		if ( !queries.isEmpty() )
		{
			p = doc.createElement( "p" );
			body.appendChild( p );
			p.appendChild( doc.createTextNode( "GET parameters:" ) );

			QDomElement table = doc.createElement( "table" );
			table.setAttribute( "border", 1 );
			table.setAttribute( "cellpadding", 2 );
			table.setAttribute( "cellspacing", 0 );
			body.appendChild( table );

			QDomElement thead = doc.createElement( "thead" );
			table.appendChild( thead );

			QDomElement tr = doc.createElement( "tr" );
			thead.appendChild( tr );

			QDomElement th = doc.createElement( "th" );
			tr.appendChild( th );
			th.appendChild( doc.createTextNode( "Name" ) );

			th = doc.createElement( "th" );
			tr.appendChild( th );
			th.appendChild( doc.createTextNode( "Value" ) );

			QDomElement tbody = doc.createElement( "tbody" );
			table.appendChild( tbody );

			for( QList< QPair< QString, QString > >::const_iterator iter = queries.begin(); iter != queries.end(); ++iter )
			{
				tr = doc.createElement( "tr" );
				tbody.appendChild( tr );

				QDomElement td = doc.createElement( "td" );
				tr.appendChild( td );
				td.appendChild( doc.createTextNode( iter->first ) );

				td = doc.createElement( "td" );
				tr.appendChild( td );
				td.appendChild( doc.createTextNode( iter->second ) );
			}
		}
		resultPage = doc.toByteArray( 2 );
	}
END_HTTP_GET_HANDLER_DEFINITION

BEGIN_CUSTOM_REQUEST_HANDLER_DEFINITION( Server, handleCustomRequest )
	if ( request.startsWith( "Hello" ) )
	{
		response = "Bonjour!\r\n";
	}
END_CUSTOM_REQUEST_HANDLER_DEFINITION
