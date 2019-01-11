#ifndef _SCRIPTMANAGER_H_
#define _SCRIPTMANAGER_H_

#include <QtCore>

class ScriptManager : public QObject
{
	Q_OBJECT
public:
	ScriptManager();

	bool setNetwork( const QString& address, const QString& netmask );
	bool setNetworkDyn();

Q_SIGNALS:
	void log( const QString&, int );

private:
};

#endif //_SCRIPTMANAGER_H_
