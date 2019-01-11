#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "rec/robotino/rpc/Client.h"

class Client : public rec::robotino::rpc::Client
{
	Q_OBJECT
public:
	Client();

Q_SIGNALS:

public Q_SLOTS:
	void on_northstar_report( float pose_x,
				float pose_y,
				unsigned short r,
				float pose_theta,
				unsigned short numGoodSpots,
				unsigned short spot0_mag,
				unsigned short spot1_mag,
				unsigned int sequenceNumber,
				int roomReported );

	void on_cruizcore_report( float angle, float rate );


private Q_SLOTS:
	void on_connected();
	void on_disconnected();

private:
	bool _connected;
};

#endif //_CLIENT_H_
