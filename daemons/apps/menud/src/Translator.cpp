#include "Translator.h"
#include "rec/robotino/daemons/Log.h"
#include "Client.h"

extern rec::robotino::daemons::Log* logHandler;
extern Client* client;

Translator* Translator::_impl = NULL;
QString Translator::_currentLanguage = QString( "en" );

Translator::Translator( QDomElement& docElem )
{
	QDomNode n = docElem.firstChild();
	while( !n.isNull() )
	{
		QDomElement e = n.toElement(); // try to convert the node to an element.
		if( !e.isNull() && "entry" == e.tagName() )
		{
			loadTranslation( e );
		}
		n = n.nextSibling();
	}
}

void Translator::loadTranslation( QDomElement& e )
{
	if( false == e.hasAttribute( "de" ) )
	{
		logHandler->log( "entry does not contain \"de\" attribute", 0 );
		return;
	}

	QString key = e.attribute( "de" );

	QString logStr;

	QDomNamedNodeMap m = e.attributes();
	for( int i=0; i<m.size(); ++i )
	{
		QDomAttr attr = m.item( i ).toAttr();
		if( false == attr.isNull() )
		{
			QMap< QString, QString >& m = _transMap[ attr.name() ];
			m[ key ] = attr.value();

			logStr += attr.name() + "=\"" + attr.value() + "\" ";
		}
	}

	logHandler->log( logStr, 2 );
}

void Translator::init()
{
	Q_ASSERT( NULL == _impl );

	QDomDocument doc("trans");

	QString fileName = client->getParameter( "~/trans", "/etc/robotino/menud/trans.xml" ).toString();

	QFile transFile( fileName );
	if( transFile.open( QIODevice::ReadOnly ) )
	{
		if( false == doc.setContent( &transFile ) )
		{
			logHandler->log( QString( "Not a valid XML file: %1" ).arg( fileName ), 0 );
			return;
		}
	}
	else
	{
		logHandler->log( QString( "Error loading %1" ).arg( fileName ), 0 );
		return;
	}
	transFile.close();

	QDomElement docElem = doc.documentElement();

	_impl = new Translator( docElem );

	_currentLanguage = client->getParameter( "~/language", "en" ).toString();
}

void Translator::done()
{
	delete _impl;
	_impl = NULL;
}

QString Translator::translate( const QString& str )
{
	Q_ASSERT( _impl );

	if( false == _impl->_transMap.contains( _currentLanguage ) )
	{
		logHandler->log( QString( "Unknown current language \"%1\"" ).arg( _currentLanguage ), 0 );
		return str;
	}

	const QMap< QString, QString >& m = _impl->_transMap[ _currentLanguage ];

	if( false == m.contains( str ) )
	{
		logHandler->log( QString( "\"%1\" not translated to \"%2\" yet" ).arg( str ).arg( _currentLanguage ), 1 );
		return str;
	}

	return m.value( str );
}

void Translator::setLanguage( const QString& key )
{
	if( false == _impl->_transMap.contains( key ) )
	{
		logHandler->log( QString( "Unknown language key \"%1\"" ).arg( key ), 0 );
		return;
	}

	_currentLanguage = key;
	client->setParameter( "~/language", key );
}
