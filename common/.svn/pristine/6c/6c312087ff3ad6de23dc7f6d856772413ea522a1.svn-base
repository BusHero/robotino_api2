#ifndef _REC_ROBOTINO_SERVER_V1_SERVER_H_
#define _REC_ROBOTINO_SERVER_V1_SERVER_H_

#include "rec/robotino/server/defines.h"
#include "rec/robotino/server/ServerImpl.h"
#include "rec/iocontrol/remotestate/SensorState.h"
#include "rec/iocontrol/remotestate/SetState.h"

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QTime>
#include <QHostAddress>

namespace rec
{
	namespace robotino
	{
		namespace server
		{
			namespace v1
			{
				class Server : public rec::robotino::server::ServerImpl
				{
					Q_OBJECT
				public:
					Server( bool isMaster );

					/**
					Copy the sensor state to the server.
					*/
					void setSensorState( const rec::iocontrol::remotestate::SensorState& sensorState );

					void run( QTcpSocket& socket, const QByteArray& alreadyReceived );

			Q_SIGNALS:
					void setStateReceived( const rec::iocontrol::remotestate::SetState& );

					void cameraControlReceived( unsigned int width, unsigned int height );

					void clientImageRequest( bool enable, quint32 address, quint16 port );

				private:
					bool readMessageData( QTcpSocket& socket, const QByteArray& headerData );

					void cleanup();

					int _readTimeout;

					rec::iocontrol::remotestate::SensorState _sensorState;

					QTime _setSensorStateTimer;
					QMutex _mutex;
					QWaitCondition _sensorStateSet;

					rec::iocontrol::remotestate::SetState _setState;

					bool _isImageRequest;
					quint32 _imageReceiverAddress;
					quint16 _imageReceiverPort;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO_SERVER_V1_SERVER_H_
