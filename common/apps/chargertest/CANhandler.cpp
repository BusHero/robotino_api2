#include "CANHandler.h"
#include "Pcan_usb.h"

#define CAN_ID_BASE 0x501

CANHandler::CANHandler( QObject* parent )
	: QObject( parent )
	, _readTimer( new QTimer( this ) )
{
	connect( _readTimer, SIGNAL( timeout() ), SLOT( on_readTimer_timeout() ) );
	_readTimer->setSingleShot( false );
	_readTimer->setInterval( 100 );
	_readTimer->start();
}

void CANHandler::on_readTimer_timeout()
{
	static QVector<unsigned int> chargerTime;
	static QVector<int> majorVer;
	static QVector<int> minorVer;
	static QVector<int> patchVer;

	TPCANMsg msg;
	DWORD result = CAN_Read(&msg);
	if( CAN_ERR_OK == result )
	{
		Q_EMIT messageReceived( CANMessage( msg ) );

		if( 0 == ((msg.ID-CAN_ID_BASE)%0x20) ) //Info
		{
			int chargerID = (msg.ID-CAN_ID_BASE) / 0x20;
			//Q_EMIT stateChanged( chargerID, msg.DATA[7] );

			QVariantMap map;

			unsigned int time = msg.DATA[0];
			time |= msg.DATA[1] << 8;
			map[ "time" ] = time;

			if( chargerID > chargerTime.count() )
			{
				chargerTime.resize( chargerID );
				majorVer.resize( chargerID );
				minorVer.resize( chargerID );
				patchVer.resize( chargerID );
			}

			if( (time > chargerTime[chargerID-1] && time-chargerTime[chargerID-1]>5) 
				|| (time < chargerTime[chargerID-1] && chargerTime[chargerID-1]-time>5) )
			{
				majorVer[chargerID-1] = 0;
				minorVer[chargerID-1] = 0;
				patchVer[chargerID-1] = 0;
			}

			chargerTime[chargerID-1] = time;

			if( 0 == majorVer[chargerID-1] )
			{
				TPCANMsg getVersionMsg;
				getVersionMsg.ID = (msg.ID + 0x10);
				getVersionMsg.LEN = 0;
				getVersionMsg.MSGTYPE = MSGTYPE_STANDARD;
				CAN_Write(&getVersionMsg);
			}

			float voltage = msg.DATA[2];
			voltage += (msg.DATA[3] << 8);
			voltage /= 1000;
			map[ "voltage" ] = voltage;

			float current = msg.DATA[4];
			current /= 100;
			map[ "current" ] = current;

			float temp1 = msg.DATA[5];
			map[ "temp1" ] = temp1;

			float temp2 = msg.DATA[5];
			map[ "temp2" ] = temp2;

			map[ "state" ] = (int)msg.DATA[7];

			map[ "version" ] = QString("%1.%2.%3").arg( majorVer[chargerID-1] ).arg( minorVer[chargerID-1] ).arg( patchVer[chargerID-1] );

			Q_EMIT readingsChanged( chargerID, map );
		}
		else if( 0 == ((msg.ID-CAN_ID_BASE-2)%0x20) ) //Version
		{
			int chargerID = (msg.ID-CAN_ID_BASE-2) / 0x20;

			if( chargerID > majorVer.count() )
			{
				majorVer.resize( chargerID );
				minorVer.resize( chargerID );
				patchVer.resize( chargerID );
			}

			majorVer[chargerID-1] = msg.DATA[0];
			minorVer[chargerID-1] = msg.DATA[1];
			patchVer[chargerID-1] = msg.DATA[2];
		}
	}
}

QString CANHandler::statusString( DWORD status )
{
	switch( status )
	{
	case CAN_ERR_OK:
		return "Ok";
 
	case CAN_ERR_XMTFULL:
		return "Send buffer overrun";
 
	case CAN_ERR_OVERRUN:
		return "CAN controller read to late";
 
	case CAN_ERR_BUSLIGHT:
		return "Error counter reached Light-limit";
 
	case CAN_ERR_BUSHEAVY:
		return "Error counter reached Heavy-limit";
 
	case CAN_ERR_BUSOFF:
		return "Bus-Off";
 
	case CAN_ERR_QRCVEMPTY:
		return "Receive-Queue empty";
 
	case CAN_ERR_QOVERRUN:
		return "Receive-Queue read to late";
 
	case CAN_ERR_QXMTFULL:
		return "Send queue full";
 
	case CAN_ERR_REGTEST:
		return "Register test 82C200/SJA1000 error";
 
	case CAN_ERR_NODRIVER:
		return "Drive not loaded";
 
	case CAN_ERR_HWINUSE:
		return "Hardware already in use";
 
	case CAN_ERR_NETINUSE:
		return "A client is already connected to the network";
 
	case CAN_ERR_ILLHW:
		return "Hardware-Handle invalid";
 
	case CAN_ERR_ILLNET:
		return "Net handle invalid";
 
	case CAN_ERR_ILLCLIENT:
		return "Client-Handle invalid";
 
	case CAN_ERR_RESOURCE:
		return "Ressource (Queue, Client, Timer) can not be created";
 
	case CAN_ERR_ILLPARAMTYPE:
		return "Parameter not allowed";
 
	case CAN_ERR_ILLPARAMVAL:
		return "Parameter value invalid";
 
	default:
		return "Unkown error";
	}
}

