#ifndef _PROGRAMHANDLER_
#define _PROGRAMHANDLER_

#include <QtCore>

QByteArray createProgramData();

QByteArray createGlobalVarsData();

QByteArray createRobviewOutData();

bool renameProgram(const QByteArray jsonData, QByteArray& response);

bool programClicked(const QByteArray jsonData, QByteArray& response);
bool removeClicked(const QByteArray jsonData, QByteArray& response);

#endif //_PROGRAMHANDLER_