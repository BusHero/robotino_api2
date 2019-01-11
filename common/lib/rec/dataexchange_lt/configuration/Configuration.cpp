#include "rec/dataexchange_lt/configuration/Configuration.h"

using namespace rec::dataexchange_lt::configuration;

Configuration::Configuration( QObject* parent )
: QObject( parent )
, _mutex( QMutex::Recursive )
{
}

Configuration::Configuration( const Configuration& other )
{
	QMutexLocker lk( &other._mutex );
	_itemFromName = other._itemFromName;
}

Configuration& Configuration::operator=( const Configuration& other )
{
	{
		QMutexLocker lk( &_mutex );
		QMutexLocker lk2( &other._mutex );
		_itemFromName = other._itemFromName;
	}

	Q_EMIT changed();
	return *this;
}

rec::dataexchange_lt::messages::Data Configuration::data( const QString& name ) const
{
	QMutexLocker lk( &_mutex );

	if( false == _itemFromName.contains( name ) )
	{
		return rec::dataexchange_lt::messages::Data();
	}

	return _itemFromName[ name ];
}

bool Configuration::setData( const rec::dataexchange_lt::messages::Data& data )
{
	QMutexLocker lk( &_mutex );

	if( false == _itemFromName.contains( data.name ) )
	{
		return false;
	}

	_itemFromName[ data.name ] = data;

	return true;
}

bool Configuration::addItem( const QString& name, rec::dataexchange_lt::DataType type )
{
	bool ret;

	{
		QMutexLocker lk( &_mutex );
		ret = addItem_i( name, type );
	}

	if( ret )
	{
		Q_EMIT changed();
		return true;
	}

	return false;
}

bool Configuration::addItem( const rec::dataexchange_lt::messages::Data& data )
{
	return addItem( data.name, data.type );
}


bool Configuration::addItem_i( const QString& name, rec::dataexchange_lt::DataType type )
{
	if( name.isEmpty() )
	{
		return false;
	}

	if( rec::dataexchange_lt::NOTYPE == type )
	{
		return false;
	}

	if( _itemFromName.contains( name ) )
	{
		return false;
	}

	_itemFromName[ name ] = rec::dataexchange_lt::messages::Data( name, type );

	return true;
}

bool Configuration::removeItem( const QString& name )
{
	{
		QMutexLocker lk( &_mutex );

		if( false == _itemFromName.contains( name ) )
		{
			return false;
		}

		_itemFromName.take( name );
	}

	Q_EMIT changed();

	return true;
}

bool Configuration::renameItem( const QString& oldName, const QString& newName )
{
	{
		QMutexLocker lk( &_mutex );

		if( oldName == newName )
		{
			return true;
		}

		if( false == _itemFromName.contains( oldName ) )
		{
			return false;
		}

		if( _itemFromName.contains( newName ) )
		{
			return false;
		}

		rec::dataexchange_lt::messages::Data item = _itemFromName.take( oldName );
		item.name = newName;
		_itemFromName[ newName ] = item;
	}

	Q_EMIT changed();

	return true;
}

bool Configuration::changeType( const QString& name, rec::dataexchange_lt::DataType newType )
{
	Q_ASSERT( rec::dataexchange_lt::NOTYPE != newType );

	{
		QMutexLocker lk( &_mutex );

		if( false == _itemFromName.contains( name ) )
		{
			return false;
		}

		_itemFromName[ name ].type = newType;
	}

	Q_EMIT changed();

	return true;
}

rec::dataexchange_lt::DataType Configuration::type( const QString& name ) const
{
	QMutexLocker lk( &_mutex );

	if( false == _itemFromName.contains( name ) )
	{
		return rec::dataexchange_lt::NOTYPE;
	}

	return _itemFromName[name].type;
}

QByteArray Configuration::save() const
{
	QMutexLocker lk( &_mutex );

	QDomDocument doc;
	QDomElement root = doc.createElement( "configuration" );
	doc.appendChild( root );

	QMap< QString, rec::dataexchange_lt::messages::Data >::const_iterator iter = _itemFromName.constBegin();

	while( _itemFromName.constEnd() != iter )
	{
		const rec::dataexchange_lt::messages::Data& item = iter.value();

		QDomElement e = doc.createElement( "item" );
		e.setAttribute( "name", item.name );
		e.setAttribute( "type", dataTypeName( item.type ) );

		root.appendChild( e );

		++iter;
	}

	return doc.toByteArray();
}

bool Configuration::load( const QByteArray& data )
{
	{
		QMutexLocker lk( &_mutex );

		_itemFromName.clear();

		QDomDocument doc;

		if( false == doc.setContent( data ) )
		{
			return false;
		}

		QDomElement configuration = doc.documentElement();
		if( "configuration" != configuration.tagName() )
		{
			return false;
		}

		for (int i=0; i<configuration.childNodes().size(); i++)
		{
			QDomElement e = configuration.childNodes().at(i).toElement();

			if( e.isNull() )
			{
				continue;
			}

			QString name;
			if( e.hasAttribute( "name" ) )
			{
				name = e.attribute( "name" );
			}
			else
			{
				name = e.tagName();
			}

			if( false == addItem_i( name, dataTypeFromName( e.attribute( "type" ).toLatin1().constData() ) ) )
			{
				return false;
			}
		}
	}

	Q_EMIT changed();

	return true;
}

