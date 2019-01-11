#ifndef _CONFIGHANDLER_H_
#define _CONFIGHANDLER_H_

#include <QtCore>

QByteArray createConfigData();

bool writeConfigData(const QByteArray jsonData, QByteArray& response);

bool restartDaemons(const QByteArray jsonData, QByteArray& response);

#endif //_CONFIGHANDLER_H_
