/*
 * UdpLogger.h
 *
 *  Created on: Dec 9, 2013
 *      Author: shao
 */

#ifndef UDPLOGGER_H_
#define UDPLOGGER_H_

#include <QObject>
#include <QtNetwork>

/**
 * Used to send log messages to udp ports.
 */
class UdpLogger : public QObject
{
    Q_OBJECT
public:
    static UdpLogger* Singleton();

    QString ip;
    int port;
    bool broadcastLogViaUDP;
    int verbocityLevel;

public Q_SLOTS:
    void broadcastLog(const QString&, int );

private:
    explicit UdpLogger(QObject *parent = 0);

    void sendMsg(const QString& msg,int verbosity);

	QMutex _mutex;
	QUdpSocket* _socket;
};


#endif /* UDPLOGGER_H_ */
