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

#include "rec/rpc/rec_rpc_Socket.hpp"
#include "rec/rpc/messages/rec_rpc_messages_Message.h"
#include "rec/rpc/rec_rpc_common.h"
#include "rec/rpc/rec_rpc_common_internal.hpp"
#include "rec/rpc/rec_rpc_Server.h"

#include <cassert>

using namespace rec::rpc;

quint16 Socket::Id::id = 0;
QMutex Socket::Id::mutex;

quint16 Socket::Id::getNext()
{
	QMutexLocker lk( &mutex );
	if ( ++id == 0 )
		++id;
	return id;
}

Socket::Socket( const QString& name )
: _tcpSocket( 0 )
, _localSocket( 0 )
, _currentSocket( 0 )
, _localSocketId( 0 )
, _socketName( name )
, _receiveState( GreetingReceiveState )
, _bytesToRead( messages::headerSize )
, _messageBuf( rec::rpc::messages::headerSize, 0 )
, _writeBusyTimer( NULL )
{
}

Socket::~Socket()
{
	QMutexLocker lk( &_topicMutex );
	_topicQueue.clear();
	_topicMap.clear();
}

void Socket::publishTopic( unsigned int id, const QByteArray& metaData, const QByteArray& source, const QByteArray& payload, bool enqueue )
{
	QMutexLocker lk( &_topicMutex );
	//qDebug() << "publishTopic " << name << "q:" << _topicQueue.size() << " m:" << _topicMap.size();

	if( _topicQueue.isEmpty() )
	{
		qApp->postEvent( this, new QEvent( QEvent::Type( TopicEvent ) ) );
	}

	if( false == _topicMap.contains( id ) )
	{
		_topicQueue.enqueue( id );
	}

	QQueue< QByteArray >& data = _topicMap[ id ];
	if ( false == enqueue )
	{
		data.clear();
	}
	data.enqueue( metaData );
	if ( false == source.isEmpty() )
		data.enqueue( source );
	data.enqueue( payload );
}

QAbstractSocket::SocketState Socket::state() const
{
	if( _tcpSocket )
		return _tcpSocket->state();
	if( _localSocket )
		return (QAbstractSocket::SocketState)_localSocket->state();
	return QAbstractSocket::UnconnectedState;
}

QString Socket::errorString() const
{
	if( _tcpSocket )
		return _tcpSocket->errorString();
	if( _localSocket )
		return _localSocket->errorString();
	return QString::null;
}

QHostAddress Socket::localAddress() const
{
	if ( _tcpSocket )
		return _tcpSocket->localAddress();
	return QHostAddress::Null;
}

quint16 Socket::localPort() const
{
	if ( _tcpSocket )
		return _tcpSocket->localPort();
	return _localSocketId;
}

QHostAddress Socket::peerAddress() const
{
	if ( _tcpSocket )
		return _tcpSocket->peerAddress();
	return QHostAddress::Null;
}

quint16 Socket::peerPort() const
{
	if ( _tcpSocket )
		return _tcpSocket->peerPort();
	return _localSocketId;
}

void Socket::setSocketName( const QString& name )
{
	_socketName = name;
}

void Socket::setTcpSocket( QTcpSocket* socket )
{
	_receiveState = GreetingReceiveState;
	_bytesToRead = messages::headerSize;

	assert( !_tcpSocket && !_localSocket );
	_tcpSocket = socket;
	_tcpSocket->setParent( this );
	_currentSocket = _tcpSocket;

	bool ok = true;
	ok &= (bool)connect( _tcpSocket,
		SIGNAL( stateChanged( QAbstractSocket::SocketState ) ),
		SLOT( on_tcpSocket_stateChanged( QAbstractSocket::SocketState ) ),
		Qt::DirectConnection );
	ok &= (bool)connect( _tcpSocket,
		SIGNAL( error( QAbstractSocket::SocketError ) ),
		SLOT( on_tcpSocket_error( QAbstractSocket::SocketError ) ),
		Qt::DirectConnection );
	ok &= (bool)connect( _tcpSocket, SIGNAL( disconnected() ), SIGNAL( disconnected() ), Qt::DirectConnection );
	assert( ok );
}

void Socket::setLocalSocket( QLocalSocket* socket )
{
	_receiveState = GreetingReceiveState;
	_bytesToRead = messages::headerSize;

	assert( !_tcpSocket && !_localSocket );
	_localSocket = socket;
	_localSocket->setParent( this );
	_currentSocket = _localSocket;

	_localSocketId = Id::getNext();

	bool ok = true;
	ok &= (bool)connect( _localSocket,
		SIGNAL( stateChanged( QLocalSocket::LocalSocketState ) ),
		SLOT( on_localSocket_stateChanged( QLocalSocket::LocalSocketState ) ),
		Qt::DirectConnection );
	ok &= (bool)connect( _localSocket,
		SIGNAL( error( QLocalSocket::LocalSocketError ) ),
		SLOT( on_localSocket_error( QLocalSocket::LocalSocketError ) ),
		Qt::DirectConnection );
	ok &= (bool)connect( _localSocket, SIGNAL( disconnected() ), SIGNAL( disconnected() ), Qt::DirectConnection );
	assert( ok );
}

void Socket::closeSocket()
{
	if ( _tcpSocket )
	{
		_tcpSocket->close();
		if ( _tcpSocket )
			_tcpSocket->deleteLater();
		_tcpSocket = 0;
	}
	if ( _localSocket )
	{
		_localSocket->close();
		if ( _localSocket )
			_localSocket->deleteLater();
		_localSocket = 0;
	}
	_currentSocket = 0;
}

void Socket::on_localSocket_error( QLocalSocket::LocalSocketError err )
{
	Q_EMIT error( (QAbstractSocket::SocketError)err );
}

void Socket::on_localSocket_stateChanged( QLocalSocket::LocalSocketState state )
{
	if( QLocalSocket::UnconnectedState == state )
	{
		closeSocket();
	}

	Q_EMIT stateChanged( (QAbstractSocket::SocketState)state );
}

void Socket::on_tcpSocket_error( QAbstractSocket::SocketError err )
{
	Q_EMIT error( err );
}

void Socket::on_tcpSocket_stateChanged( QAbstractSocket::SocketState state )
{
	if( QAbstractSocket::UnconnectedState == state )
	{
		closeSocket();
	}

	Q_EMIT stateChanged( state );
}

void Socket::customEvent( QEvent* ev )
{
	if ( ev->type() == QEvent::Type( TopicEvent ) )
	{
		topicEvent();
		ev->accept();
	}
	else
	{
		ev->ignore();
	}
}

#include <iostream>
void Socket::topicEvent()
{
	if( tcpSocket()  )
	{
		if( tcpSocket()->bytesToWrite() > 0 )
		{
			if( NULL == _writeBusyTimer )
			{
				_writeBusyTimer = new QTime();
				_writeBusyTimer->start();
			}

			if( _writeBusyTimer->elapsed() > Server::sendFailSocketTimeout )
			{
				closeSocket();
				return;
			}
#ifdef DEBUG_SOCKET
			qDebug() << "bytes to write " << tcpSocket()->bytesToWrite();
#endif //DEBUG_SOCKET
			qApp->postEvent( this, new QEvent( QEvent::Type( TopicEvent ) ) );
			return;
		}
	}
	else if( localSocket() )
	{
		if( localSocket()->bytesToWrite() > 0 )
		{
			if( NULL == _writeBusyTimer )
			{
				_writeBusyTimer = new QTime();
				_writeBusyTimer->start();
			}

			if( _writeBusyTimer->elapsed() > Server::sendFailSocketTimeout )
			{
				closeSocket();
				return;
			}
#ifdef DEBUG_SOCKET
			qDebug() << "bytes to write " << localSocket()->bytesToWrite();
#endif //DEBUG_SOCKET
			qApp->postEvent( this, new QEvent( QEvent::Type( TopicEvent ) ) );
			return;
		}
	}

	if( _writeBusyTimer )
	{
		delete _writeBusyTimer;
		_writeBusyTimer = NULL;
	}

	QMutexLocker lk( &_topicMutex );
	Q_ASSERT( false == _topicQueue.isEmpty() );

	unsigned int id = _topicQueue.dequeue();
	Q_ASSERT( _topicMap.contains( id ) );

	QQueue< QByteArray > data = _topicMap.take( id );

#ifdef DEBUG_SOCKET
	qDebug() << "write topic " << name << " bytes:" << data.size();
#endif //DEBUG_SOCKET

	if( tcpSocket() )
	{
		while( false == data.isEmpty() )
			tcpSocket()->write( data.dequeue() );
	}
	else if ( localSocket() )
	{
		while( false == data.isEmpty() )
			localSocket()->write( data.dequeue() );
	}
	else
	{
		_topicMap.clear();
		_topicQueue.clear();
		return;
	}

	if( _topicQueue.isEmpty() )
	{
		Q_ASSERT( _topicMap.isEmpty() );
		return;
	}
	qApp->postEvent( this, new QEvent( QEvent::Type( TopicEvent ) ) );
}
