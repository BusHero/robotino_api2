#include "ScriptManager.h"
#include "Client.h"

extern Client* client;

ScriptManager::ScriptManager()
{
}

bool ScriptManager::setNetwork( const QString& address, const QString& netmask )
{
	QString command = client->getParameter( "~/scriptpath" ).toString() + "/set_network.pl";
	QProcess process;
	process.start( command, QStringList() << address << netmask );
	return process.waitForFinished( 2000 );
}

bool ScriptManager::setNetworkDyn()
{
	QString command = client->getParameter( "~/scriptpath" ).toString() + "/set_network_dyn.pl";
	QProcess process;
	process.start( command, QStringList() );
	return process.waitForFinished( 2000 );
}
