#include "rec/robotino/server/v1/messages/KeepAlive.h"

using namespace rec::robotino::server::v1::messages;

QByteArray KeepAlive::encode()
{
	QByteArray ba;
	ba.resize( 3 );

	ba[0] = 3;
	ba[1] = 0;
	ba[2] = 0;

	return ba;
}