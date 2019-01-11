#ifndef _UTILS_H_
#define _UTILS_H_

#include <QtCore>

void getHostapdConf( QString* ssid, QString* cipher, QString* passphrase );

/**
If isDHCP==true then address and netmask are empty strings.
mode can be master or client.
*/
void getInterfacesWLAN0Conf( bool* isDHCP, QString* address, QString* netmask, QString* mode );

void getWpaSupplicantConf( QString* ssid, QString* proto, QString* psk );

#endif //_UTILS_H_
