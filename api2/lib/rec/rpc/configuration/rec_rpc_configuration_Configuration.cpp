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

#include "rec/rpc/configuration/rec_rpc_configuration_Configuration.hpp"
#include "rec/rpc/rec_rpc_Exception.h"

using namespace rec::rpc::configuration;

Configuration::Configuration( QObject* parent )
: QObject( parent )
, _mutex( QMutex::Recursive )
, _curId( 0 )
, _batchJobStarted(false)
{
}

Configuration::Configuration( const Configuration& other )
: _mutex( QMutex::Recursive )
{
	QMutexLocker lk( &other._mutex );
	_itemFromId = other._itemFromId;
	_idFromName = other._idFromName;
	_curId = other._curId;
	_batchJobStarted = other._batchJobStarted;
}

Configuration& Configuration::operator=( const Configuration& other )
{
	{
		QMutexLocker lk( &_mutex );
		QMutexLocker lk2( &other._mutex );
		_itemFromId = other._itemFromId;
		_idFromName = other._idFromName;
		_curId = other._curId;
	}

	Q_EMIT changed();
	return *this;
}

bool Configuration::isEmpty() const
{
	QMutexLocker lk( &_mutex );
	return _itemFromId.isEmpty();
}

unsigned int Configuration::nextId()
{
	QMutexLocker lk( &_mutex );
	unsigned int id = _curId;
	++_curId;
	_curId %= 0x3FFFFFFF;
	return id;
}

bool Configuration::contains( const QString& name ) const
{
	QMutexLocker lk( &_mutex );
	return _idFromName.contains( name );
}

bool Configuration::contains( unsigned int id ) const
{
	QMutexLocker lk( &_mutex );
	return _itemFromId.contains( id );
}

QList< unsigned int > Configuration::ids() const
{
	QMutexLocker lk( &_mutex );
	return _itemFromId.keys();
}

QStringList Configuration::names() const
{
	QMutexLocker lk( &_mutex );
	return _idFromName.keys();
}

Item Configuration::item( const QString& name ) const
{
	QMutexLocker lk( &_mutex );
	return _itemFromId.value( _idFromName.value( name ) );
}

Item Configuration::item( unsigned int id ) const
{
	QMutexLocker lk( &_mutex );
	return _itemFromId.value( id );
}

Item& Configuration::itemRef( const QString& name )
{
	QMap< QString, unsigned int >::const_iterator iter = _idFromName.find( name );
	if ( iter == _idFromName.end() )
		throw Exception( rec::rpc::NoSuchTopic, name );
	return _itemFromId[ iter.value() ];
}

Item& Configuration::itemRef( unsigned int id )
{
	QMap< unsigned int, Item >::iterator iter = _itemFromId.find( id );
	if ( iter == _itemFromId.end() )
		throw Exception( rec::rpc::NoSuchTopic, QString::number( id ) );
	return iter.value();
}

unsigned int Configuration::id( const QString& name ) const
{
	QMutexLocker lk( &_mutex );
	return _idFromName.value( name );
}

QString Configuration::name( unsigned int id ) const
{
	QMutexLocker lk( &_mutex );
	return _itemFromId.value( id ).name;
}

bool Configuration::isInitialized( const QString& name ) const
{
	QMutexLocker lk( &_mutex );

	if( false == _idFromName.contains( name ) )
	{
		return false;
	}

	return _itemFromId[ _idFromName[ name ] ].isInitialized();
}

bool Configuration::isInitialized( unsigned int id ) const
{
	QMutexLocker lk( &_mutex );

	if( false == _itemFromId.contains( id ) )
	{
		return false;
	}

	return _itemFromId[ id ].isInitialized();
}

QByteArray Configuration::data( const QString& name, rec::rpc::ClientInfo* publisher ) const
{
	QMutexLocker lk( &_mutex );

	if( false == _idFromName.contains( name ) )
	{
		publisher->address = QHostAddress::Null;
		publisher->port = 0;
		return QByteArray();
	}

	return _itemFromId[ _idFromName[ name ] ].data( publisher );
}

QByteArray Configuration::data( unsigned int id, rec::rpc::ClientInfo* publisher ) const
{
	QMutexLocker lk( &_mutex );

	if( false == _itemFromId.contains( id ) )
	{
		publisher->address = QHostAddress::Null;
		publisher->port = 0;
		return QByteArray();
	}

	return _itemFromId[ id ].data( publisher );
}

bool Configuration::isServerOnly( const QString& name ) const
{
	QMutexLocker lk( &_mutex );

	return _itemFromId[ _idFromName[ name ] ].serverOnly;
}

bool Configuration::isEnqueuedTopic( const QString& name ) const
{
	QMutexLocker lk( &_mutex );
	return _itemFromId[ _idFromName[ name ] ].isEnqueuedTopic;
}

bool Configuration::isEnqueuedTopic( unsigned int id ) const
{
	QMutexLocker lk( &_mutex );
	return _itemFromId[ id ].isEnqueuedTopic;
}

bool Configuration::setData( const QString& name, const QByteArray& data, const rec::rpc::ClientInfo& publisher )
{
	QMutexLocker lk( &_mutex );

	if( false == _idFromName.contains( name ) )
	{
		return false;
	}

	_itemFromId[ _idFromName[ name ] ].setData( data, publisher );

	return true;
}

bool Configuration::setData( unsigned int id, const QByteArray& data, const rec::rpc::ClientInfo& publisher )
{
	QMutexLocker lk( &_mutex );

	if( false == _itemFromId.contains( id ) )
	{
		return false;
	}

	_itemFromId[ id ].setData( data, publisher );

	return true;
}

rec::rpc::ClientInfoSet Configuration::registeredClients( const QString& name ) const
{
	QMutexLocker lk( &_mutex );

	if ( false == _idFromName.contains( name ) )
		return rec::rpc::ClientInfoSet();

	return _itemFromId[ _idFromName[ name ] ].registeredClients;
}

bool Configuration::isClientRegistered( const QString& name, const rec::rpc::ClientInfo& clientInfo ) const
{
	QMutexLocker lk( &_mutex );

	if ( false == _idFromName.contains( name ) )
		return false;

	return _itemFromId[ _idFromName[ name ] ].registeredClients.contains( clientInfo );
}

bool Configuration::isClientRegistered( unsigned int id, const rec::rpc::ClientInfo& clientInfo ) const
{
	QMutexLocker lk( &_mutex );

	if ( false == _itemFromId.contains( id ) )
		return false;

	return _itemFromId[ id ].registeredClients.contains( clientInfo );
}

rec::rpc::ClientInfoSet Configuration::addRegisteredClient( const QString& name, const rec::rpc::ClientInfo& clientInfo )
{
	QMutexLocker lk( &_mutex );

	if ( false == _idFromName.contains( name ) )
		return rec::rpc::ClientInfoSet();

	rec::rpc::ClientInfoSet& clients = _itemFromId[ _idFromName[ name ] ].registeredClients;
	clients.insert( clientInfo );
	return clients;
}

rec::rpc::ClientInfoSet Configuration::addRegisteredClient( unsigned int id, const rec::rpc::ClientInfo& clientInfo )
{
	QMutexLocker lk( &_mutex );

	if ( false == _itemFromId.contains( id ) )
		return rec::rpc::ClientInfoSet();

	rec::rpc::ClientInfoSet& clients = _itemFromId[ id ].registeredClients;
	clients.insert( clientInfo );
	return clients;
}

rec::rpc::ClientInfoSet Configuration::removeRegisteredClient( const QString& name, const rec::rpc::ClientInfo& clientInfo )
{
	QMutexLocker lk( &_mutex );

	if ( false == _idFromName.contains( name ) )
		return rec::rpc::ClientInfoSet();

	rec::rpc::ClientInfoSet& clients = _itemFromId[ _idFromName[ name ] ].registeredClients;
	clients.remove( clientInfo );
	return clients;
}

rec::rpc::ClientInfoSet Configuration::removeRegisteredClient( unsigned int id, const rec::rpc::ClientInfo& clientInfo )
{
	QMutexLocker lk( &_mutex );

	if ( false == _itemFromId.contains( id ) )
		return rec::rpc::ClientInfoSet();

	rec::rpc::ClientInfoSet& clients = _itemFromId[ id ].registeredClients;
	clients.remove( clientInfo );
	return clients;
}

void Configuration::clearRegisteredClients( const QString& name )
{
	QMutexLocker lk( &_mutex );

	if ( false == _idFromName.contains( name ) )
		return;

	_itemFromId[ _idFromName[ name ] ].registeredClients.clear();
}

void Configuration::clearRegisteredClients( unsigned int id )
{
	QMutexLocker lk( &_mutex );

	if ( false == _itemFromId.contains( id ) )
		return;

	_itemFromId[ id ].registeredClients.clear();
}

bool Configuration::addItem( unsigned int id, const QString& name, bool serverOnly, bool permanent, bool enqueued, const QString& sharedMemKey )
{
	bool ret;

	{
		QString n = name;
		if ( id & LocalFlag )
			n.append( "__local" );
		else if ( id & InfoFlag )
			n.append( "__info" );

		QMutexLocker lk( &_mutex );
		ret = addItem_i( id, n, serverOnly, permanent, enqueued, sharedMemKey  );
	}

	if( ret )
	{
		if (false == _batchJobStarted) Q_EMIT changed();
		return true;
	}

	return false;
}

bool Configuration::addItem_i( unsigned int id, const QString& name, bool serverOnly, bool permanent, bool enqueued, const QString& sharedMemKey )
{
	if( name.isEmpty() )
	{
		return false;
	}

	if( _idFromName.contains( name ) )
	{
		return false;
	}

	_idFromName[ name ] = id;
	_itemFromId[ id ] = Item( id, name, serverOnly, permanent, enqueued );
	_itemFromId[ id ].sharedMemKey = sharedMemKey;

	return true;
}

bool Configuration::removeItem( const QString& name )
{
	{
		QMutexLocker lk( &_mutex );

		if( false == _idFromName.contains( name ) )
		{
			return false;
		}

		unsigned int id = _idFromName.take( name );
		_itemFromId.remove( id );
	}

	if (false == _batchJobStarted) Q_EMIT changed();

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

		if( false == _idFromName.contains( oldName ) )
		{
			return false;
		}

		if( _idFromName.contains( newName ) )
		{
			return false;
		}

		unsigned int id = _idFromName.take( oldName );
		_itemFromId[ id ].name = newName;
		_idFromName[ newName ] = id;
	}

	if (false == _batchJobStarted) Q_EMIT changed();

	return true;
}

void Configuration::beginBatchJob()
{
	_batchJobStarted = true;
}

void Configuration::endBatchJob()
{
	_batchJobStarted = false;
	Q_EMIT changed();
}

QByteArray Configuration::save() const
{
	QMutexLocker lk( &_mutex );

	QDomDocument doc;
	QDomElement root = doc.createElement( "configuration" );
	doc.appendChild( root );

	QMap< unsigned int, Item >::const_iterator iter = _itemFromId.constBegin();

	while( _itemFromId.constEnd() != iter )
	{
		const Item& item = iter.value();

		QDomElement e = doc.createElement( "item" );
		e.setAttribute( "id", item.id );
		e.setAttribute( "name", item.name );

		if( item.serverOnly )
		{
			e.setAttribute( "serveronly", "true" );
		}

		if( false == item.sharedMemKey.isEmpty() )
		{
			e.setAttribute( "sharedMemKey", item.sharedMemKey );
		}

		if( item.permanent )
		{
			e.setAttribute( "permanent", "true" );
		}

		if( item.isEnqueuedTopic )
		{
			e.setAttribute( "enqueued", "true" );
		}

		root.appendChild( e );

		++iter;
	}

	return doc.toByteArray();
}

bool Configuration::load( const QByteArray& data )
{
	{
		QMutexLocker lk( &_mutex );

		_idFromName.clear();
		_itemFromId.clear();

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

			if( e.isNull() || false == e.hasAttribute( "id" ) || false == e.hasAttribute( "name" ) )
			{
				continue;
			}

			bool serverOnly = false;
			QString sharedMemKey;
			bool permanent = false;
			bool enqueued = false;

			bool ok = true;
			unsigned int id = e.attribute( "id" ).toUInt( &ok );
			if ( false == ok )
			{
				continue;
			}

			QString name = e.attribute( "name" );

			if ( e.hasAttribute( "serveronly" ) )
			{
				if ( e.attribute( "serveronly", "false" ).trimmed().toLower() == "true" )
					serverOnly = true;
			}

			if ( e.hasAttribute( "sharedMemKey" ) )
			{
				sharedMemKey = e.attribute( "sharedMemKey" );
			}

			if ( e.hasAttribute( "permanent" ) )
			{
				if ( e.attribute( "permanent", "false" ).trimmed().toLower() == "true" )
					permanent = true;
			}

			if ( e.hasAttribute( "enqueued" ) )
			{
				if ( e.attribute( "enqueued", "false" ).trimmed().toLower() == "true" )
					enqueued = true;
			}

			if( false == addItem_i( id, name, serverOnly, permanent, enqueued, sharedMemKey ) )
			{
				return false;
			}
		}
	}

	Q_EMIT changed();

	return true;
}
