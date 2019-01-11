#include "TestEthernetPage.h"
#include "Config.h"
#include "LogList.h"
#include "Com.h"

TestEthernetPage::TestEthernetPage()
	: IOTestPage( false )
	, _socket( NULL )
{
	setTitle( "Test der LPC2378-Ethernetverbindung." );

	setTimerInterval( Config::singleton().ethernetTimeout() );
}

void TestEthernetPage::on_socket_readyRead()
{
	
}

bool TestEthernetPage::startButtonClicked()
{

	_socket = new QUdpSocket;

	if( false == _socket->bind( Config::singleton().ethernetPort() ) )
	{
		addLogEntry( LogEntry::FATAL, QString("Datenempfang auf Port %1 nicht möglich.").arg( Config::singleton().ethernetPort() ) );
		delete _socket;
		_socket = NULL;
		return false;
	}
	else
	{
		addLogEntry( LogEntry::INFO, QString("Empfange Daten auf Port %1.").arg( Config::singleton().ethernetPort() ) );
	}
	
	QByteArray ba;
	ba.resize( 3 );
	ba[0] = 5; //ea09com_RFV_t;
	ba[1] = 0;
	ba[2] = 0;

	addLogEntry( LogEntry::INFO, QString("Sende Daten zu %1 auf Port %2.").arg( Config::singleton().ethernetRemoteAddress() ).arg( Config::singleton().ethernetRemotePort() ) );
	_socket->writeDatagram( ba, QHostAddress( Config::singleton().ethernetRemoteAddress() ), Config::singleton().ethernetRemotePort() );

	return true;
}

void TestEthernetPage::timerTimeout()
{
	QByteArray data( 256, 0 );
	QHostAddress addr;
	quint16 port;

	int numRead = _socket->readDatagram( data.data(), data.size(), &addr, &port );

	if( numRead > 3 )
	{
		switch( data[0] )
		{
		case 6://ea09com_FV_t
			{
				unsigned int size = 0;
				size = ( data[1] << 8 );
				size |= data[2];

				if( size > (unsigned int)( numRead - 3 ) )
				{
					addLogEntry( LogEntry::FATAL, QString("Falsche Nachricht empfangen.") );
				}

				QString str;

				for( unsigned int i=0; i<size; ++i )
				{
					str += data[i+3];
				}

				addLogEntry( LogEntry::FINE, QString("Firmware Version empfangen: %1").arg( str ) );
				setPassed(true);
			}
			break;

		default:
			break;
		}
	}
	else
	{
		addLogEntry( LogEntry::FATAL, QString("Keine Nachricht empfangen.") );
	}

	stopTimer();
}

void TestEthernetPage::timerStopped()
{
	QByteArray ba;
	ba.resize( 3 );
	ba[0] = 9;//ea09com_UR_t;
	ba[1] = 0;
	ba[2] = 0;

	_socket->writeDatagram( ba, QHostAddress( Config::singleton().ethernetRemoteAddress() ), Config::singleton().ethernetRemotePort() );

	delete _socket;
	_socket = NULL;
}
