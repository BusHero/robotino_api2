#ifndef _CAN_HANDLER_
#define _CAN_HANDLER_

#include <QtCore>
#include <QMetaType>
#include <Windows.h>
#include "Pcan_usb.h"

class CANMessage 
{
public:
	CANMessage()
		: ID( 0 )
		,MSGTYPE( 0 )
		,LEN( 0 )
	{
	}

	CANMessage( const TPCANMsg& msg )
		: ID( msg.ID )
		, MSGTYPE( msg.MSGTYPE )
		, LEN( msg.LEN )
	{
		for( int i=0; i<LEN; ++i )
		{
			data.append( msg.DATA[i] );
		}
	}

	QString toString() const
	{
		QString str = QString("ID:%1 TYPE:%2 LEN:%3 DATA:").arg( ID ).arg( MSGTYPE ).arg( LEN );
		for( int i=0; i<data.size(); ++i )
		{
			str += QString(" %1").arg( (unsigned char)data[i], 2, 16, QChar( '0' ) );
		}
		return str;
	};

	unsigned long  ID;        // 11/29 bit identifier
	int  MSGTYPE;   // Bits from MSGTYPE_*
	int  LEN;       // Data Length Code of the Msg (0..8)
	QByteArray data;
};

Q_DECLARE_METATYPE(CANMessage)

class CANHandler : public QObject
{
	Q_OBJECT
public:
	CANHandler( QObject* parent );

	static QString statusString( DWORD status );

Q_SIGNALS:
	void messageReceived( const CANMessage& );
	void info( const QString& );
	void readingsChanged( int, const QVariantMap& );

private Q_SLOTS:
	void on_readTimer_timeout();

private:
	QTimer* _readTimer;
};

#endif //_CAN_HANDLER_
