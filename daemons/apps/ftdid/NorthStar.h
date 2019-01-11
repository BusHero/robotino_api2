#ifndef _NorthStar_H_
#define _NorthStar_H_

#include <QtCore>

class NS1;
class NS2;

class NorthStar : public QObject
{
	Q_OBJECT
public:
	NorthStar();

	bool open( const QString& device );

	void close();

	void setOrientation( int orientation ) { _orientation = orientation; }

public Q_SLOTS:
	void start();

Q_SIGNALS:
	void set_northstar_parameters_signal( unsigned int, float );

public Q_SLOTS:
	/*
	@param ceilingCal The distance in m from detector to ceiling.
	*/
	void on_set_northstar_parameters_changed( unsigned int, float ceilingCal );

private Q_SLOTS:
	void on_report( float pose_x,
				float pose_y,
				unsigned short r,
				float pose_theta,
				unsigned short numGoodSpots,
				unsigned short spot0_mag,
				unsigned short spot1_mag,
				unsigned int sequenceNumber,
				int roomReported );

Q_SIGNALS:
	void report( float pose_x,
				float pose_y,
				unsigned short r,
				float pose_theta,
				unsigned short numGoodSpots,
				unsigned short spot0_mag,
				unsigned short spot1_mag,
				unsigned int sequenceNumber,
				int roomReported );
private:
	unsigned int room() const;
	float ceilingCal() const;

	NS1* _ns1;
	NS2* _ns2;

	int _orientation;
};

#endif //_NorthStar_H_
