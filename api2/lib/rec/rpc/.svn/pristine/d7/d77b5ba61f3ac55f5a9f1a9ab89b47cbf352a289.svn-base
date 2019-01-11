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

#ifndef _REC_RPC_CONFIGURATION_CONFIGURATION_H_
#define _REC_RPC_CONFIGURATION_CONFIGURATION_H_

#include "rec/rpc/defines.h"
#include "rec_rpc_configuration_Item.hpp"

#include <QtCore>
#include <QtNetwork>
#include <QtXml>

namespace rec
{
	namespace rpc
	{
		namespace configuration
		{
			class ConfigurationLocker;

			class REC_RPC_EXPORT Configuration : public QObject
			{
				Q_OBJECT
				friend class ConfigurationLocker;
			public:
				static const unsigned int InfoFlag = 0x80000000;
				static const unsigned int LocalFlag = 0x40000000;

				Configuration( QObject* parent = NULL );

				Configuration( const Configuration& other );

				Configuration& operator=( const Configuration& other );

				bool isEmpty() const;

				unsigned int nextId();

				bool contains( const QString& name ) const;
				bool contains( unsigned int id ) const;

				QList< unsigned int > ids() const;
				QStringList names() const;

				Item item( const QString& name ) const;
				Item item( unsigned int id ) const;

				Item& itemRef( const QString& name ); // Don't forget to lock!!!
				Item& itemRef( unsigned int id ); // Don't forget to lock!!!

				unsigned int id( const QString& name ) const;
				QString name( unsigned int id ) const;

				bool isInitialized( const QString& name ) const;
				bool isInitialized( unsigned int id ) const;

				QByteArray data( const QString& name, rec::rpc::ClientInfo* publisher ) const;
				QByteArray data( unsigned int id, rec::rpc::ClientInfo* publisher ) const;
				bool setData( const QString& name, const QByteArray& data, const rec::rpc::ClientInfo& publisher );
				bool setData( unsigned int id, const QByteArray& data, const rec::rpc::ClientInfo& publisher );

				bool isServerOnly( const QString& name ) const;

				bool isEnqueuedTopic( const QString& name ) const;
				bool isEnqueuedTopic( unsigned int id ) const;

				rec::rpc::ClientInfoSet registeredClients( const QString& name ) const;
				bool isClientRegistered( const QString& name, const rec::rpc::ClientInfo& clientInfo ) const;
				bool isClientRegistered( unsigned int id, const rec::rpc::ClientInfo& clientInfo ) const;
				rec::rpc::ClientInfoSet addRegisteredClient( const QString& name, const rec::rpc::ClientInfo& clientInfo );
				rec::rpc::ClientInfoSet addRegisteredClient( unsigned int id, const rec::rpc::ClientInfo& clientInfo );
				rec::rpc::ClientInfoSet removeRegisteredClient( const QString& name, const rec::rpc::ClientInfo& clientInfo );
				rec::rpc::ClientInfoSet removeRegisteredClient( unsigned int id, const rec::rpc::ClientInfo& clientInfo );
				void clearRegisteredClients( const QString& name );
				void clearRegisteredClients( unsigned int id );

				/**
				@return Returns false if the name is already in use of if name or type are empty strings.
				*/
				bool addItem( unsigned int id, const QString& name, bool serverOnly = false, bool permanent = false, bool enqueued = false, const QString& sharedMemKey = QString::null );

				bool removeItem( const QString& name );

				bool renameItem( const QString& oldName, const QString& newName );

				void beginBatchJob();
				void endBatchJob();

				QByteArray save() const;

				bool load( const QByteArray& data );

			Q_SIGNALS:
				void changed();

			private:
				bool addItem_i( unsigned int id, const QString& name, bool serverOnly, bool permanent, bool enqueued, const QString& sharedMemKey );
				
				QMap< unsigned int, Item > _itemFromId;
				QMap< QString, unsigned int > _idFromName;
				mutable QMutex _mutex;

				unsigned int _curId;
				bool _batchJobStarted;
			};

			class ConfigurationLocker
			{
			public:
				ConfigurationLocker( Configuration& configuration )
					: _lk( &configuration._mutex )
				{
				}

			private:
				QMutexLocker _lk;
			};
		}
	}
}

#include <QMetaType>
Q_DECLARE_METATYPE( rec::rpc::configuration::Configuration )
static QDebug operator<<( QDebug dbg, const rec::rpc::configuration::Configuration& cfg )
{
	dbg.nospace() << cfg.names();

	return dbg.space();
}

#endif //_REC_RPC_CONFIGURATION_CONFIGURATION_H_
